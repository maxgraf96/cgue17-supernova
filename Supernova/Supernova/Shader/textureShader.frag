#version 430

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoords; 

out vec4 fragColor;

uniform struct Material {
	sampler2D texture_diffuse;
    sampler2D texture_specular;
	float shininess;
} material;

uniform struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// Probably never more than 20 point lights in game
#define POINT_LIGHTS_NR 20 
uniform struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
	int initialized;
};

uniform DirectionalLight dirLight;
uniform PointLight pointLights[POINT_LIGHTS_NR];
uniform vec3 cameraPos;

// Calculate directional light - there is only one - our supernova
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);

	/* Ambient */
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, texCoords));

	/* Diffuse */
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse, texCoords))) * light.diffuse;
	
	/* Specular */ 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow( max(dot(viewDir, reflectDir), 0.0f), material.shininess );
	vec3 specular = (vec3(texture(material.texture_specular, texCoords)) * spec) * light.specular;

	return (ambient + diffuse + specular);
}

// Calculate point lights
vec3 calculatePointLights(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);

	/* Ambient */
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, texCoords));

	/* Diffuse */
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse, texCoords))) * light.diffuse;
	
	/* Specular */ 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow( max(dot(viewDir, reflectDir), 0.0f), material.shininess );
	vec3 specular = (vec3(texture(material.texture_specular, texCoords)) * spec) * light.specular;

	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

void main() {

	// Fragment properties
	vec3 normal = normalize(fragNormal);
	vec3 viewDir = normalize(cameraPos - fragPos);

	// Add directional light
	vec3 result = calculateDirectionalLight(dirLight, normal, viewDir);

	// Add point lights
	for(int i = 0; i < POINT_LIGHTS_NR; i++){
		if(pointLights[i].initialized < 1) continue;
		result += calculatePointLights(pointLights[i], normal, fragPos, viewDir);
	}

	fragColor = vec4(result, 1.0f);
}