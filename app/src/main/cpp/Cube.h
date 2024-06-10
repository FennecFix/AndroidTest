
#ifndef TESTAPPLICATION_CUBE_H
#define TESTAPPLICATION_CUBE_H

#include <GLES2/gl2.h>

class Cube
{
    GLuint VBO;
public:
    Cube(GLuint shaderProgram);
    ~Cube();
    void draw(GLuint shaderProgram) const;
};


#endif //TESTAPPLICATION_CUBE_H
