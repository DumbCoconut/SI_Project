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

  vec3 ambientColor = vec3(0.2, 0.2, 0.5);
  vec3 diffuseColor = vec3(0.2, 0.2, 0.5);
  vec3 specularColor = vec3(1.0, 1.0, 1.0);

  vec3 n = normalMat * normal; // normal of the surface 
  vec3 r = reflect(light, n);
  vec3 e = vec3(0.0, 0.0, -1.0);

  float rdote = dot(r,e); 
  float ndotl = dot(n, light); 
  float ec = 10;

  fragmentColor = vec4((specularColor * pow(rdote, ec)) + (diffuseColor * ndotl) + ambientColor, 1.0);
}
