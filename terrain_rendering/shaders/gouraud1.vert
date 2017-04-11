#version 330

// input attributes 
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 mdvMat; // modelview matrix (constant for all the vertices)
uniform mat4 projMat; // projection matrix (constant for all the vertices)

// special matrix for transforming a normal in the view space
// normal matrix = transpose(inverse(upperleft3x3(mdvMat))
// removes the (bad) scale effects contained in the mdv matrix
uniform mat3 normalMat; // normal matrix (constant for all the vertices)
uniform vec3 light; // light direction

// output variables that will be interpolated during rasterization (equivalent to varying)
out vec4 fragmentColor;

void main() {
  gl_Position = projMat*mdvMat*vec4(position,1.0);

  // Khaki
  float r = 0.941;
  float g = 0.902;
  float b = 0.549;

  vec3 n = normalMat * normal; // normal of the surface 
  vec3 color = vec3(r, g, b); // surface color
  fragmentColor = vec4(color * dot(n, light), 1.0);
}
