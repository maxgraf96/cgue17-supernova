#version 430
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloom;
uniform sampler2D lensflares;
uniform sampler2D motionBlur;

const float offset = 1.0 / 300.0; 
vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

void main() 
{
	const float gamma = 2.2;
	vec3 color = texture(scene, texCoords).rgb;
	vec3 bloomColor = texture(bloom, texCoords).rgb;

	//should not do blur in this shader!
	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(lensflares, texCoords.st + offsets[i]));
    }
    vec3 lensflaresColor = vec3(0.0);
    for(int i = 0; i < 9; i++)
	{
        lensflaresColor += sampleTex[i] * kernel[i];
	}

	vec3 motionBlurColor = texture(motionBlur, texCoords).rgb;
	
	//TODO: correct addition!
	vec3 result = 0.5 * color + 0.2 * bloomColor + 0.1 * lensflaresColor + 0.2 * motionBlurColor;

	fragColor = vec4(result, 1.0);
}