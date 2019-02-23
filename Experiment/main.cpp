#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "misc.h"
#include "Matrices.h"
#include "Types.h"
#include "GLSL.h"
#include "FBO.h"
#include "Scene.h"

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ) {
	if ( action != GLFW_PRESS )
		return;
	if ( key >= MAX_KEY ) {
		Beep( 99, 99 );
		return;
	}
	keyStates[key] = !keyStates[key];
	if ( key == GLFW_KEY_ESCAPE )
		glfwSetWindowShouldClose( window, GLFW_TRUE );
	if ( key == GLFW_KEY_SPACE )
		memset( keyStates, 0, sizeof( keyStates ) );
}

int main() {
	GLFWwindow* window;

	if ( !glfwInit() )
		return -1;

	glfwWindowHint( GLFW_MAXIMIZED, 1 );
	window = glfwCreateWindow( 1280, 768, "Hello World", NULL, NULL );
	if ( !window ) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent( window );
	if ( !gladLoadGL() )
		return -2;

	glfwSetKeyCallback( window, key_callback );
	glfwSwapInterval( GLFW_TRUE );

	Init();

	if ( !passthroughShader.program || !worldShader.program )
		return 3;

	glfwSetWindowTitle(window, (char *)glGetString( GL_VERSION ));

	while ( !glfwWindowShouldClose( window ) ) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		lightView();
		mainView();
		glCheck();

		glfwSwapBuffers( window );

		glfwPollEvents();
	}

	glfwTerminate();
}