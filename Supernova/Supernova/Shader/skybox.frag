#version 430

layout(location = 0) in vec3 textureCoordinates; 

uniform samplerCube skybox; // Sampler for cubemap texture

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main() {
	fragColor = texture(skybox, textureCoordinates);

	//we do not want to use bloom on the skybox
	brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}