#version 430

in vec3 position;
in vec3 normal;

out vec3 textureCoordinates;

uniform mat4 model;
uniform mat4 proj;

void main() {
	vec4 pos = proj * model * vec4(position, 1.0);
	gl_Position = pos.xyww;
	textureCoordinates = position;
}