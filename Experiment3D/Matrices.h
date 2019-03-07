#pragma once

struct Mat {
	float elements[16];
	static const Mat identity;
	Vec & operator [] ( int i ) {
		return ((Vec *)elements)[i];
	}
	Vec operator *( Vec &a ) {
		Vec r;
		for(int i=0; i<4; i++ )
			r[i] = (*this)[i] * a;
		return r;
	}
	void rotateToNorm( float fromAngle ) {
		memset( elements, 0, sizeof( elements ) );
		elements[0] = elements[5] = cos( fromAngle );
		elements[1] = -sin( fromAngle );
		elements[4] = -elements[1];
		elements[10] = elements[15] = 1;
	}
	void projectionFor( float l, float r, float t, float b, float n ) {
		elements[0] = 2 * n / (r - l);
		elements[1] = -(r + l) / (r - l);
		elements[6] = 1;
		elements[9] = 1;
		elements[11] = -2 * n;
		elements[13] = 1;
	}
	void projectionFor( float n, float a ) {
		memset( elements, 0, sizeof( elements ) );
		elements[0] = 1/a;
		elements[5] = 1;
		elements[10] = -1;
		elements[11] = -2 * n;
		elements[14] = -1;
	}
	void viewFrom( Vec v ) {
		*this = identity;
		elements[3] = -v.x;
		elements[7] = -v.y;
		elements[11] = -v.z;
	}	
	void translationTo( Vec v ) {
		*this = identity;
		elements[3] = v.x;
		elements[7] = v.y;
		elements[11] = v.z;
	}
};

const Mat Mat::identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
}; 

Mat mainProjectionMatrix = Mat::identity, mapViewMatrix[4], mapProjectionMatrix[4];