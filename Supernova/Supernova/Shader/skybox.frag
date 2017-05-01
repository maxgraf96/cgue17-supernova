#version 430

layout(location = 0) in vec3 textureCoordinates; 
uniform samplerCube skybox; // Sampler for cubemap texture

out vec4 fragColor;

void main() {
	fragColor = texture(skybox, textureCoordinates);
}