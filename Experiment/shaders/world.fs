R"(#version 130

in vec4 inMapSpace[4];

uniform bool f;
uniform int colorComp;
uniform int pageSize;
#if 0
uniform sampler2DShadow depthTexture;
#else
uniform sampler2D depthTexture;
#endif
uniform sampler2D depthSubTexture;
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
	if (f)                                                                              // debug vis
	    return;
	FindMap();                                                                          // find the atlas page to sample from
	if(mapSide < 0)	 {		                                                            // show error as purple color
		gl_FragColor.rgb = vec3(1, 0, 1);	
		return;
	}
	float thisDepth = inMapSpace[mapSide].z / inMapSpace[mapSide].w * .5 + .5;
    vec2 proj2tex = (mapProjection * 0.5 + 0.5) * (1-2*halfTexel) + halfTexel;          // -1..1 to halfTexel..1-halfTexel
    proj2tex = proj2tex * vec2(1, 0.25) + vec2(0, 0.25 * mapSide);                      // page space to atlas space
#if 0
	vec3 depthTexCoord = vec3(proj2tex, thisDepth);
	float lit = texture(depthTexture, depthTexCoord);
    //lit = lit * 2 - 1;
#else
    vec2 depthTexCoord = proj2tex;
	vec4 depthSample = texture(depthTexture, depthTexCoord);
	float lit = float( thisDepth < depthSample.r );
#endif
	gl_FragColor.rgb = vec3(1-lit, lit, 0);
	//gl_FragColor.rg = proj2tex;
})"