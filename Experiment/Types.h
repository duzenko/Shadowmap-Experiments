#pragma once

#include <random>

static std::default_random_engine random_engine;
static std::uniform_real_distribution<float> real_distribution( 0, 1 );

struct Vec {
	float x, y, z, w;
	Vec operator + ( Vec &a ) {
		return { x + a.x, y + a.y };
	}
	Vec operator + ( float a ) {
		return { x + a, y + a };
	}
	Vec operator * ( float a ) {
		return { x * a, y * a };
	}
	void random() { // rage 0 - 1
		x = real_distribution( random_engine );
		y = real_distribution( random_engine );
	}
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