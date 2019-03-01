R"(#version 130

in vec4 inMapSpace[4];

uniform bool f;
uniform int colorComp;
uniform int pageSize;
uniform sampler2DShadow depthTexture;

int mapSide = -1;
vec2 mapProjection;

float halfTexel = 0.5 / pageSize;

void FindMap() {
	for(int i=0; i<4; i++) {
		if(inMapSpace[i].w < 0.)
			continue;
		mapProjection = inMapSpace[i].xy/inMapSpace[i].w;
		if(abs(mapProjection.x) > 1.)
			continue;
		mapSide = i;
		break;
	}
}

void main() {
	gl_FragColor = gl_Color;	
	if (f) {
		return;
	} 
	FindMap();
	if(mapSide < 0)	 {		// show error
		gl_FragColor.rgb = vec3(1, 0, 1);	
		return;
	}
	float thisDepth = inMapSpace[mapSide].z / inMapSpace[mapSide].w * .5 + .5;
#if 1
    vec2 proj2tex = (mapProjection * 0.5 + 0.5) * (1-2*halfTexel) + halfTexel;       // -1..1 to halfTexel..1-halfTexel
    proj2tex = proj2tex * vec2(1, 0.25) + vec2(0, 0.25 * mapSide);      // page space to atlas space
	vec3 depthTexCoord = vec3(proj2tex, thisDepth);
	float lit = texture(depthTexture, depthTexCoord);
    //lit = lit * 2 - 1;
#else
	float inShadow = float( thisDepth > depthSample.r );
#endif
	gl_FragColor.rgb = vec3(1-lit, lit, 0);
	//gl_FragColor.rg = proj2tex;
})"