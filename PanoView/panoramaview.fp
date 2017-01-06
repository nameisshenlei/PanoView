// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;
uniform sampler2D colorMap;


smooth in vec2 vTexCoords;

void main(void)
{ 
    vFragColor = texture(colorMap, vTexCoords);
}
    