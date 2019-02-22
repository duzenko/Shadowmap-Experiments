#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>

void get_random2( float *xy ) {

}

struct Vec {
	float x, y, z, w;
	Vec operator + ( Vec &a ) {
		return { x + a.x, y + a.y };
	}
	Vec operator + ( float a ) {
		return { x + a, y + a };
	}
	Vec operator * ( float a ) {
		return { x * a, y * a };
	}
	void random() { // rage 0 - 1
		static std::default_random_engine e;
		static std::uniform_real_distribution<float> dis( 0, 1 ); 
		x = dis( e );
		y = dis( e );
	}
};

void randomLine() {
}

Vec vertexData[20];

void drawWorld() {
	glBegin( GL_LINES );
	for ( int i = 0; i < 20; ) {
		glVertex2fv( &vertexData[i++].x );
		glVertex2fv( &vertexData[i++].x );
	}
	glEnd();
}

struct ViewPort { int x, y, w, h; };

void setViewPort( ViewPort &vp ) {
	glViewport( vp.x, vp.y, vp.w, vp.h );
	glScissor( vp.x, vp.y, vp.w, vp.h );
}

void Blit( ViewPort &src, ViewPort &dest) {
	glBlitFramebuffer( src.x, src.y, src.x + src.w, src.y + src.h, dest.x, dest.y, dest.x + dest.w, dest.y + dest.h, GL_COLOR_BUFFER_BIT, GL_NEAREST );
}

void lightView() {
	ViewPort vpMain, vpLight = {0, 0, 128, 1};
	glGetIntegerv( GL_VIEWPORT, &vpMain.x );
	for ( int i = 0; i < 4; i++ ) {
		setViewPort( vpLight );
		glClear( GL_COLOR_BUFFER_BIT );
		drawWorld();
		setViewPort( vpMain );
		ViewPort vpVisual = { 0, vpMain.h - 8 - i * 8, vpMain.w, 4 };
		Blit( vpLight, vpVisual );
	}
}

void mainView() {
	drawWorld();
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

	for ( int i = 0; i < 20; i++ )
		vertexData[i].random();
	for ( int i = 0; i < 10; i++ ) {
		vertexData[i * 2] = vertexData[i * 2] * 1.6f;
		vertexData[i * 2] = vertexData[i * 2] + -1;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] * 0.4f;
		vertexData[i * 2 + 1] = vertexData[i * 2 + 1] + vertexData[i * 2];
	}

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window ) ) {
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT );
		glEnable( GL_SCISSOR_TEST );

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