#version 430
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D texColor;
uniform sampler2D texDepth;

uniform mat4 inverseVP;
uniform mat4 previousVP;
uniform float fps;
 
 vec3 depth(){
	float t2 = pow(texture(texDepth, texCoords).x , 256);	
	return vec3(t2,t2,t2);
}

void main() {
	float depthForPixel = texture(texDepth, texCoords).x;

	// H is the viewport position at this pixel in the range -1 to 1.
	vec4 H = vec4(texCoords.x * 2 - 1, (1 - texCoords.y) * 2 - 1, depthForPixel, 1);
	vec4 D = H * inverseVP;
	vec4 worldPosition = D / D.w;

	// Current viewport position
	vec4 currentPos = H;
	// Get previous position from previous VP matrix
	vec4 previousPos = worldPosition * previousVP;

	// Convert to points in [-1, 1] by dividing by w
	previousPos /= previousPos.w;

	// BLUR
	// Calculate pixel velocity from previous position
	vec2 velocity = ((currentPos - previousPos) / 64).xy;

	// At high framerates the blur becomes almost unnoticeable -> adjust the blur length i.e. "velocity" accordingly
	// That way the blur gives the impression of the game running at 30fps
	float scale = fps / 40;
	if(scale > 1.0f){ velocity *= scale; }
	else { velocity /= scale; }

	// Get the pixel color value
	vec4 color = texture(texColor, texCoords);
	vec2 curTexCoords = texCoords + velocity;

	// number of samples
	int numSamples = 8;
	for(int i = 1; i < numSamples; ++i, curTexCoords += velocity){
		// Sample color buffer along the velocity vector
		vec4 curColor = texture(texColor, curTexCoords);
		color += curColor;
	}

	// Calculate average
	fragColor = color/numSamples;
	
}