#pragma once

struct Mat {
	float elements[16];
	static const Mat identity;
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
	void rotateToNorm( float fromAngle ) {
		memset( elements, 0, sizeof( elements ) );
		elements[0] = elements[5] = cos( fromAngle );
		elements[1] = sin( fromAngle );
		elements[4] = -elements[1];
		elements[10] = elements[15] = 1;
	}
	void projectionFor( float l, float r, float t, float b, float n ) {
		elements[0] = 2 * n / (r - l);
		elements[4] = -(r + l) / (r - l);
		elements[6] = elements[7] = 1;
		elements[9] = 1;
		elements[14] = -2 * n;
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