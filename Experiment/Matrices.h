#pragma once

struct Mat {
	float elements[16];
	void Apply( GLenum type ) {
		glMatrixMode( type );
		glLoadMatrixf( elements );
	}
	Vec operator *( Vec &a ) {
		Vec r;
		r.x = a.x * elements[0] + a.y * elements[4] + a.z * elements[8] + a.w * elements[12];
		r.y = a.x * elements[1] + a.y * elements[5] + a.z * elements[9] + a.w * elements[13];
		r.z = a.x * elements[2] + a.y * elements[6] + a.z * elements[10] + a.w * elements[14];
		r.w = a.x * elements[3] + a.y * elements[7] + a.z * elements[11] + a.w * elements[15];
		return r;
	}
	float & operator [] (int i) {
		return elements[i];
	}
};

Mat mapViewMatrix[4] = {
	{
		1,	0,	0,	0,
		0,	1,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1,
	},
	{
		0,	1,	0,	0,
		-1,	0,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1,
	},
	{
		-1,	0,	0,	0,
		0,	-1,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1,
	},
	{
		0,	-1,	0,	0,
		1,	0,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1,
	},
};

Mat mapViewInvMatrix[4] = {
	{
		1,	0,	0,	0,
		0,	1,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1,
	},
	{
		0,	-1,	0,	0,
		1,	0,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1,
	},
	{
		-1,	0,	0,	0,
		0,	-1,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1,
	},
	{
		0,	1,	0,	0,
		-1,	0,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1,
	},
};

Mat mainProjectionMatrix = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
};

Mat mapProjectionMatrix[4];/* = {
	1, 0, 0,	0,
	0, 1, 0,	0,
	0, 0, -1,	-1,
	0, 0, -.2f,	0,
};*/