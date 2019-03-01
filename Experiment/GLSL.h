#pragma once

#define BindUniform(name) name = glGetUniformLocation( program, #name )

struct Shader {
	const char *vertexSource, *fragmentSource;
	GLuint program, viewMatrix, projectionMatrix;
	void Load() {
		program = LoadShader( vertexSource, fragmentSource );
		LocateUniforms();
		glCheck();
	}
	static GLuint LoadShader( const char *vertexSource, const char *fragmentSource );
	virtual void LocateUniforms() {
		glUseProgram( program );
		BindUniform( viewMatrix );
		BindUniform( projectionMatrix );
	}
	virtual void Use() {
		glUseProgram( program );
	}
	void SetMatrices( Mat &view, Mat projection ) {
		glUniformMatrix4fv( viewMatrix, 1, false, view.elements );
		glUniformMatrix4fv( projectionMatrix, 1, false, projection.elements );
	}
	Shader( const char *vertexSource, const char *fragmentSource ) : vertexSource( vertexSource ), fragmentSource( fragmentSource ) {}
	Shader() : Shader( 
#include "shaders/generic.vs"
		,
#include "shaders/generic.fs"
	) {}
};

struct ShadowShader : Shader {
	GLint pageSize;
	ShadowShader(): Shader(
#include "shaders/shadow.vs"
		,
#include "shaders/shadow.fs"
	) {}
	virtual void LocateUniforms() {
		Shader::LocateUniforms();
		BindUniform( pageSize );
	}
	virtual void Use() {
		Shader::Use();
		glUniform1i( pageSize, fboShadows.pageSize );
	}
};

struct WorldShader : Shader {
	GLint mapView, mapProjection, f, colorComp, pageSize, depthTexture, depthSubTexture;
	using Shader::Shader;
	virtual void LocateUniforms() {
		Shader::LocateUniforms();
		BindUniform( mapView );
		BindUniform( mapProjection );
		BindUniform( f );
		BindUniform( colorComp );
		BindUniform( pageSize );
		BindUniform( depthTexture );
		BindUniform( depthSubTexture );
	}
	virtual void Use() {
		Shader::Use();
		glUniformMatrix4fv( mapView, 4, false, mapViewMatrix[0].elements );
		glUniformMatrix4fv( mapProjection, 4, false, mapProjectionMatrix[0].elements );
		glUniform1i( f, keyStates[GLFW_KEY_F1] );
		glUniform1i( pageSize, fboShadows.pageSize );
		static int x = 0;
		for ( int i = GLFW_KEY_F2; i <= GLFW_KEY_F5; i++ ) // F2-F5 toggle color display for x,y,z,w
			if ( keyStates[i] ) {
				x = i - GLFW_KEY_F2;
				keyStates[i] = 0;
			}
		glUniform1i( colorComp, x );
		glUniform1i( f, keyStates[GLFW_KEY_F1] );
		glUniform1i( depthTexture, 0 );
		glUniform1i( depthSubTexture, 1 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, fboShadows.textures[0] );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, fboShadows.textures[1] );
		glCheck();
	}
	WorldShader() : Shader(
#include "shaders/world.vs"
		,
#include "shaders/world.fs"
	) {}
};

Shader passthroughShader;
ShadowShader shadowShader;
WorldShader worldShader;

GLuint Shader::LoadShader( const char *vertexSource, const char *fragmentSource ) {
	GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
	GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );

	GLint Result = GL_FALSE;
	int InfoLogLength;

	glShaderSource( VertexShaderID, 1, &vertexSource, NULL );
	glCompileShader( VertexShaderID );

	glGetShaderiv( VertexShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::string VertexShaderErrorMessage( InfoLogLength + 1, 0 );
		glGetShaderInfoLog( VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0] );
		fprintf( stdout, "%sn", &VertexShaderErrorMessage[0] );
	}
	if( Result != GL_TRUE )
		return 0;

	glShaderSource( FragmentShaderID, 1, &fragmentSource, NULL );
	glCompileShader( FragmentShaderID );

	glGetShaderiv( FragmentShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::string FragmentShaderErrorMessage( InfoLogLength + 1, 0 );
		glGetShaderInfoLog( FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0] );
		fprintf( stdout, "%s\n", &FragmentShaderErrorMessage[0] );
	}
	if ( Result != GL_TRUE )
		return 0;

	GLuint ProgramID = glCreateProgram();
	glAttachShader( ProgramID, VertexShaderID );
	glAttachShader( ProgramID, FragmentShaderID );
	glLinkProgram( ProgramID );

	glGetProgramiv( ProgramID, GL_LINK_STATUS, &Result );
	glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::vector<char> ProgramErrorMessage( InfoLogLength + 1 );
		glGetProgramInfoLog( ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0] );
		fprintf( stdout, "%s\n", &ProgramErrorMessage[0] );
	}
	if ( Result != GL_TRUE )
		return 0;

	glDeleteShader( VertexShaderID );
	glDeleteShader( FragmentShaderID );

	return ProgramID;
}

void LoadShaders() {
	worldShader.Load();
	shadowShader.Load();
	passthroughShader.Load();
}