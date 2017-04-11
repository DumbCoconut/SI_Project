#version 330

in vec3 l;
in vec3 n;
out vec4 bufferColor;

void main() {
    // colors
    vec3 ambientColor = vec3(0.2, 0.2, 0.5);
    vec3 diffuseColor = vec3(0.2, 0.2, 0.5);
    vec3 specularColor = vec3(1.0, 1.0, 1.0);
    
    // Note: The normal vector should be renormalized when received after rasterization
    vec3 n_norm = normalize(n);
    vec3 r = reflect(l, n_norm);
    vec3 e = vec3(0.0, 0.0, -1.0);

    float rdote = dot(r, e);
    float ndotl = dot(n_norm, l);
    float ec = 10;

    bufferColor = vec4((specularColor * pow(rdote, ec)) + (diffuseColor * ndotl) + ambientColor, 1.0);
}
