#include "ShaderProgram.h"

#include <GLES2/gl2.h>
#include <string>
#include <android/log.h>
#include <glm/gtc/type_ptr.hpp>

#define LOG_TAG "OpenGLES"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

ShaderProgram::ShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode)
{
	auto createShader = [](const char* code, GLenum type)
	{
		const unsigned int shaderId = glCreateShader(type);

		glShaderSource(shaderId, 1, &code, nullptr);
		glCompileShader(shaderId);

        GLint log_length;
        glGetProgramiv(shaderId, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length > 0)
        {
            char log[512];
            glGetProgramInfoLog(shaderId, 512, nullptr, log);
            LOGE("%s", log);
        }

		return shaderId;
	};

	const auto vertexShader = createShader(vertexShaderCode, GL_VERTEX_SHADER);
	const auto fragmentShader = createShader(fragmentShaderCode, GL_FRAGMENT_SHADER);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	int success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		char log[512];
		glGetProgramInfoLog(ID, 512, nullptr, log);
        LOGE("%s", log);
	}

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderProgram::use() const
{
	glUseProgram(ID);
}

void ShaderProgram::setMatrix(const char* name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setVec3(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void ShaderProgram::setVec3(const char* name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void ShaderProgram::setBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name), static_cast<int>(value));
}

void ShaderProgram::setInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void ShaderProgram::setFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(ID);
}
