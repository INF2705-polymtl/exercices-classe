#version 410


 // L'image sous forme de tableau de vec4. Le tableau est en rangée-major et le 0,0 est en bas à gauche.
uniform vec4 img[8 * 8];


// Les coordonnées originales des sommets. Elles sont interpolées aux fragments.
in vec3 position;


out vec4 fragColor;


void main() {
	int s = /* TODO */;
	int t = /* TODO */;
	fragColor = img[/* TODO */];
}
