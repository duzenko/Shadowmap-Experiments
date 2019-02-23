#pragma once

struct Shader {
	const char *vertexSource, *fragmentSource;
	GLuint program;
	Shader( const char *vertexSource, const char *fragmentSource ) : vertexSource( vertexSource ), fragmentSource( fragmentSource ) {}
	void Load() {
		program = LoadShader( vertexSource, fragmentSource );
		LocateUniforms();
	}
	static GLuint LoadShader( const char *vertexSource, const char *fragmentSource );
	virtual void LocateUniforms() {}
	void Use() {
		glUseProgram( program );
	}
};

struct MainShader : Shader {
	GLint start, end;
	using Shader::Shader;
	virtual void LocateUniforms() {
		Use();
		start = glGetUniformLocation( program, "start" );
		end = glGetUniformLocation( program, "end" );
	}
};

Shader mapShader( R"(
#version 110

void main() {
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
)", R"(
#version 110

void main() {
	gl_FragColor = gl_Color;	
}
)" );

MainShader mainShader( R"(
#version 110

void main() {
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
)", R"(
#version 110

uniform vec2 start, end;

void main() {
	gl_FragColor = gl_Color;	
})" );

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
		std::vector<char> VertexShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0] );
		fprintf( stdout, "%sn", &VertexShaderErrorMessage[0] );
		return 0;
	}

	glShaderSource( FragmentShaderID, 1, &fragmentSource, NULL );
	glCompileShader( FragmentShaderID );

	glGetShaderiv( FragmentShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::vector<char> FragmentShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0] );
		fprintf( stdout, "%s\n", &FragmentShaderErrorMessage[0] );
		return 0;
	}

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
		return 0;
	}

	glDeleteShader( VertexShaderID );
	glDeleteShader( FragmentShaderID );

	return ProgramID;
}

void LoadShaders() {
	mainShader.Load();
	mapShader.Load();
}