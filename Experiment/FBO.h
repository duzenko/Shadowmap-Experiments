#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

float mapSideNear[4] = { .1f,.1f,.1f,.1f };
Vec mapCorners[4] = {
	{ -.1f, .1f },
	{ .1f, .1f },
	{ .1f, -.1f },
	{ -.1f, -.1f },
};

struct ViewPort {
	int x, y, w, h;

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
		static float movingAngle = 0;
		if ( keyStates[GLFW_KEY_RIGHT] )
			movingAngle += 1e-3f;
		if ( keyStates[GLFW_KEY_LEFT] )
			movingAngle -= 1e-3f;
		Mat &proj = mapProjectionMatrix[side], viewInv;
		mapViewMatrix[side].rotateToNorm( movingAngle + side * (float)M_PI / 2 );
		gluInvertMatrix( mapViewMatrix[side].elements, viewInv.elements );
		
		int leftSide = (side /* -1 */ + 3 ) % 4, rightSide = (side + 1) % 4;
		Vec leftCorner( -mapSideNear[leftSide], mapSideNear[side] ), rightCorner( mapSideNear[rightSide], mapSideNear[side] );
		mapCorners[side] = viewInv * leftCorner;
		mapCorners[rightSide] = viewInv * rightCorner;
		
		proj[0] = 2 * mapSideNear[side] / (rightCorner.x - leftCorner.x);
		proj[4] = -(rightCorner.x + leftCorner.x) / (rightCorner.x - leftCorner.x);
		proj[6] = proj[7] = 1;
		proj[9] = 1;
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