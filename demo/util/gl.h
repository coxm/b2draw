#include <string>


namespace demo {


/** Get the info log of an OpenGL object. */
std::string
getLog(
	GLuint const handle,
	decltype(glGetShaderiv) writeLength,
	decltype(glGetShaderInfoLog) writeLog
);


/** Get the info log of a shader. */
inline std::string
getShaderLog(GLuint const handle)
{
	return getLog(handle, glGetShaderiv, glGetShaderInfoLog);
}


/** Get the info log of a program. */
inline std::string
getProgramLog(GLuint const handle)
{
	return getLog(handle, glGetProgramiv, glGetProgramInfoLog);
}


/**
 * Compile a shader.
 *
 * @returns the shader handle if successful, or 0 on failure.
 */
GLuint const compileShader(GLuint const type, GLchar const* pSource);


} // namespace demo
