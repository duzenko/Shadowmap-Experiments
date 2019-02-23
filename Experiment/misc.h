#pragma once

const int MAX_KEY = 512; // ?
int keyStates[MAX_KEY];

void glCheck() {
	auto error = glGetError();
	if ( error != GL_NO_ERROR )
		Beep( 99, 99 );
}

void drawCenterRect( float r ) {
	glBegin( GL_QUADS );	
	glVertex2f( r, r );
	glVertex2f( r, -r );
	glVertex2f( -r, -r );
	glVertex2f( -r, r );
	glColor4f( 1, 1, 1, 1 );
	glEnd();
}