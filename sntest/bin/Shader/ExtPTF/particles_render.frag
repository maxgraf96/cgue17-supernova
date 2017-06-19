#version 330

uniform sampler2D gSampler;

smooth in vec2 vTexCoord;
flat in vec4 vColorPart;

layout(location = 0) out vec4 fragColor; 
layout(location = 1) out vec4 brightColor;

void main()
{
  vec4 vTexColor = texture2D(gSampler, vTexCoord);
  fragColor = vec4(vTexColor.xyz, 1.0)*vColorPart;

  /*calculate bright areas*/
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.9) {
		brightColor = vec4(fragColor.rgb, 1.0);
	}
	else {
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}