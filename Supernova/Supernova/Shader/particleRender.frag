#version 430 

uniform sampler2D sampler; 

smooth in vec2 texCoord; 
flat in vec4 colorPart; 

layout(location = 0) out vec4 fragColor; 
layout(location = 1) out vec4 brightColor;

void main() 
{ 
  vec4 texColor = texture(sampler, texCoord); 
  fragColor = vec4(texColor.xyz, 1.0f)*colorPart; 

  /*calculate bright areas*/
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.9) {
		brightColor = vec4(fragColor.rgb, 1.0);
	}
	else {
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}