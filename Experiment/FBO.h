#pragma once

const float BASE_NEAR = 0.1f;
float mapSideNear[4] = {0, BASE_NEAR, 0, BASE_NEAR}; // front, right, back, left
Vec mapCorners[4];	// front left, front right, back right, back left

struct ViewPort {
	int x, y, w, h;

	ViewPort() {}
	ViewPort( int x, int y, int w, int h ) : x( x ), y( y ), w( w ), h( h ) {}
	ViewPort( float x, float y, float w, float h ) : x( (int)x ), y( (int)y ), w( (int)w ), h( (int)h ) {}

	void MakeCurrent() {
		glViewport( x, y, w, h );
		glScissor( x, y, w, h );
	}

	void ReadCurrent() {
		glGetIntegerv( GL_VIEWPORT, &x );
	}
}; 

ViewPort vpDefault;

struct Framebuffer {
	GLuint glHandle, depthTexture, rboColor;
	int pageSize = 64;
	ViewPort vpInternal = {0, 0, pageSize, pageSize*4 };

	void Init() {
		glGenTextures( 1, &depthTexture );
		glBindTexture( GL_TEXTURE_2D, depthTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, vpInternal.w, vpInternal.h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
		glCheck();

		vpDefault.ReadCurrent();
		glGenFramebuffers( 1, &glHandle );
		glBindFramebuffer( GL_FRAMEBUFFER, glHandle );
		glCheck();
		glGenRenderbuffers( 1, &rboColor );
		glCheck();
		glBindRenderbuffer( GL_RENDERBUFFER, rboColor );
		glCheck();
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA, vpInternal.w, vpInternal.h );
		glCheck();
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColor );
		glCheck();
		glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0 );
		glCheck();
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			Beep( 99, 99 );
		Unbind();
		glCheck();
	}

	void Bind(int side) {
		if ( vpInternal.w != pageSize ) {
			vpInternal.w = pageSize;
			vpInternal.h = pageSize*4;
			glBindTexture( GL_TEXTURE_2D, depthTexture );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, vpInternal.w, vpInternal.h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
			glBindRenderbuffer( GL_RENDERBUFFER, rboColor );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA, vpInternal.w, vpInternal.h );
		}

		glBindFramebuffer( GL_FRAMEBUFFER, glHandle );
		ViewPort vpPage = { 0, side*pageSize, pageSize, pageSize };
		vpPage.MakeCurrent();
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
		glBlitFramebuffer( 0, 0, vpInternal.w, vpInternal.h,
			dest.x, dest.y, dest.x + dest.w, dest.y + dest.h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, glHandle );
		//viewPort.MakeCurrent();
		glCheck();
	}
};

Framebuffer fboShadows;