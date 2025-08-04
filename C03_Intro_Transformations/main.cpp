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

	float angle = 0;

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
			{{-0.5f, -0.5f, 0.0f}, {1, 0, 0}, {}},
			{{ 0.5f, -0.5f, 0.0f}, {0, 1, 0}, {}},
			{{ 0.0f,  0.5f, 0.0f}, {0, 0, 1}, {}},
		};
		triangle.setup();
	}

	// Appelée à chaque trame. Le buffer swap est fait juste après.
	void drawFrame() override {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		angle += 1;
		basicProg.use();
		basicProg.setFloat("angle", angle);
		triangle.draw();
	}

	// Appelée lorsque la fenêtre se ferme.
	void onClose() override {
		basicProg.deleteShaders();
		basicProg.deleteProgram();
	}

	// Appelée lors d'une touche de clavier.
	void onKeyPress(const sf::Event::KeyPressed& key) override {
		using enum sf::Keyboard::Key;
		switch (key.code) {
		case F5: {
			auto path = saveScreenshot();
			std::cout << "Capture d'écran dans " << path << std::endl;
			break;
		}}
	}

	void loadShaders() {
		basicProg.create();
		basicProg.attachSourceFile(GL_VERTEX_SHADER, "vert.glsl");
		basicProg.attachSourceFile(GL_FRAGMENT_SHADER, "frag.glsl");
		basicProg.link();
	}
};


int main(int argc, char* argv[]) {
	WindowSettings settings = {};
	settings.fps = 30;
	settings.context.antiAliasingLevel = 4;

	App app;
	app.run(argc, argv, "Introduction cours 3 : Transformations", settings);
}
