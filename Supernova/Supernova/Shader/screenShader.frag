#version 430
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloom;

void main() 
{
	const float gamma = 2.2;
	vec3 color = texture(scene, texCoords).rgb;
	vec3 bloomColor = texture(bloom, texCoords).rgb;
	
	vec3 result = 0.5 * color + 0.5 * bloomColor;
	fragColor = vec4(result, 1.0);
}