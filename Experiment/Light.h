#pragma once

Vec playerPosition;

void calcLightMatrices() {
	float squeeze = log2( playerPosition.Length() / BASE_NEAR ) - 1;
	squeeze = std::max<float>( squeeze, 0 );
	std::cout << squeeze << '\n';
	if ( keyStates[GLFW_KEY_F] ) // anti-flicker #1
		squeeze = floor( squeeze );
	squeeze = (float) pow( 2, squeeze );
	mapSideNear[0] = BASE_NEAR * squeeze;
	mapSideNear[2] = BASE_NEAR / squeeze;

	float movingAngle = 0;
	if ( squeeze > 1 ) {
		float texelSize = 2 * BASE_NEAR / fboShadows.pageSize;
		float angleStep = atan2( texelSize, mapSideNear[0] );
		movingAngle = atan2( playerPosition.y, playerPosition.x ) + (float)M_PI / 2;
		if ( keyStates[GLFW_KEY_S] ) { // anti-flicker #2
			int steps = (int)(movingAngle / angleStep);
			movingAngle = steps * angleStep;
		}
	}

	for ( int side = 0; side < 4; side++ ) {
		mapViewMatrix[side].rotateToNorm( movingAngle + side * (float)M_PI / 2 );

		Mat viewInv;
		gluInvertMatrix( mapViewMatrix[side].elements, viewInv.elements );

		int leftSide = (side /* -1 */ + 3) % 4, rightSide = (side + 1) % 4;
		Vec leftCorner( -mapSideNear[leftSide], mapSideNear[side] ), rightCorner( mapSideNear[rightSide], mapSideNear[side] );
		mapCorners[side] = viewInv * leftCorner;
		mapCorners[rightSide] = viewInv * rightCorner;

		float zNear = mapSideNear[side];
		mapProjectionMatrix[side].projectionFor( leftCorner.x, rightCorner.x, zNear, -zNear, zNear );
	}
}

