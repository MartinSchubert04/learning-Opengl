#include "clases/Shader.h"
#include <glm/glm.hpp>
#include <string>

struct Vertex {
  glm::vec3 postion;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

struct Texture {
  unsigned int id;
  std::string type;
};

class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures);

  void draw(Shader &shader);

private:
  unsigned int VAO, VBO, EBO;

  void setupMesh();
};
