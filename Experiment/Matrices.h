#pragma once

struct Mat {
	float elements[16];
	void Apply( GLenum type ) {
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
	},
	{
		0,	0,	-1,	0,
		-1,	0,	0,	0,
		0,	0,	0,	0,
		0,	0,	0,	1,
	},
	{
		-1,	0,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	0,
		0,	0,	0,	1,
	},
	{
		0,	0,	1,	0,
		1,	0,	0,	0,
		0,	0,	0,	0,
		0,	0,	0,	1,
	},
};

Mat mainProjectionMatrix = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
};

Mat mapProjectionMatrix = {
	1, 0, 0,	0,
	0, 1, 0,	0,
	0, 0, -1,	-1,
	0, 0, -.2f,	0,
};