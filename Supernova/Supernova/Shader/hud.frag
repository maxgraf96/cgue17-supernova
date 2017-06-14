#version 430
in vec2 textureCoordinates;
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

uniform sampler2D text;
uniform vec3 textColor;

void main(){
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, textureCoordinates).r);
	color = vec4(textColor, 1.0) * sampled;

	//we do not want to use bloom on HUD
	brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}