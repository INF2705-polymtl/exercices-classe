#version 410


uniform float angle = 0; // Angle en degrés qui change dans le temps.


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertColor;


out vec4 color;


void main() {
	gl_Position = vec4(pos, 1);
	float rads = radians(angle);
	gl_Position.x = /* TODO */;
	gl_Position.y = /* TODO */;
	color = vec4(vertColor, 1);
}
