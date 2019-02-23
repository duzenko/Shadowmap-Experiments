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

struct WorldShader : Shader {
	GLint mapView, mapProjection, f, colorComp;
	using Shader::Shader;
	virtual void LocateUniforms() {
		glUseProgram( program );
		BindUniform( mapView );
		BindUniform( mapProjection );
		BindUniform( f );
		BindUniform( colorComp );
	}
	virtual void Use() {
		Shader::Use();
		glUniformMatrix4fv( mapView, 1, false, mapViewMatrix[0].elements );
		glUniformMatrix4fv( mapProjection, 1, false, mapProjectionMatrix.elements );
		glUniform1i( f, keyStates[GLFW_KEY_F1] );
		static int x = 0;
		for ( int i = GLFW_KEY_F2; i <= GLFW_KEY_F5; i++ ) // F2-F5 toggle color display for x,y,z,w
			if ( keyStates[i] ) {
				x = i - GLFW_KEY_F2;
				keyStates[i] = 0;
			}
		glUniform1i( colorComp, x );
	}
};

WorldShader worldShader( R"(
#version 110

uniform mat4 mapView, mapProjection;

varying vec4 inMapSpace;

void main() {
	inMapSpace = mapProjection * mapView * gl_Vertex;
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
)", R"(
#version 110

varying vec4 inMapSpace;

uniform bool f;
uniform int colorComp;

void main() {
	gl_FragColor = gl_Color;	
	if (!f) {
		gl_FragColor.rgb = vec3(0);	
		float p = inMapSpace.x/inMapSpace.w;
		if(p < -1.)
			gl_FragColor.rgb = vec3(1, 1, 0);	
		else if(p > 1.)
			gl_FragColor.rgb = vec3(1, 0, 1);	
		else if(inMapSpace.w < 0.)
			gl_FragColor.rgb = vec3(0, 1, 1);	
		else
			gl_FragColor.rg = vec2(p) * vec2(-1, 1);
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
		std::string FragmentShaderErrorMessage( InfoLogLength + 1, 0 );
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