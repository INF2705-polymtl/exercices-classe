#version 410


uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);
uniform mat4 projection = mat4(1);


layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoords;
layout(location = 3) in vec4 a_color;


out vec4 color;


void main() {
	// Appliquer les transformations habituelles.
	vec4 worldPosition = model * vec4(a_position, 1.0);
	vec4 viewPosition = view * worldPosition;
	vec4 clipPosition = projection * viewPosition;

	gl_Position = clipPosition;
	color = a_color;
}
