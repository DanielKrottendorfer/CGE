/* srand example */
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <math.h>
#include <iostream>
#include <GL/glew.h>

#include "Noisegenerator2d.hpp"
#include "glm/glm.hpp"
#include "vsShaderLib.h"

using namespace glm;

vec4 *generate3dPlain(float width, float height, int columns, int rows)
{
    int a = 10;
    int b = 10;
    Noisegenerator2d ng = Noisegenerator2d(a, b, 2222222);
    int a1 = 2;
    int b1 = 2;
    Noisegenerator2d ng1 = Noisegenerator2d(a1, b1, 111111);
    vec4 *plain = new vec4[columns * rows];
    for (int i = 0; i < columns * rows; i++)
    {
        float tx = width * (float)(i % columns) / ((float)columns - 1);
        float tz = height * (float)(i / columns) / ((float)rows - 1);
        //std::cout << ta << " " << tb << std::endl;
        float ty = ng.perlin((tx / width) * (((float)a) - 1.001), (tz / height) * (((float)b) - 1.001));
        ty += ng1.perlin((tx / width) * (((float)a1) - 1.001), (tz / height) * (((float)b1) - 1.001)) * 2.0;
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

vec3 colorlerp(float c)
{
    vec3 color;
    if (c < 0.3)
    {
        color = vec3(0.0, 1.0, 0.0);
    }
    else if (c < 0.8)
    {
        color = vec3(0.5, 0.5, 0.5);
    }
    else
    {
        color = vec3(0.9, 0.9, 0.9);
    }
    return color;
}

GLuint generateHills(GLuint *vao)
{
    float width = 20.0;
    float height = 20.0;

    int columns = 50;
    int rows = 50;

    int faceC = ((columns - 1) * (rows - 1)) * 2;
    int vertC = faceC * 3;

    vec4 *plain = generate3dPlain(width, height, columns, rows);
    vec4 *colors = new vec4[columns * rows];
    int *triangles = generateTriangles(columns, rows);

    vec4 *verts = new vec4[vertC];
    vec4 *normals = new vec4[vertC];
    vec3 *rgbs = new vec3[vertC];

    for (int i = 0; i < vertC; i += 3)
    {
        int a = triangles[i];
        int b = triangles[i + 1];
        int c = triangles[i + 2];

        vec4 verta = plain[a];
        vec4 vertb = plain[b];
        vec4 vertc = plain[c];

        vec3 rgba = colorlerp(verta.y);
        vec3 rgbb = colorlerp(vertb.y);
        vec3 rgbc = colorlerp(vertc.y);

        vec3 vertab = vec3(verta - vertb);
        vec3 vertac = vec3(verta - vertc);

        vec3 vn = normalize(cross(vertab, vertac));
        if (vn.y < 0.0)
            vn *= -1;

        vec4 vnorm = vec4(vn, 0.0);

        verts[i] = verta;
        verts[i + 1] = vertb;
        verts[i + 2] = vertc;

        normals[i] = vnorm;
        normals[i + 1] = vnorm;
        normals[i + 2] = vnorm;

        rgbs[i] = rgba * 0.5f;
        rgbs[i + 1] = rgbb * 0.5f;
        rgbs[i + 2] = rgbc * 0.5f;
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * vertC, verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
    glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * vertC, normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::NORMAL_ATTRIB);
    glVertexAttribPointer(VSShaderLib::NORMAL_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertC, rgbs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
    glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB1, 3, GL_FLOAT, 0, 0, 0);

    // unbind the VAO
    glBindVertexArray(0);

    return faceC;
}
