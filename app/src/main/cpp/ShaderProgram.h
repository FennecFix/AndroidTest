#ifndef TESTAPPLICATION_SHADERPROGRAM_H
#define TESTAPPLICATION_SHADERPROGRAM_H

#include <glm/fwd.hpp>

class ShaderProgram
{
public:
	unsigned int ID;

	ShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode);
    ~ShaderProgram();

	void use() const;

	void setMatrix(const char* name, const glm::mat4& value) const;

	void setVec3(const char* name, float x, float y, float z) const;
	void setVec3(const char* name, const glm::vec3& value) const;

	void setBool(const char* name, bool value) const;
	void setInt(const char* name, int value) const;
	void setFloat(const char* name, float value) const;
};

#endif //TESTAPPLICATION_SHADERPROGRAM_H

