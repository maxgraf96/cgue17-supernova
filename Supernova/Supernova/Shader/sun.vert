#version 430

in vec3 position;
in vec3 normal;

uniform mat4 model;
uniform mat4 proj;

out vec3 fragNormal;
out vec3 fragPos;

void main() {
	gl_Position = proj * model * vec4(position, 1);
	fragPos = vec3(model * vec4(position, 1.0f));
	fragNormal = mat3(transpose(inverse(model))) * normal;
}