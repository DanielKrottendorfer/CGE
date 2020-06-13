/* srand example */
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <math.h>
#include <iostream>
#include <GL/glew.h>

#include "glm/vec4.hpp"
#include "objloader.hpp"
#include "Vertex.hpp"

using namespace glm;

class Mesh
{
private:
  GLuint vao = 0;
  unsigned nrOfTriangles;
  Vertex *vertexArray = new Vertex;

  TransformationData tester;

  glm::vec3 position;
  glm::vec3 origin;
  glm::vec3 rotation;
  glm::vec3 scale;

  glm::mat4 ModelMatrix;

  int loadObject(const char * path)
  {
      int res = loadOBJ(path, vertexArray->vertices, vertexArray->uvs, vertexArray->normals);
      return res;
  }

  void initVao(int counter)
  {
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, counter * sizeof(glm::vec4) * 3, &vertexArray->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
    glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, counter * sizeof(glm::vec4) * 3, &vertexArray->normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::NORMAL_ATTRIB);
    glVertexAttribPointer(VSShaderLib::NORMAL_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, counter * sizeof(glm::vec2) * 3, &vertexArray->uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VSShaderLib::TEXTURE_COORD_ATTRIB);
    glVertexAttribPointer(VSShaderLib::TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);


    glBindVertexArray(0);
  }

public:
  Mesh(const char * path,
    glm::vec3 position = glm::vec4(0.f),
		glm::vec3 origin = glm::vec4(0.f),
		glm::vec3 rotation = glm::vec4(0.f),
		glm::vec3 scale = glm::vec4(1.f))
  {
      this->position = position;
      this->origin = origin;
      this->rotation = rotation;
      this->scale = scale;
      this->nrOfTriangles = loadObject(path);
      initVao(nrOfTriangles);
  }

  ~Mesh()
  {

  }

  void drawStuff()
  {
    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, this->nrOfTriangles * 3 *5);
    glBindVertexArray(0);
  }

};
