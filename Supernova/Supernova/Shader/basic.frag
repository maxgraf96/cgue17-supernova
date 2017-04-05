#version 330

in vec3 fragNormal;

layout(location = 0) out vec4 fragColor;

void main() {
	fragColor = vec4((fragNormal + vec3(1)) * 0.5, 1);
}