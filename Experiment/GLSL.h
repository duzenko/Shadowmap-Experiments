#pragma once

struct Shader {
	const char *vertexSource, *fragmentSource;
	GLuint program;
	Shader( const char *vertexSource, const char *fragmentSource ) : vertexSource( vertexSource ), fragmentSource( fragmentSource ) {}
	void Load() {
		program = LoadShader( vertexSource, fragmentSource );
		LocateUniforms();
		glCheck();
	}
	static GLuint LoadShader( const char *vertexSource, const char *fragmentSource );
	virtual void LocateUniforms() {}
	virtual void Use() {
		glUseProgram( program );
	}
};

Shader passthroughShader( R"(
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

#define BindUniform(name) name = glGetUniformLocation( program, #name )
#define BindUniforms(...)		int i, _arr_[] = {__VA_ARGS__};						\
								for(i=0; i<sizeof(_arr_)/sizeof(int) ; i++)			\
								BindUniform(_arr_[i])
							

struct WorldShader : Shader {
	GLint mapView, mapProjection, f1;
	using Shader::Shader;
	virtual void LocateUniforms() {
		glUseProgram( program );
		//BindUniforms( mapView, mapProjection, f1 );
		BindUniform( mapView );
		BindUniform( mapProjection );
		BindUniform( f1 );
	}
	virtual void Use() {
		Shader::Use();
		glUniformMatrix4fv( mapView, 1, false, mapViewMatrix[0].elements );
		glUniformMatrix4fv( mapProjection, 1, false, mapProjectionMatrix.elements );
		glUniform1i( f1, keyStates[GLFW_KEY_F1] );
	}
};

WorldShader worldShader( R"(
#version 110

uniform mat4 mapView, mapProjection;

varying vec4 inMapSpace;

void main() {
	inMapSpace = mapView * mapProjection * gl_Vertex;
	inMapSpace = mapProjection * gl_Vertex;
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
)", R"(
#version 110

varying vec4 inMapSpace;

uniform bool f1;

void main() {
	gl_FragColor = gl_Color;	
	if (!f1) {
		gl_FragColor.rgb = vec3(0);	
		gl_FragColor.rg = vec2(inMapSpace.x) * vec2(1, -1);
	}
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
		std::string VertexShaderErrorMessage( InfoLogLength + 1, 0 );
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
	worldShader.Load();
	passthroughShader.Load();
}