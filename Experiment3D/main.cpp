#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Vectors.h"
#include "misc.h"
#include "Matrices.h"
#include "FBO.h"
#include "GLSL.h"
#include "Light.h"
#include "Scene.h"

int seed = 0;

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ) {
	if ( action != GLFW_PRESS )
		return;
	if ( key >= MAX_KEY ) {
		Beep( 99, 99 );
		return;
	}
	keyStates[key] = !keyStates[key];
	switch ( key ) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose( window, GLFW_TRUE );
		break;
	case GLFW_KEY_SPACE:
		memset( keyStates, 0, sizeof( keyStates ) );
		break;
	case GLFW_KEY_TAB:
		random_engine.seed( seed++ );
		ShuffleLines();
		break;
	case GLFW_KEY_KP_ADD:
		fboShadows.pageSize *= 2;
		break;
	case GLFW_KEY_KP_SUBTRACT:
		fboShadows.pageSize /= 2;
		break;
	default:
		break;
	}
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

	random_engine.seed( 20 );
	Init();

	if ( !passthroughShader.program || !worldShader.program )
		return 3;

	glfwSetWindowTitle(window, (char *)glGetString( GL_VERSION ));
	int width, height;
	glfwGetWindowSize( window, &width, &height );
	glfwSetCursorPos( window, width/2, height/2 );

	while ( !glfwWindowShouldClose( window ) ) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		lightView();
		mainView();
		glCheck();

		glfwSwapBuffers( window );

		glfwPollEvents();
		glfwGetWindowSize( window, &width, &height );
		glViewport( 0, 0, width, height );
		glScissor( 0, 0, width, height );
		double xpos, ypos;
		glfwGetCursorPos( window, &xpos, &ypos );
		int focused = glfwGetWindowAttrib( window, GLFW_FOCUSED ); 
		if ( focused ) {
			playerPosition.x = (float)(xpos - vpDefault.w / 2) / vpDefault.h * 2;
			playerPosition.y = (float)(ypos - vpDefault.h / 2) / vpDefault.h * 2;
		}
		vpDefault.ReadCurrent();
		mainProjectionMatrix.projectionFor( 0.1f, (float)width/height );
	}

	glfwTerminate();
}