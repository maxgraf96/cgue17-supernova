#version 430

in vec3 fragNormal;
in vec3 fragPos;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

layout(location = 0) out vec4 fragColor;

/* fragColor is the final output color */
void main() {
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(light.position - fragPos);

	/* Ambient */
	vec3 ambient = material.ambient * light.ambient;

	/* Diffuse */
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * material.diffuse) * light.diffuse;
	
	/* Specular */ 
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = (material.specular * spec) * light.specular;

	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0f);
}