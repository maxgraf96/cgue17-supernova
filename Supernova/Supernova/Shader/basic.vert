#version 330

in vec3 position;
in vec3 normal;

out vec3 fragNormal;

uniform mat4 model;
uniform mat4 proj;

void main() {
	fragNormal = (model * vec4(normal, 0)).xyz;
	gl_Position = proj * model * vec4(position, 1);
}