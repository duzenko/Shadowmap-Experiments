#pragma once

struct Mat {
	float elements[16];
	GLenum type;
	void Apply() {
		glMatrixMode( type );
		glLoadMatrixf( elements );
	}
};

Mat mapViewMatrix[4] = {
	{
		1,	0,	0,	0,
		0,	0,	-1,	0,
		0,	0,	0,	0,
		0,	0,	0,	1,
		GL_MODELVIEW
	},
	{
		0,	0,	-1,	0,
		-1,	0,	0,	0,
		0,	0,	0,	0,
		0,	0,	0,	1,
		GL_MODELVIEW
	},
	{
		-1,	0,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	0,
		0,	0,	0,	1,
		GL_MODELVIEW
	},
	{
		0,	0,	1,	0,
		1,	0,	0,	0,
		0,	0,	0,	0,
		0,	0,	0,	1,
		GL_MODELVIEW
	},
};

Mat mainProjectionMatrix = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
	GL_PROJECTION
};

Mat mapProjectionMatrix = {
	1, 0, 0,	0,
	0, 1, 0,	0,
	0, 0, -1,	-1,
	0, 0, -.2f,	0,
	GL_PROJECTION
};