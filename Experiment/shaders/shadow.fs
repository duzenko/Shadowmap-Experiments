R"(#version 130

in vec4 clipSpace;

uniform int pageSize;

void main() {
	gl_FragColor = gl_Color;	
    vec2 screenSpace = pageSize * clipSpace.xy / clipSpace.w;
    float subPixel = fract(screenSpace.x);
    vec2 tmp = vec2(subPixel, 1-subPixel);
	gl_FragColor.rg = tmp;
})"