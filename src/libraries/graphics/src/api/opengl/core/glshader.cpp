#include "rex_graphics_pch.h"
#include "api/opengl/core/glshader.h"

//-------------------------------------------------------------------------
rex::opengl::Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
	:m_shader_id(0)
{
	// Read our shaders into the appropriate buffers
	// Create an empty vertex shader handle
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Send the vertex shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	const char* source = vertexSource.c_str();
	glShaderSource(vertexShader, 1, &source, 0);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	int isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> infoLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(vertexShader);

		RX_ERROR("{0}", infoLog.data());
		RX_ASSERT_X(false, "Vertex shader compilation failure");

		return;
	}

	// Create an empty fragment shader handle
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Send the fragment shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	source = fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &source, 0);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> infoLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(fragmentShader);
		// Either of them. Don't leak shaders.
		glDeleteShader(vertexShader);

		RX_ERROR("{0}", infoLog.data());
		RX_ASSERT_X(false, "Fragment shader compilation failure");

		return;
	}

	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.
	m_shader_id = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(m_shader_id, vertexShader);
	glAttachShader(m_shader_id, fragmentShader);

	// Link our program
	glLinkProgram(m_shader_id);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	int isLinked = 0;
	glGetProgramiv(m_shader_id, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		int maxLength = 0;
		glGetProgramiv(m_shader_id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> infoLog(maxLength);
		glGetProgramInfoLog(m_shader_id, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(m_shader_id);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		RX_ERROR("{0}", infoLog.data());
		RX_ASSERT_X(false, "Shader link failure");

		return;
	}

	// Always detach shaders after a successful link.
	glDetachShader(m_shader_id, vertexShader);
	glDetachShader(m_shader_id, fragmentShader);
}
//-------------------------------------------------------------------------
rex::opengl::Shader::~Shader()
{
	glDeleteProgram(m_shader_id);
}

//-------------------------------------------------------------------------
void rex::opengl::Shader::bind() const
{
	glUseProgram(m_shader_id);
}

//-------------------------------------------------------------------------
void rex::opengl::Shader::unbind() const
{
	glUseProgram(0);
}
