#pragma once

const int lineCount = 31;
Vec vertexData[lineCount * 2];
Vec playerPosition;

void ShuffleLines() {
	for ( int i = 0; i < 2 * lineCount; i++ )
		vertexData[i].random();
	for ( int i = 0; i < lineCount; i++ ) {
		const float distrib = 1.3f;
		vertexData[i * 2] = vertexData[i * 2] * distrib * 2;
		vertexData[i * 2] = vertexData[i * 2] + -distrib;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] * 0.4f + -.2f;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] + vertexData[i * 2];
	}
}

void Init() {
	glEnable( GL_SCISSOR_TEST );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	LoadShaders();
	fboShadows.Init();

	ShuffleLines();
}

void drawWorld() {
	glBegin( GL_LINES );
	for ( int i = 0; i < 2 * lineCount; ) {
		glVertex2fv( &vertexData[i++].x );
		glVertex2fv( &vertexData[i++].x );
	}
	glEnd();
}

void lightViewSide( int side ) {
	float dist = playerPosition.Length2();
	float movingAngle = atan2( playerPosition.y, playerPosition.x ) + (float)M_PI / 2;
	if ( dist < .1f ) {		// anti-flicker								// magic const's
		//movingAngle *= dist / .1f;
	}
	mapViewMatrix[side].rotateToNorm( movingAngle + side * (float)M_PI / 2 );

	Mat viewInv;
	gluInvertMatrix( mapViewMatrix[side].elements, viewInv.elements );

	int leftSide = (side /* -1 */ + 3) % 4, rightSide = (side + 1) % 4;
	Vec leftCorner( -mapSideNear[leftSide], mapSideNear[side] ), rightCorner( mapSideNear[rightSide], mapSideNear[side] );
	mapCorners[side] = viewInv * leftCorner;
	mapCorners[rightSide] = viewInv * rightCorner;

	float zNear = mapSideNear[side];
	mapProjectionMatrix[side].projectionFor( leftCorner.x, rightCorner.x, zNear, -zNear, zNear );
	shadowShader.SetMatrices( mapViewMatrix[side], mapProjectionMatrix[side] );

	fboShadows.Bind( side );
	drawWorld();
}

void lightView() {
	float squeeze = pow( 1 + 3e1f * playerPosition.Length2(), 0.3f );	// magic const's
	mapSideNear[0] = BASE_NEAR * squeeze;
	mapSideNear[2] = BASE_NEAR / squeeze;

	vpDefault.ReadCurrent();
	glClearColor( 0.4f, 0, 0, 1 );
	glColor3f( 0.8f, 0.4f, 0.2f );
	shadowShader.Use();
	for ( int side = 0; side < 4; side++ ) {
		lightViewSide( side );
		const int magnify = 6;
		ViewPort vpVisual = { magnify, vpDefault.h - magnify * 3 - side * magnify * 2, fboShadows.viewPort.w*magnify, magnify };
		fboShadows.BlitTo( vpVisual );
	}
	fboShadows.Unbind();
	glClearColor( 0, 0, 0, 1 );
	glColor3f( 1, 1, 1 );
	glLoadIdentity();
}

void mainView() {
	vpDefault.ReadCurrent();
	mainProjectionMatrix.elements[0] = (float)vpDefault.h / vpDefault.w;

	passthroughShader.Use();
	passthroughShader.SetMatrices( identity, mainProjectionMatrix );

	glBegin( GL_LINE_LOOP );
	for ( int i = 0; i < 4; i++ )
		glVertex2fv( mapCorners[i] );
	glEnd();

	glBegin( GL_LINES );
	glColor3f( 0.7f, 0, 1 );
	for ( int i = 0; i < 4; i++ ) {
		glVertex2f( 0, 0 );
		glVertex4f( mapCorners[i].x, mapCorners[i].y, 0, 0 ); // infinity projection
	}
	glEnd();

	worldShader.Use();
	worldShader.SetMatrices( identity, mainProjectionMatrix );
	glBindTexture( GL_TEXTURE_2D, fboShadows.depthTexture );
	drawWorld();

	glColor4f( 1, 1, 1, .25f );
	drawCenterRect( 2 );
	glColor4f( 1, 1, 1, 1 );
}