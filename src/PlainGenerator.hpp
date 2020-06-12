/* srand example */
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <math.h>
#include <iostream>
#include <GL/glew.h>

#include "Noisegenerator2d.hpp"
#include "glm/vec4.hpp"
#include "vsShaderLib.h"

using namespace glm;

vec4 *generate3dPlain(float width, float height, int columns, int rows)
{
    Noisegenerator2d ng = Noisegenerator2d(5, 5, 123);
    vec4 *plain = new vec4[columns * rows];
    for (int i = 0; i < columns * rows; i++)
    {
        float tx = width * (float)(i % columns) / ((float)columns - 1);
        float tz = height * (float)(i / columns) / ((float)rows - 1);
        float ty = ng.perlin((tx / width) * 3.0f, (tz / height) * 3.0f);
        ty *= 3;
        ty *= ty;
        plain[i] = vec4(tx, ty, tz, 1.0);
    }
    return plain;
}

int *generateTriangles(int columns, int rows)
{
    int *triangels = new int[((columns - 1) * (rows - 1)) * 2 * 3];

    int t = 0;
    for (int i = 0; i < rows - 1; i++)
    {
        for (int j = 0; j < columns - 1; j++)
        {
            int topleft = (i * columns) + j;
            int topright = topleft + 1;
            int bottomleft = topleft + columns;
            int bottomright = bottomleft + 1;

            triangels[t] = topleft;
            triangels[t + 1] = topright;
            triangels[t + 2] = bottomright;
            t += 3;

            triangels[t] = topleft;
            triangels[t + 1] = bottomright;
            triangels[t + 2] = bottomleft;
            t += 3;
        }
    }
    return triangels;
}

GLuint generateHills(GLuint *vao)
{
    float width = 20.0;
    float height = 20.0;

    int columns = 20;
    int rows = 20;

    int faceC = ((columns - 1) * (rows - 1)) * 2;

    vec4 *plain = generate3dPlain(width, height, columns, rows);
    vec4 *colors = new vec4[columns * rows];
    int *triangles = generateTriangles(columns, rows);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            vec4 v = plain[(columns * i) + j] / 2.0f;
            // std::cout << v.y << " $$ ";
            colors[(columns * i) + j] = vec4(v.y, v.y, v.y, 1.0f);
        }
        // std::cout << std::endl;
    }
    *vao = 0;
    // create the VAO
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    // create buffers for our vertex data
    GLuint buffers[3];
    glGenBuffers(3, buffers);

    //vertex coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * columns * rows, plain, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
    glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * columns * rows, colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
    glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB1, 4, GL_FLOAT, 0, 0, 0);

    //texture coordinates buffer
    //index buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * faceC * 3, triangles, GL_STATIC_DRAW);

    // unbind the VAO
    glBindVertexArray(0);

    return faceC;
}
