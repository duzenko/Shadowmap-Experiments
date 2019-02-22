#pragma once

void glCheck() {
	auto error = glGetError();
	if ( error != GL_NO_ERROR )
		Beep( 99, 99 );
}