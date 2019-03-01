R"(#version 130

in vec4 clipSpace;

uniform int pageSize;

void main() {
	gl_FragColor = gl_Color;	
    vec2 screenSpace = pageSize * (clipSpace.xy / clipSpace.w * .5 + .5);
    float subPixel = fract(screenSpace.x);
    vec2 tmp = vec2(1-subPixel, subPixel);
	gl_FragColor.rg = tmp;
	//gl_FragColor.rg = screenSpace / pageSize;
})"