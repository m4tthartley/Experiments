/*
	Opengl stuff
*/

//#define GL_GLEXT_PROTOTYPES
#include "w:/lib/glext.h"

//typedef unsigned int GLuint;
//typedef int GLint;
//typedef unsigned int GLenum;
//typedef int GLsizei;
//typedef char GLchar;

#define GL_EXTENSIONS\
	GLE(GLuint, CreateShader, GLenum type)\
	GLE(void, ShaderSource, GLuint shader, GLsizei count, const GLchar * const *string, const GLint *length)\
	GLE(void, CompileShader, GLuint shader)\
	GLE(GLuint, CreateProgram, void)\
	GLE(void, AttachShader, GLuint program, GLuint shader)\
	GLE(void, LinkProgram, GLuint program)\
	GLE(void, GetShaderInfoLog, GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog)\
	GLE(void, UseProgram, GLuint program)\
	\
	GLE(void, EnableVertexAttribArray, GLuint index)\
	GLE(void, VertexAttribPointer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)\
	/*GLE(void, DrawElements, GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)*/\
	\
	GLE(GLint, GetUniformLocation, GLuint program, const GLchar *name)\
	GLE(void, UniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)\
	GLE(void, Uniform4f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)\


#define GLE(ret, name, ...) typedef ret name##_proc(__VA_ARGS__); name##_proc *gl##name;
GL_EXTENSIONS
#undef GLE

void load_opengl_extensions() {
	HMODULE opengl_lib = LoadLibraryA("opengl32.dll");

#define GLE(ret, name, ...) gl##name = (name##_proc*)wglGetProcAddress("gl" #name);
	GL_EXTENSIONS
#undef GLE

	//glCreateShader = (CreateShader_proc*)wglGetProcAddress("glCreateShader ");
}

GLuint shader_from_string(char *vs, char *fs) {
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	int vlen = strlen(vs);
	int flen = strlen(fs);
	glShaderSource(vshader, 1, &vs, &vlen);
	glShaderSource(fshader, 1, &fs, &flen);
	glCompileShader(vshader);
	glCompileShader(fshader);
	char buffer[256];
	glGetShaderInfoLog(vshader, 256, NULL, buffer);
	OutputDebugString(buffer);
	glGetShaderInfoLog(fshader, 256, NULL, buffer);
	OutputDebugString(buffer);

	GLuint program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

	return program;
}

void use_shader(GLuint shader) {
	glUseProgram(shader);
}