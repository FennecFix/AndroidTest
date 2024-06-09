#include "Cube.h"

Cube::Cube(GLuint shaderProgram)
{
    GLfloat cubeVertices[] =
    {
            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            // Задняя грань
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f
    };

    GLubyte cubeIndices[] =
    {
            0, 1, 2, 0, 2, 3, // Передняя грань
            1, 5, 6, 1, 6, 2, // Правая грань
            4, 7, 6, 4, 6, 5, // Задняя грань
            4, 0, 3, 4, 3, 7, // Левая грань
            3, 2, 6, 3, 6, 7, // Верхняя грань
            4, 5, 1, 4, 1, 0  // Нижняя грань
    };

    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &indexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    GLint positionAttribute = glGetAttribLocation(shaderProgram, "vPosition");
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void Cube::draw(GLuint shaderProgram) const
{
    GLuint positionAttribLocation = glGetAttribLocation(shaderProgram, "vPosition");
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glEnableVertexAttribArray(positionAttribLocation);
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
}
