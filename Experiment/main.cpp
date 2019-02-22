#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Matrices.h"
#include "Types.h"

const int lineCount = 9;
Vec vertexData[lineCount * 2];

void drawWorld() {
	glBegin( GL_LINES );
	for ( int i = 0; i < 2 * lineCount; ) {
		glVertex2fv( &vertexData[i++].x );
		glVertex2fv( &vertexData[i++].x );
	}
	glEnd();
}

void lightView() {
	mapProjectionMatrix.Apply();
	ViewPort vpMain, vpLight = { 0, 0, 256, 1 };
	glGetIntegerv( GL_VIEWPORT, &vpMain.x );
	glClearColor( 0, .5, 0, 1 );
	for ( int i = 0; i < 4; i++ ) {
		mapViewMatrix[i].Apply();
		vpLight.MakeCurrent();
		glClear( GL_COLOR_BUFFER_BIT );
		drawWorld();
		vpMain.MakeCurrent();
		const int magnify = 6;
		ViewPort vpVisual = { 0, vpMain.h - magnify*3 - i * magnify*2, vpLight.w*magnify, magnify };
		vpLight.BlitTo( vpVisual );
	}
	glClearColor( 0, 0, 0, 1 );
	glLoadIdentity();
}

void mainView() {
	ViewPort vpMain;
	glGetIntegerv( GL_VIEWPORT, &vpMain.x );
	
	mainProjectionMatrix.elements[0] = (float)vpMain.h / vpMain.w;
	mainProjectionMatrix.Apply();

	glBegin( GL_LINES );
	glColor3f( 0.7f, 0, 0 );
	glVertex2d( 1, 1 );
	glVertex2d( -1, -1 );
	glVertex2d( -1, 1 );
	glVertex2d( 1, -1 );
	glColor3f( 1, 1, 1 );
	glEnd();

	drawWorld();

	/*glBegin( GL_QUADS );	// center
	glVertex2f( .01f, .01f );
	glVertex2f( .01f, -.01f );
	glVertex2f( -.01f, -.01f );
	glVertex2f( -.01f, .01f );
	glEnd();*/
}

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ) {
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GLFW_TRUE );
}

int main() {
	GLFWwindow* window;

	/* Initialize the library */
	if ( !glfwInit() )
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint( GLFW_MAXIMIZED, 1 );
	window = glfwCreateWindow( 1280, 768, "Hello World", NULL, NULL );
	if ( !window ) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent( window );
	if ( !gladLoadGL() )
		return -2;
	glfwSetKeyCallback( window, key_callback );
	glfwSwapInterval( 1 );

	random_engine.seed( 7 );
	for ( int i = 0; i < 2 * lineCount; i++ )
		vertexData[i].random();
	for ( int i = 0; i < 2 * lineCount; i++ ) {
		vertexData[i * 2] = vertexData[i * 2] * 1.8f;
		vertexData[i * 2] = vertexData[i * 2] + -0.9f;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] * 0.14f;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] + vertexData[i * 2];
	}

	glEnable( GL_SCISSOR_TEST );

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window ) ) {
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT );

		lightView();
		mainView();
		auto error = glGetError();
		if ( error != GL_NO_ERROR )
			Beep( 99, 99 );

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
}