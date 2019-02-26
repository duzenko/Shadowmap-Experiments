#pragma once

struct Mat {
	float elements[16];
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
};

Mat identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
}, mainProjectionMatrix = identity, mapViewMatrix[4], mapProjectionMatrix[4];