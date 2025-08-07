#include <cstddef>
#include <cstdint>

#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <numbers>

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
	Mesh sineLine;
	Mesh refQuad;
	Texture texYellow;
	Texture texWhite;
	ShaderProgram basicProg;

	TransformStack model = {"model"};
	TransformStack view = {"view"};
	TransformStack projection = {"projection"};

	OrbitCamera camera = {4, 0, 0, 0};
	float t = 0;
	int numSegments = 4;

	// Appelée avant la première trame.
	void init() override {
		setKeybindMessage(
			"F5 : capture d'écran." "\n"
			"R : réinitialiser la position de la caméra." "\n"
			"+ et - :  rapprocher et éloigner la caméra orbitale." "\n"
			"haut/bas : changer la latitude de la caméra orbitale." "\n"
			"gauche/droite : changer la longitude ou le roulement (avec shift) de la caméra orbitale." "\n"
			"clic central (cliquer la roulette) : bouger la caméra en glissant la souris." "\n"
			"1 et 2 : diminuer/augmenter le nombres de segments de la courbe." "\n"
		);

		// Config de base, pas de cull, lignes assez visibles.
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glPointSize(3.0f);
		glLineWidth(4.0f);
		glClearColor(0.1f, 0.2f, 0.2f, 1.0f);

		loadShaders();

		texYellow = Texture::createFromColor({1, 1, 0.5f, 1});
		texWhite = Texture::createFromColor({1, 1, 1, 1});
		basicProg.use();
		basicProg.setInt("texMain", 0);

		refQuad.vertices = {
			{{0, -1.1f, 0}, {}, {}},
			{{1, -1.1f, 0}, {}, {}},
			{{1,  1.1f, 0}, {}, {}},
			{{0,  1.1f, 0}, {}, {}},
		};
		refQuad.setup();

		sineLine.vertices = {
			{{0.00f,  0, 0}, {}, {}},
			{{0.25f,  1, 0}, {}, {}},
			{{0.50f,  0, 0}, {}, {}},
			{{0.75f, -1, 0}, {}, {}},
			{{1.00f,  0, 0}, {}, {}},
		};
		sineLine.setup();

		camera.updateProgram(basicProg, view);
		applyPerspective();
	}

	// Appelée à chaque trame. Le buffer swap est fait juste après.
	void drawFrame() override {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		using std::numbers::pi;

		// Mise à jour du temps.
		t += 0.5f * getFrameDeltaTime();

		// Mise à jour du maillage.
		sineLine.vertices.resize(numSegments + 1);
		for (int i = 0; i < numSegments + 1; i++) {
			// Position du sommet à modifier
			auto& position = sineLine.vertices[i].position;
			// Segmenter de façon à garder une courbe qui va en x de 0 à 1 (inclu)
			position.x = /* TODO */;
			// La formule de l'onde est sin(2π(t + x))
			position.y = /* TODO */;
		}
		sineLine.setup();

		// Affichage de la courbe et du cadre.
		model.push(); {
			model.scale({3, 1, 1});
			model.translate({-0.5f, 0, 0});
			basicProg.setMat(model);
		} model.pop();
		texWhite.bindToTextureUnit(0);
		refQuad.draw(GL_LINE_LOOP);
		texYellow.bindToTextureUnit(0);
		sineLine.draw(GL_LINE_STRIP);
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

		camera.handleKeyEvent(key, 5, 0.5, {4, 0, 0, 0});
		camera.updateProgram(basicProg, view);

		using enum sf::Keyboard::Key;
		switch (key.code) {
		case Num1:
			numSegments -= 1;
			numSegments = std::max(numSegments, 3);
			std::cout << "Nb segments : " << numSegments << std::endl;
			break;
		case Num2:
			numSegments += 1;
			numSegments = std::max(numSegments, 3);
			std::cout << "Nb segments : " << numSegments << std::endl;
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
	app.run(argc, argv, "Introduction Semaine 8: Tessellation", settings);
}
