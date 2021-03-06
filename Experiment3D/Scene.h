#pragma once

const int cubeCount = 31;
Vec cubes[cubeCount];

void ShuffleLines() {
	for ( int i = 0; i < cubeCount; i++ ) {
		cubes[i].random();
		cubes[i] = (cubes[i] + -.5) * 29;
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
	glCheck();
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, 0, cube_vertices );
	glCheck();
	for ( int i = 0; i < cubeCount; i++ ) {
		Mat model;
		model.translationTo( cubes[i] );
		glCheck();
		Shader::current->SetModelMatrix( model );
		glCheck();
		//glDrawArrays( GL_TRIANGLES, 0, 36 );
		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, cube_elements );
		glCheck();
	}
	glCheck();
	Shader::current->SetModelMatrix( Mat::identity );
	glCheck();
}

void lightView() {
	calcLightMatrices();

	glClearColor( 0, 0, 1, 1 );
	glColor3f( 0.8f, 0.4f, 0.2f );
	glBlendFunc( GL_ONE, GL_ZERO ); // ROP overwrite
	shadowShader.Use();
	for ( int side = 0; side < 4; side++ ) {
		shadowShader.SetViewProjectionMatrices( mapViewMatrix[side], mapProjectionMatrix[side] );
		fboShadows.Bind( side );
		drawWorld();
	}
	fboShadows.Unbind();
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); // ROP blend
	glLineWidth( 1 );
	glClearColor( 0, 0, 0, 1 );
	glColor3f( 1, 1, 1 );
	glLoadIdentity();
}

void mainView() {
	Mat view;
	view.viewFrom( Vec( 0, 0, 11, 1 ) );
	passthroughShader.Use();
	passthroughShader.SetViewProjectionMatrices( view, mainProjectionMatrix );

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

	glColor4f( 1, 1, 0, 0.1f );
	worldShader.Use();
	worldShader.SetViewProjectionMatrices( view, mainProjectionMatrix );
	drawWorld();

	if ( !keyStates[GLFW_KEY_F1] ) {									// space light up
		Mat model;
		model.translationTo( Vec(0, 0, -1, 1 ) );
		glCheck();
		Shader::current->SetModelMatrix( model );
		glColor4f( 1, 1, 1, .25f );
		drawCenterRect( 12 );
	}

	float magnify = (float)vpDefault.h / fboShadows.pageSize / 8;
	ViewPort vpVisual = { vpDefault.h / 99.f, vpDefault.h / 99.f, fboShadows.pageSize * magnify, fboShadows.pageSize * magnify * 4 };
	fboShadows.BlitTo( vpVisual );
}