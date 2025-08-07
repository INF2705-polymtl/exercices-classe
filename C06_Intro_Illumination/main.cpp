#include <cstddef>
#include <cstdint>

#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <inf2705/OpenGLApplication.hpp>
#include <inf2705/Mesh.hpp>
#include <inf2705/ShaderProgram.hpp>
#include <inf2705/Texture.hpp>
#include <inf2705/TransformStack.hpp>
#include <inf2705/OrbitCamera.hpp>


using namespace gl;
using namespace glm;


struct App : public OpenGLApplication
{
	Mesh sphere;
	Mesh triangle;
	Texture texRock;
	Texture texYellow;
	ShaderProgram basicProg;
	ShaderProgram prog;

	TransformStack model = {"model"};
	TransformStack view = {"view"};
	TransformStack projection = {"projection"};

	OrbitCamera camera = {5, 30, 30, 0};

	vec3 lightPosition = {0, 0, 1};

	// Appelée avant la première trame.
	void init() override {
		setKeybindMessage(
			"F5 : capture d'écran." "\n"
			"R : réinitialiser la position de la caméra." "\n"
			"+ et - :  rapprocher et éloigner la caméra orbitale." "\n"
			"haut/bas : changer la latitude de la caméra orbitale." "\n"
			"gauche/droite : changer la longitude ou le roulement (avec shift) de la caméra orbitale." "\n"
			"clic central (cliquer la roulette) : bouger la caméra en glissant la souris." "\n"
			"WASD : Bouger la source de lumière dans le plan *xz*." "\n"
		);

		// Config de base, pas de cull, lignes assez visibles.
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(3.0f);
		glLineWidth(3.0f);
		glClearColor(0.1f, 0.2f, 0.2f, 1.0f);

		loadShaders();

		sphere = Mesh::loadFromWavefrontFile("sphere.obj")[0];
		triangle.vertices = {
			{{-1, -1, 0}, {}, {0, 0}},
			{{ 1, -1, 0}, {}, {1, 0}},
			{{ 0,  1.5, 0}, {}, {0.5f, 1}},
		};
		triangle.setup();

		texYellow = Texture::createFromColor({1, 1, 0.5f, 1});
		texYellow.bindToTextureUnit(0, basicProg, "texMain");
		texRock = Texture::loadFromFile("rock.png", 5);
		texRock.bindToTextureUnit(1, prog, "texMain");

		camera.updateProgram(basicProg, view);
		camera.updateProgram(prog, view);
		applyPerspective();
	}

	// Appelée à chaque trame. Le buffer swap est fait juste après.
	void drawFrame() override {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		basicProg.use();
		texYellow.bindToTextureUnit(0);
		model.push(); {
			model.translate(lightPosition);
			model.scale({0.2f, 0.2f, 0.2f});
			basicProg.setMat(model);
		} model.pop();
		sphere.draw();

		prog.use();
		texRock.bindToTextureUnit(1);
		prog.setVec("lightPosition", lightPosition);
		triangle.draw();
	}

	// Appelée lorsque la fenêtre se ferme.
	void onClose() override {
		basicProg.deleteShaders();
		basicProg.deleteProgram();
		prog.deleteShaders();
		prog.deleteProgram();
		sphere.deleteObjects();
		triangle.deleteObjects();
	}

	// Appelée lors d'une touche de clavier.
	void onKeyPress(const sf::Event::KeyPressed& key) override {
		// La touche R réinitialise la position de la caméra.
		// Les touches + et - rapprochent et éloignent la caméra orbitale.
		// Les touches haut/bas change l'élévation ou la latitude de la caméra orbitale.
		// Les touches gauche/droite change la longitude ou le roulement (avec shift) de la caméra orbitale.

		camera.handleKeyEvent(key, 5, 0.5, {5, 30, 30, 0});
		camera.updateProgram(basicProg, view);
		camera.updateProgram(prog, view);

		using enum sf::Keyboard::Key;
		switch (key.code) {
		case W:
			lightPosition.z -= 0.2f;
			break;
		case S:
			lightPosition.z += 0.2f;
			break;
		case A:
			lightPosition.x -= 0.2f;
			break;
		case D:
			lightPosition.x += 0.2f;
			break;
		case F5: {
			auto path = saveScreenshot();
			std::cout << "Capture d'écran dans " << path << std::endl;
			break;
		}}
	}

	// Appelée lors d'un mouvement de souris.
	void onMouseMove(const sf::Event::MouseMoved& mouseDelta) override {
		// Mettre à jour la caméra si on a un clic de la roulette.
		auto& mouse = getMouse();
		camera.handleMouseMoveEvent(mouseDelta, mouse, deltaTime_ / (0.7f / 30));
		camera.updateProgram(basicProg, view);
		camera.updateProgram(prog, view);
	}

	// Appelée lors d'un défilement de souris.
	void onMouseScroll(const sf::Event::MouseWheelScrolled& mouseScroll) override {
		// Zoom in/out
		camera.altitude -= mouseScroll.delta;
		camera.updateProgram(basicProg, view);
		camera.updateProgram(prog, view);
	}

	// Appelée lorsque la fenêtre se redimensionne (juste après le redimensionnement).
	void onResize(const sf::Event::Resized& event) override {
		applyPerspective();
	}

	void applyPerspective(float fovy = 50) {
		projection.perspective(fovy, getWindowAspect(), 0.1f, 1000.0f);
		basicProg.use();
		basicProg.setMat(projection);
		prog.use();
		prog.setMat(projection);
	}

	void loadShaders() {
		basicProg.create();
		basicProg.attachSourceFile(GL_VERTEX_SHADER, "vert.glsl");
		basicProg.attachSourceFile(GL_FRAGMENT_SHADER, "basic_frag.glsl");
		basicProg.link();

		prog.create();
		prog.attachSourceFile(GL_VERTEX_SHADER, "vert.glsl");
		prog.attachSourceFile(GL_FRAGMENT_SHADER, "lit_frag.glsl");
		prog.link();
	}
};


int main(int argc, char* argv[]) {
	WindowSettings settings = {};
	settings.fps = 30;
	settings.context.antiAliasingLevel = 4;

	App app;
	app.run(argc, argv, "Intro Semaine 6: Illumination", settings);
}
