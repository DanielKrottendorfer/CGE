#include <glm/vec2.hpp>                 // glm::vec2
#include <glm/vec3.hpp>                 // glm::vec3
#include <glm/vec4.hpp>                 // glm::vec4
#include <glm/mat4x4.hpp>               // glm::mat4

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/transform.hpp>

struct Vertex
{
  std::vector<glm::vec4> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec4> normals;
};

class TransformationData
{
private:

public:
  glm::vec3 position = glm::vec3(0.f);
  glm::vec4 origin = glm::vec4(0.f);
  glm::vec4 rotation = glm::vec4(0.f);
  glm::vec3 scale = glm::vec3(1.f);
  glm::vec3 rotationaxes = vec3(0.f, 1.0, 0.f);
  float angle = 0.0;

  mat4 calculateTransformationMatrix()
  {
    mat4 rotationMatrix = glm::rotate(angle, rotationaxes);
    mat4 translationMatrix = glm::translate(position);
    mat4 scaleMatrix = glm::scale(scale);
    return translationMatrix * rotationMatrix * scaleMatrix;
  }
};
