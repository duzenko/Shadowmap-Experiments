#pragma once

const int lineCount = 31;
Vec vertexData[lineCount * 2];

void Init() {
	random_engine.seed( 7 );
	for ( int i = 0; i < 2 * lineCount; i++ )
		vertexData[i].random();
	for ( int i = 0; i < lineCount; i++ ) {
		vertexData[i * 2] = vertexData[i * 2] * 1.8f;
		vertexData[i * 2] = vertexData[i * 2] + -0.9f;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] * 0.4f + -.2f;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] + vertexData[i * 2];
	}

	glEnable( GL_SCISSOR_TEST );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );

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
	vpDefault.ReadCurrent();
	mapProjectionMatrix.Apply( GL_PROJECTION );
	glClearColor( 0.4f, 0, 0, 1 );
	glColor3f( 0.8f, 0.4f, 0.2f );
	for ( int i = 0; i < 4; i++ ) {
		fboShadows.Bind(i);
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
	vpDefault.ReadCurrent();
	mainProjectionMatrix.elements[0] = (float)vpDefault.h / vpDefault.w;
	mainProjectionMatrix.Apply( GL_PROJECTION );

	glBegin( GL_LINES );
	glColor3f( 0.7f, 0, 1 );
	glVertex2d( 1, 1 );
	glVertex2d( -1, -1 );
	glVertex2d( -1, 1 );
	glVertex2d( 1, -1 );
	glColor3f( 1, 1, 1 );
	glEnd();

	glColor3f( 1, 1, 1 );
	worldShader.Use();
	drawWorld();

	glBegin( GL_QUADS );	// center
	glColor4f( 1, 1, 1, .3f );
	const float r = 0.6f;
	glVertex2f( r, r );
	glVertex2f( r, -r );
	glVertex2f( -r, -r );
	glVertex2f( -r, r );
	glColor4f( 1, 1, 1, 1 );
	glEnd();

	passthroughShader.Use();
}