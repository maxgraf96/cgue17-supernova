#version 430

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoords; 

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

uniform struct Material {
	sampler2D texture_diffuse;
    sampler2D texture_specular;
	float shininess;
} material;

#define DIR_LIGHTS_NR 12
uniform struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int initialized;
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

#define SPOT_LIGHTS_NR 12
uniform struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 color;
	float cutoff;
	float outercutoff;
	int initialized;
};

uniform DirectionalLight dirLights[DIR_LIGHTS_NR];
uniform PointLight pointLights[POINT_LIGHTS_NR];
uniform SpotLight spotLights[SPOT_LIGHTS_NR];
uniform vec3 cameraPos;

// Calculate directional lights
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);

	/* Ambient */
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, texCoords));

	/* Diffuse */
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * texture(material.texture_diffuse, texCoords).rgb * light.diffuse;
	
	/* Specular */ 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow( max(dot(viewDir, reflectDir), 0.0f), material.shininess );
	vec3 specular = texture(material.texture_specular, texCoords).rgb * spec * light.specular;

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

// Calculate spot lights
vec3 calculateSpotLights(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutoff - light.outercutoff;
	float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0, 1.0);    
	float col = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = texture(material.texture_diffuse, texCoords).rgb * light.color * col;
	return diffuse * intensity;
}

void main() {

	// Fragment properties
	vec3 normal = normalize(fragNormal);
	vec3 viewDir = normalize(cameraPos - fragPos);

	vec3 result = vec3(0.0f);

	// Add directional lights
	for(int i = 0; i < DIR_LIGHTS_NR; i++){
		if(dirLights[i].initialized < 1) continue;
		result += calculateDirectionalLight(dirLights[i], normal, viewDir);
	}

	// Add point lights
	for(int i = 0; i < POINT_LIGHTS_NR; i++){
		if(pointLights[i].initialized < 1) continue;
		result += calculatePointLights(pointLights[i], normal, fragPos, viewDir);
	}

	// Add spot lights
	for(int i = 0; i < SPOT_LIGHTS_NR; i++){
		if(spotLights[i].initialized < 1) continue;
		result += calculateSpotLights(spotLights[i], normal, fragPos, viewDir);
	}

	fragColor = vec4(result, 1.0f);

	/*calculate bright areas*/
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.9) {
		brightColor = vec4(fragColor.rgb, 1.0);
	}
	else {
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}