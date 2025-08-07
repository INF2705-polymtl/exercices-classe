#version 410


uniform sampler2D texMain;
uniform vec3 lightPosition; // Position de la lumière dans la scène.


in vec2 texCoords;
in vec3 sceneCoords; // Position du fragment dans la scène.


out vec4 fragColor;


void main() {
	fragColor = texture(texMain, texCoords);
	// Le facteur à appliquer est 1/d².
	float distance = /* TODO */;
	fragColor.rgb *= /* TODO */;
}
