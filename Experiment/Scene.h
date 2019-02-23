#pragma once

const int lineCount = 9;
Vec vertexData[lineCount * 2];

void Init() {
	random_engine.seed( 7 );
	for ( int i = 0; i < 2 * lineCount; i++ )
		vertexData[i].random();
	for ( int i = 0; i < lineCount; i++ ) {
		vertexData[i * 2] = vertexData[i * 2] * 1.8f;
		vertexData[i * 2] = vertexData[i * 2] + -0.9f;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] * 0.14f;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] + vertexData[i * 2];
	}

	glEnable( GL_SCISSOR_TEST );
	glEnable( GL_DEPTH_TEST );

	LoadShaders();
	fboShadows.Init();
}

void drawWorld() {
	glBegin( GL_LINES );
	for ( int i = 0; i < 2 * lineCount; ) {
		glVertex2fv( &vertexData[i++].x );
		glVertex2fv( &vertexData[i++].x );
	}
	glEnd();
}

void lightView() {
	mapShader.Use();
	vpDefault.ReadCurrent();
	mapProjectionMatrix.Apply();
	glClearColor( 0.4f, 0, 0, 1 );
	glColor3f( 1, 1, 0.5 );
	fboShadows.Bind();
	for ( int i = 0; i < 4; i++ ) {
		mapViewMatrix[i].Apply();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		drawWorld();
		const int magnify = 6;
		ViewPort vpVisual = { magnify, vpDefault.h - magnify * 3 - i * magnify * 2, fboShadows.viewPort.w*magnify, magnify };
		fboShadows.BlitTo( vpVisual );
	}
	fboShadows.Unbind();
	glClearColor( 0, 0, 0, 1 );
	glLoadIdentity();
}

void mainView() {
	mainShader.Use();
	vpDefault.ReadCurrent();
	mainProjectionMatrix.elements[0] = (float)vpDefault.h / vpDefault.w;
	mainProjectionMatrix.Apply();

	glBegin( GL_LINES );
	glColor3f( 0.7f, 0, 1 );
	glVertex2d( 1, 1 );
	glVertex2d( -1, -1 );
	glVertex2d( -1, 1 );
	glVertex2d( 1, -1 );
	glColor3f( 1, 1, 1 );
	glEnd();

	glColor3f( 1, 1, 1 );
	drawWorld();

	/*glBegin( GL_QUADS );	// center
	glVertex2f( .01f, .01f );
	glVertex2f( .01f, -.01f );
	glVertex2f( -.01f, -.01f );
	glVertex2f( -.01f, .01f );
	glEnd();*/
}