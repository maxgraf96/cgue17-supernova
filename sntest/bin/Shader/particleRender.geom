#version 430 

uniform struct Matrices 
{ 
   mat4 proj; 
   mat4 view; 
   mat4 myVP;
} matrices; 

uniform vec3 quad1, quad2; 

layout(points) in; 
layout(triangle_strip) out; 
layout(max_vertices = 4) out; 

in vec3 colorPass[]; 
in float lifeTimePass[]; 
in float sizePass[]; 
in int typePass[]; 

smooth out vec2 texCoord; 
flat out vec4 colorPart; 

void main() 
{ 
  if(typePass[0] != 0) 
  { 
    vec3 posOld = gl_in[0].gl_Position.xyz; 
    float size = sizePass[0]; 
    mat4 VP = matrices.proj*matrices.view; 
	//mat4 VP = matrices.myVP;

    colorPart = vec4(colorPass[0], lifeTimePass[0]); 
        
    vec3 pos = posOld+(-quad1-quad2)*size; 
    texCoord = vec2(0.0f, 0.0f); 
    gl_Position = VP*vec4(pos, 1.0f); 
    EmitVertex(); 
     
    pos = posOld+(-quad1+quad2)*size; 
    texCoord = vec2(0.0f, 1.0f); 
    gl_Position = VP*vec4(pos, 1.0f); 
    EmitVertex(); 
     
    pos = posOld+(quad1-quad2)*size; 
    texCoord = vec2(1.0f, 0.0f); 
    gl_Position = VP*vec4(pos, 1.0f); 
    EmitVertex(); 
     
    pos = posOld+(quad1+quad2)*size; 
    texCoord = vec2(1.0f, 1.0f); 
    gl_Position = VP*vec4(pos, 1.0f); 
    EmitVertex(); 
    EndPrimitive(); 
  } 
}