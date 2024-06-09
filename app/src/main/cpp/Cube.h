
#ifndef TESTAPPLICATION_CUBE_H
#define TESTAPPLICATION_CUBE_H

#include <GLES2/gl2.h>

class Cube
{
    GLuint vertexVBO = 0;
    GLuint indexVBO = 0;
public:
    Cube(GLuint shaderProgram);
    void draw(GLuint shaderProgram) const;
};


#endif //TESTAPPLICATION_CUBE_H
