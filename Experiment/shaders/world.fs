R"(#version 130

in vec4 inMapSpace[4];

uniform bool f;
uniform int colorComp;
uniform sampler2DShadow depthTexture;

int mapSide = -1;
float mapProjection;

void FindMap() {
	for(int i=0; i<4; i++) {
		if(inMapSpace[i].w < 0.)
			continue;
		mapProjection = inMapSpace[i].x/inMapSpace[i].w;
		if(mapProjection < -1.)
			continue;
		if(mapProjection > 1.)
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
	vec3 depthTexCoord = vec3(mapProjection * .5 + .5, 0.25 * mapSide + 0.125, thisDepth);
	float lit = texture(depthTexture, depthTexCoord);
    lit = lit * 2 - 1;
#else
	float inShadow = float( thisDepth > depthSample.r );
#endif
	gl_FragColor.rgb = vec3(1-lit, lit, 0);
})"