R"(#version 400

in vec4 inMapSpace[4];

uniform bool f[13];
uniform int colorComp;
uniform int pageSize;
uniform sampler2D depthTexture;
uniform sampler2D depthSubTexture;
uniform mat4 mapProjections[4];

int mapSide = -1;
vec2 mapProjection;
float halfTexel = 0. / pageSize;

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
	if( f[1] )                                                                          // debug vis
	    return;
	FindMap();                                                                          // find the atlas page to sample from
	if( mapSide < 0 ) {		                                                            // show error as purple color
		gl_FragColor.rgb = vec3(1, 0, 1);	
		return;
	}
	float thisDepth = inMapSpace[mapSide].z / inMapSpace[mapSide].w * .5 + .5;
    vec2 proj2tex = (mapProjection * 0.5 + 0.5) * (1-2*halfTexel) + halfTexel;          // -1..1 to halfTexel..1-halfTexel
    proj2tex = proj2tex * vec2(1, 0.25) + vec2(0, 0.25 * mapSide);                      // page space to atlas space
	vec4 depthSample = texture(depthTexture, proj2tex);
	float lit = float( thisDepth < depthSample.r );
	gl_FragColor.rgb = vec3(1-lit, lit, 0);
    if( !f[3] ) { // anti-flicker for shadow edges
	    depthSample = textureGather(depthTexture, proj2tex);
        vec2 lit2 = vec2(lessThan(vec2(thisDepth), depthSample.xy));
        if( lit2[0] + lit2[1] == 0 ) {
        	gl_FragColor.rgb = vec3(1, 0, 0);
            return;
        }
        if( lit2[0] + lit2[1] == 2 ) {
        	gl_FragColor.rgb = vec3(0, 1, 0);
            return;
        }
    	float thisX = fract(pageSize*proj2tex.x + .5);
        if( lit2[0] == 0 ) {
	        float sampleX = textureGather(depthSubTexture, proj2tex, 1)[0];
            lit = float(thisX > sampleX);
	        gl_FragColor.rgb = vec3(1-lit, lit, 0);
        }
        if( lit2[1] == 0 ) {
	        float sampleX = textureGather(depthSubTexture, proj2tex)[1];
            lit = float(thisX < sampleX);
	        gl_FragColor.rgb = vec3(1-lit, lit, 0);
        }
    }
})"