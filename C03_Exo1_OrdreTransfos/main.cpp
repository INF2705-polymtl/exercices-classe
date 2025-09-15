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
	Mesh triangle;

	ShaderProgram basicProg;

	TransformStack view = {"view"};
	TransformStack projection = {"projection"};

	OrbitCamera camera = {5, 0, 0, 0};
	float angle = 0.0f;

	// Appelée avant la première trame.
	void init() override {
		setKeybindMessage(
			"F5 : capture d'écran." "\n"
			"R : réinitialiser la position de la caméra." "\n"
			"+ et - :  rapprocher et éloigner la caméra orbitale." "\n"
			"haut/bas : changer la latitude de la caméra orbitale." "\n"
			"gauche/droite : changer la longitude ou le roulement (avec shift) de la caméra orbitale." "\n"
			"clic central (cliquer la roulette) : bouger la caméra en glissant la souris." "\n"
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

		triangle.vertices = {
			{{ 1, -1, 0}, {}, {}, {0, 1, 0, 1}},
			{{ 0,  1, 0}, {}, {}, {0, 0, 1, 1}},
			{{-1, -1, 0}, {}, {}, {1, 0, 0, 1}},
		};
		triangle.setup();

		camera.updateProgram(basicProg, view);
		applyPerspective();
	}

	// Appelée à chaque trame. Le buffer swap est fait juste après.
	void drawFrame() override {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		basicProg.use();

		angle += 1; // Angle de rotation

		mat4 m = mat4(1); // Matrice identité
		// TODO: Appliquer les opérations dans le bon ordre
		// Voici les choix à réordonner :
		m = scale(m, {0.5, 1, 1});
		m = rotate(m, radians(angle), {0, 0, 1});
		m = translate(m, {0, 1, 0});

		basicProg.setMat("model", m);
		triangle.draw(GL_TRIANGLES);
	}

	// Appelée lorsque la fenêtre se ferme.
	void onClose() override {
		basicProg.deleteShaders();
		basicProg.deleteProgram();
	}

	// Appelée lors d'une touche de clavier.
	void onKeyPress(const sf::Event::KeyPressed& key) override {
		// La touche R réinitialise la position de la caméra.
		// Les touches + et - rapprochent et éloignent la caméra orbitale.
		// Les touches haut/bas change l'élévation ou la latitude de la caméra orbitale.
		// Les touches gauche/droite change la longitude ou le roulement (avec shift) de la caméra orbitale.

		camera.handleKeyEvent(key, 5, 0.5, {5, 0, 0, 0});
		camera.updateProgram(basicProg, view);

		using enum sf::Keyboard::Key;
		switch (key.code) {
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
	}

	// Appelée lors d'un défilement de souris.
	void onMouseScroll(const sf::Event::MouseWheelScrolled& mouseScroll) override {
		// Zoom in/out
		camera.altitude -= mouseScroll.delta;
		camera.updateProgram(basicProg, view);
	}

	// Appelée lorsque la fenêtre se redimensionne (juste après le redimensionnement).
	void onResize(const sf::Event::Resized& event) override {
		applyPerspective();
	}

	void applyPerspective(float fovy = 50) {
		projection.perspective(fovy, getWindowAspect(), 0.1f, 100.0f);
		basicProg.setMat(projection);
	}

	void loadShaders() {
		basicProg.create();
		basicProg.attachSourceFile(GL_VERTEX_SHADER, "basic_vert.glsl");
		basicProg.attachSourceFile(GL_FRAGMENT_SHADER, "basic_frag.glsl");
		basicProg.link();
	}
};


int main(int argc, char* argv[]) {
	WindowSettings settings = {};
	settings.fps = 30;
	settings.context.antiAliasingLevel = 4;

	App app;
	app.run(argc, argv, "Exercice cours 3 : Ordre des transformations", settings);
}
