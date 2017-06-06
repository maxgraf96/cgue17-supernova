#version 330 

layout(points) in; 
layout(points) out; 
layout(max_vertices = 40) out; 

// All that we get from vertex shader

in vec3 positionPass[]; 
in vec3 velocityPass[]; 
in vec3 colorPass[]; 
in float lifeTimePass[]; 
in float sizePass[]; 
in int typePass[]; 

// All that we send further

out vec3 positionOut; 
out vec3 velocityOut; 
out vec3 colorOut; 
out float lifeTimeOut; 
out float sizeOut; 
out int typeOut; 

uniform vec3 genPosition; // Position where new particles are spawned
uniform vec3 genGravityVector; // Gravity vector for particles - updates velocity of particles 
uniform vec3 genVelocityMin; // Velocity of new particle - from min to (min+range)
uniform vec3 genVelocityRange; 

uniform vec3 genColor; 
uniform float genSize;  

uniform float genLifeMin, fGenLifeRange; // Life of new particle - from min to (min+range)
uniform float timePassed; // Time passed since last frame

uniform vec3 randomSeed; // Seed number for our random number function
vec3 localSeed; 

uniform int numToGenerate; // How many particles will be generated next time, if greater than zero, particles are generated

// This function returns random number from zero to one
float randZeroOne() 
{ 
    uint n = floatBitsToUint(localSeed.y * 214013.0 + localSeed.x * 2531011.0 + localSeed.z * 141251.0); 
    n = n * (n * n * 15731u + 789221u); 
    n = (n >> 9u) | 0x3F800000u; 
  
    float res =  2.0 - uintBitsToFloat(n); 
    localSeed = vec3(localSeed.x + 147158.0 * res, localSeed.y*res  + 415161.0 * res, localSeed.z + 324154.0*res); 
    return res; 
} 

void main() 
{ 
  localSeed = randomSeed; 
   
  // gl_Position doesn't matter now, as rendering is discarded, so I don't set it at all

  positionOut = positionPass[0]; 
  velocityOut = velocityPass[0]; 
  if(typePass[0] != 0)positionOut += velocityOut*timePassed; 
  if(typePass[0] != 0)velocityOut += genGravityVector*timePassed; 

  colorOut = colorPass[0]; 
  lifeTimeOut = lifeTimePass[0]-timePassed; 
  sizeOut = sizePass[0]; 
  typeOut = typePass[0]; 
     
  if(typeOut == 0) 
  { 
    EmitVertex(); 
    EndPrimitive(); 
     
    for(int i = 0; i < numToGenerate; i++) 
    { 
      positionOut = genPosition; 
      velocityOut = genVelocityMin+vec3(genVelocityRange.x*randZeroOne(), genVelocityRange.y*randZeroOne(), genVelocityRange.z*randZeroOne());
      colorOut = genColor; 
      lifeTimeOut = genLifeMin+genLifeRange*randZeroOne(); 
      sizeOut = genSize; 
      typeOut = 1; 
      EmitVertex(); 
      EndPrimitive(); 
    } 
  } 
  else if(lifeTimeOut > 0.0) 
  { 
      EmitVertex(); 
      EndPrimitive();  
  } 
}