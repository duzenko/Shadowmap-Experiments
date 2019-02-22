#pragma once

ViewPort vpDefault;

struct Framebuffer {
	GLuint glHandle;
	ViewPort viewPort = {0, 0, 128, 1};

	void Init() {
		vpDefault.ReadCurrent();
		glGenFramebuffers( 1, &glHandle );
		Bind();
		GLuint rbo[2];
		glGenRenderbuffers( 2, rbo );
		glBindRenderbuffer( GL_RENDERBUFFER, rbo[0] );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewPort.w, viewPort.h );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo[0] );
		glBindRenderbuffer( GL_RENDERBUFFER, rbo[1] );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA, viewPort.w, viewPort.h );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo[1] );
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			Beep( 99, 99 );
		Unbind();
		glCheck();
	}

	void Bind() {
		glBindFramebuffer( GL_FRAMEBUFFER, glHandle );
		viewPort.MakeCurrent();
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
		glBlitFramebuffer( viewPort.x, viewPort.y, viewPort.x + viewPort.w, viewPort.y + viewPort.h,
			dest.x, dest.y, dest.x + dest.w, dest.y + dest.h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, glHandle );
		viewPort.MakeCurrent();
		glCheck();
	}
};

Framebuffer fboShadows;