#ifndef MODEL_HPP_
#define MODEL_HPP_

#include <vector>

#include "abcg.hpp"

struct Vertex {
  glm::vec3 position{};

  bool operator==(const Vertex& other) const noexcept {
    return position == other.position;
  }
};

class Model {
 public:
  void loadObj(std::string_view path, GLuint m_program,
               bool standardize = true);
  void render() const;
  void setupVAO();
  void terminateGL();

  // [[nodiscard]] int getNumTriangles() const {
  //   return static_cast<int>(m_indices.size()) / 3;
  // }
  glm::mat4 m_modelMatrix{1.0f};

  GLuint m_program{};
  glm::vec4 m_color{1.0f};

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float m_scale{1.0f};
  void createBuffers();
  void standardize();
};

#endif