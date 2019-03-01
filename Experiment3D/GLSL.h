#pragma once

#define BindUniform(name) name = glGetUniformLocation( program, #name )

struct Shader {
	const char *vertexSource, *geometrySource, *fragmentSource;
	GLuint program, modelMatrix, viewMatrix, projectionMatrix;
	static Shader* current;
	void Load() {
		program = LoadShader( vertexSource, geometrySource, fragmentSource );
		LocateUniforms();
		glCheck();
	}
	static GLuint LoadShader( const char *vertexSource, const char *geometrySource, const char *fragmentSource );
	virtual void LocateUniforms() {
		glUseProgram( program );
		BindUniform( modelMatrix );
		BindUniform( viewMatrix );
		BindUniform( projectionMatrix );
		SetModelMatrix( Mat::identity );
		SetViewProjectionMatrices( Mat::identity, Mat::identity );
	}
	virtual void Use() {
		glUseProgram( program );
		current = this;
	}
	void SetViewProjectionMatrices( const Mat &view, const Mat &projection ) {
		glUniformMatrix4fv( viewMatrix, 1, false, view.elements );
		glUniformMatrix4fv( projectionMatrix, 1, false, projection.elements );
	}	
	void SetModelMatrix( const Mat &model ) {
		glUniformMatrix4fv( modelMatrix, 1, false, model.elements );
	}
	Shader( const char *vertexSource, const char *fragmentSource ) : vertexSource( vertexSource ), fragmentSource( fragmentSource ) {}
	Shader( const char *vertexSource, const char *geometrySource, const char *fragmentSource ) : vertexSource( vertexSource ), geometrySource( geometrySource ), fragmentSource( fragmentSource ) {}
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
#include "shaders/shadow.gs"
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
	GLint mapViews, mapProjections, f, colorComp, pageSize, depthTexture, depthSubTexture;
	using Shader::Shader;
	virtual void LocateUniforms() {
		glCheck();
		Shader::LocateUniforms();
		BindUniform( mapViews );
		BindUniform( mapProjections );
		BindUniform( f );
		BindUniform( colorComp );
		BindUniform( pageSize );
		BindUniform( depthTexture );
		BindUniform( depthSubTexture );
		glCheck();
	}
	virtual void Use() {
		Shader::Use();
		glUniformMatrix4fv( mapViews, 4, false, mapViewMatrix[0].elements );
		glUniformMatrix4fv( mapProjections, 4, false, mapProjectionMatrix[0].elements );
		glUniform1i( pageSize, fboShadows.pageSize );
		glUniform1iv( f, 13, &keyStates[GLFW_KEY_F1-1] );
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

GLuint Shader::LoadShader( const char *vertexSource, const char *geometrySource, const char *fragmentSource ) {
	GLint Result = GL_FALSE;
	int InfoLogLength;

	GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( VertexShaderID, 1, &vertexSource, NULL );
	glCompileShader( VertexShaderID );
	glGetShaderiv( VertexShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::string ErrorMessage( InfoLogLength + 1, 0 );
		glGetShaderInfoLog( VertexShaderID, InfoLogLength, NULL, &ErrorMessage[0] );
		fprintf( stdout, "%sn", &ErrorMessage[0] );
	}
	if ( Result != GL_TRUE )
		return 0;

	GLuint GeometryShaderID = glCreateShader( GL_GEOMETRY_SHADER );
	if ( geometrySource ) {
		glShaderSource( GeometryShaderID, 1, &geometrySource, NULL );
		glCompileShader( GeometryShaderID );
		glGetShaderiv( GeometryShaderID, GL_COMPILE_STATUS, &Result );
		glGetShaderiv( GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
		if ( InfoLogLength > 0 ) {
			std::string ErrorMessage( InfoLogLength + 1, 0 );
			glGetShaderInfoLog( GeometryShaderID, InfoLogLength, NULL, &ErrorMessage[0] );
			fprintf( stdout, "%s\n", &ErrorMessage[0] );
		}
		if ( Result != GL_TRUE )
			return 0;
	}

	GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( FragmentShaderID, 1, &fragmentSource, NULL );
	glCompileShader( FragmentShaderID );
	glGetShaderiv( FragmentShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::string ErrorMessage( InfoLogLength + 1, 0 );
		glGetShaderInfoLog( FragmentShaderID, InfoLogLength, NULL, &ErrorMessage[0] );
		fprintf( stdout, "%s\n", &ErrorMessage[0] );
	}
	if ( Result != GL_TRUE )
		return 0;

	GLuint ProgramID = glCreateProgram();
	glAttachShader( ProgramID, VertexShaderID );
	if ( geometrySource )
		glAttachShader( ProgramID, GeometryShaderID );
	glAttachShader( ProgramID, FragmentShaderID );
	glLinkProgram( ProgramID );

	glGetProgramiv( ProgramID, GL_LINK_STATUS, &Result );
	glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::string ErrorMessage( InfoLogLength + 1, 0 );
		glGetProgramInfoLog( ProgramID, InfoLogLength, NULL, &ErrorMessage[0] );
		fprintf( stdout, "%s\n", &ErrorMessage[0] );
	}
	if ( Result != GL_TRUE )
		return 0;

	glDeleteShader( VertexShaderID );
	glDeleteShader( GeometryShaderID );
	glDeleteShader( FragmentShaderID );

	glCheck();
	return ProgramID;
}

void LoadShaders() {
	worldShader.Load();
	shadowShader.Load();
	passthroughShader.Load();
}

Shader* Shader::current = NULL;