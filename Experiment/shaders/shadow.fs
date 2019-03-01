R"(#version 130

in vec4 clipSpace;

void main() {
	gl_FragColor = gl_Color;	
	gl_FragColor.rg = clipSpace.xy;
})"