#pragma once

float mapSideNear[4] = { .1f,.1f,.1f,.1f };
Vec mapCorners[4] = {
	{ -.1f, .1f },
	{ .1f, .1f },
	{ .1f, -.1f },
	{ -.1f, -.1f },
};

ViewPort vpDefault;

struct Framebuffer {
	GLuint glHandle, depthTexture;
	ViewPort viewPort = {0, 0, 64, 4}, vpSide = { 0, 0, viewPort.w, 1 };

	void Init() {
		glGenTextures( 1, &depthTexture );
		glBindTexture( GL_TEXTURE_2D, depthTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewPort.w, viewPort.h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
		glCheck();

		vpDefault.ReadCurrent();
		glGenFramebuffers( 1, &glHandle );
		glBindFramebuffer( GL_FRAMEBUFFER, glHandle );
		GLuint rbo[1];
		glCheck();
		glGenRenderbuffers( 1, rbo );
		glCheck();
		glBindRenderbuffer( GL_RENDERBUFFER, rbo[0] );
		glCheck();
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA, viewPort.w, viewPort.h );
		glCheck();
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo[0] );
		glCheck();
		glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0 );
		glCheck();
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			Beep( 99, 99 );
		Unbind();
		glCheck();
	}

	void Bind(int side) {
		Mat &proj = mapProjectionMatrix[side];
		Vec left = mapViewMatrix[side] * mapCorners[side], right = mapViewMatrix[side] * mapCorners[(side + 1) % 4];
		proj[0] = 2 * mapSideNear[side] / (right.x - left.x);
		proj[4] = -(right.x + left.x) / (right.x - left.x);
		proj[9] = 1;
		proj[6] = proj[7] = 1;
		proj[14] = -2 * mapSideNear[side];
		proj.Apply( GL_PROJECTION );
		mapViewMatrix[side].Apply( GL_MODELVIEW );

		glBindFramebuffer( GL_FRAMEBUFFER, glHandle );
		vpSide.y = side;
		vpSide.MakeCurrent();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glCheck();
	}

	void Unbind() {
		glBindFramebuffer( GL_FRAMEBUFFER, 0 ); 
		vpDefault.MakeCurrent();
		glCheck();
	}

	void BlitTo( ViewPort &dest ) {
		glCheck();
		vpDefault.MakeCurrent();
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glBlitFramebuffer( vpSide.x, vpSide.y, vpSide.x + vpSide.w, vpSide.y + vpSide.h,
			dest.x, dest.y, dest.x + dest.w, dest.y + dest.h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, glHandle );
		viewPort.MakeCurrent();
		glCheck();
	}
};

Framebuffer fboShadows;