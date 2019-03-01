#pragma once

const int lineCount = 31;
Vec vertexData[lineCount * 2];

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
	glEnableVertexAttribArray( 0 );

	LoadShaders();
	fboShadows.Init();

	ShuffleLines();
}

void drawWorld() {
	glVertexAttribPointer( 0, 4, GL_FLOAT, false, 0, vertexData );
	glDrawArrays( GL_LINES, 0, lineCount * 2 );
}

void lightView() {
	calcLightMatrices();

	glClearColor( 0, 0, 0, 1 );
	glColor3f( 0.8f, 0.4f, 0.2f );
	glLineWidth( 2 );		// HW smoothing conflict with line width = 1
	glBlendFunc( GL_ONE, GL_ZERO ); // ROP overwrite
	shadowShader.Use();
	for ( int side = 0; side < 4; side++ ) {
		shadowShader.SetMatrices( mapViewMatrix[side], mapProjectionMatrix[side] );
		fboShadows.Bind( side );
		drawWorld();
	}
	fboShadows.Unbind();
	glBlendFunc( GL_SRC_ALPHA, GL_ONE ); // ROP blend
	glLineWidth( 1 );
	glClearColor( 0, 0, 0, 1 );
	glColor3f( 1, 1, 1 );
	glLoadIdentity();
}

void mainView() {
	passthroughShader.Use();
	passthroughShader.SetMatrices( Mat::identity, mainProjectionMatrix );

	// Z-near 
	glColor4f( 0, 1, 1, 1 );
	glVertexAttribPointer( 0, 4, GL_FLOAT, false, 0, mapCorners );
	glDrawArrays( GL_LINE_LOOP, 0, 4 );

	// page frustum boundaries
	glColor3f( 0.7f, 0, 1 );
	Vec glData[4 * 2];
	for ( int i = 0; i < 4; i++ ) 
		glData[i * 2 + 1] = { mapCorners[i].x, mapCorners[i].y, 0, 0 }; // infinity projection
	glVertexAttribPointer( 0, 4, GL_FLOAT, false, 0, glData );
	glDrawArrays( GL_LINES, 0, 4 * 2 );

	glColor4f( 1, 1, 1, 1 );
	worldShader.Use();
	worldShader.SetMatrices( Mat::identity, mainProjectionMatrix );
	drawWorld();

	if ( !keyStates[GLFW_KEY_F1] ) {									// space light up
		glColor4f( 1, 1, 1, .25f );
		drawCenterRect( 2 );
	}

	float magnify = (float)vpDefault.h / fboShadows.pageSize / 8;
	ViewPort vpVisual = { vpDefault.h / 99.f, vpDefault.h / 99.f, fboShadows.pageSize * magnify, fboShadows.pageSize * magnify * 4 };
	fboShadows.BlitTo( vpVisual );
}