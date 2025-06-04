#version 410


uniform mat4 P,V,M;

in vec3 pos;

out vec4 color;

void main() {
	color = vec4(pos * 0.5 + vec3(0.5), 1);
	gl_Position = P*V*M * vec4(pos, 1);
}
