#include <iostream>

#include <GL/glew.h>

#include "./gl.h"


namespace demo {


std::string
getLog(
	GLuint const handle,
	decltype(glGetShaderiv) writeLength,
	decltype(glGetShaderInfoLog) writeLog
)
{
	GLint length{0};
	writeLength(handle, GL_INFO_LOG_LENGTH, &length);
	std::string log(length, '\0');
	if (length > 0)
	{
		writeLog(handle, length, nullptr, &log[0]);
	}
	return log;
}


GLuint const compileShader(GLuint const type, GLchar const* pSource)
{
	GLuint const shaderID{glCreateShader(type)};
	glShaderSource(shaderID, 1, &pSource, nullptr);
	glCompileShader(shaderID);

	GLint compiled{GL_FALSE};
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE)
	{
		std::cout << "Shader compilation failed:" << getShaderLog(shaderID)
			<< std::endl;
		throw std::runtime_error{"Shader compilation failed"};
	}

	return shaderID;
}


} // namespace demo
