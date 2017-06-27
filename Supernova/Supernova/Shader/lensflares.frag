#version 430
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D input;
uniform sampler2D lensflaresColor;

uniform int ghosts; // number of ghost samples
uniform float ghostDispersal; // dispersion factor
uniform float haloWidth; //width of halo
uniform float distortion; //distortion factor

vec3 textureDistorted(in sampler2D tex, in vec2 texcoord, in vec2 direction, in vec3 distortion) {
      return vec3(
         texture(tex, texcoord + direction * distortion.r).r,
         texture(tex, texcoord + direction * distortion.g).g,
         texture(tex, texcoord + direction * distortion.b).b
      );
}

void main() {
    vec2 coords = -texCoords + vec2(1.0);
    vec2 texelSize = 1.0 / vec2(textureSize(input, 0));
 
	// ghost vector to image centre:
    vec2 ghostVec = (vec2(0.5) - texCoords) * ghostDispersal; //use texCoords or coords?

	vec3 distortion = vec3(-texelSize.x * distortion, 0.0, texelSize.x * distortion);
	vec2 direction = normalize(ghostVec);
   
	// sample ghosts:  
    vec4 result = vec4(0.0);
    for (int i = 0; i < ghosts; i++) { //originally it was ++i; try if this doesn't work
        vec2 offset = fract(coords + ghostVec * float(i));

		float weight = length(vec2(0.5) - offset) / length(vec2(0.5));
		weight = pow(1.0 - weight, 10.0);
  
        result += vec4(textureDistorted(input, offset, direction, distortion) * weight, 1.0);
    }

	//sample halo
	vec2 haloVec = normalize(ghostVec) * haloWidth;
	float weight = length(vec2(0.5) - fract(texCoords + haloVec)) / length(vec2(0.5));
	weight = pow(1.0 - weight, 5.0);
	result += vec4(textureDistorted(input, texCoords + haloVec, direction, distortion) * weight, 1.0);

	//wrong multiplication
	result *= vec4(textureDistorted(lensflaresColor, texCoords, direction, distortion) * length(vec2(0.5) - texCoords) / length(vec2(0.5)), 1.0);
    fragColor = result;
}