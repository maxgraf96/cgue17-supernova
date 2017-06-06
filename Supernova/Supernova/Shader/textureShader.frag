#version 430

in vec2 texCoords; 

out vec4 fragColor;

struct Material {
	sampler2D texture_diffuse;
    sampler2D texture_specular;
	float shininess;
};

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform float material_shininess;

// Use later to implement specular, compare with basic.frag!
uniform Material material;


void main() {
	fragColor = vec4(
	vec3(texture(material.texture_diffuse, texCoords))
	//+ vec3(texture(material.texture_specular, texCoords))
	, 1);
}