#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoordinates;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 proj;

void main() {
	gl_Position = proj * model * vec4(position, 1.0f);
	fragPos = vec3(model * vec4(position, 1.0f));
	fragNormal = mat3(transpose(inverse(model))) * normal;
	texCoords = textureCoordinates;
}