#pragma once

ViewPort vpDefault;

struct Framebuffer {
	GLuint glHandle;
	ViewPort viewPort = {0, 0, 128, 1};

	void Init() {
		GLuint depth_tex;
		glGenTextures( 1, &depth_tex );
		glBindTexture( GL_TEXTURE_2D, depth_tex );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewPort.w, viewPort.h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
		glCheck();

		vpDefault.ReadCurrent();
		glGenFramebuffers( 1, &glHandle );
		Bind();
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
		glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0 );
		glCheck();
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