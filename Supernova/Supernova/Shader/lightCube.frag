#version 430

in vec3 fragNormal;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main() {
	fragColor = vec4(1, 0.5f, 0.8f, 1);

	/*calculate bright areas*/
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.9) {
		brightColor = vec4(fragColor.rgb, 1.0);
	}
	else {
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}