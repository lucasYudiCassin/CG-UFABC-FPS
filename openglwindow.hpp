#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"
#include "camera.hpp"
#include "model.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  Model m_gunModel;
  Model m_roomModel;
  Model m_targetModel;

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  glm::vec2 m_mouseMovement{0.0f};
  bool m_relativeMouse{true};
  abcg::ElapsedTimer m_mouseTimer{};

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
  int m_mappingMode{};

  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess{};

  void loadModel(std::string_view path);
  void renderGun();
  void renderRoom();
  void renderTarget();
  void render(GLuint m_program, glm::mat4 modelMatrix, glm::vec4 color,
              Model obj);

  void update();
  glm::vec2 getMouseRotationSpeed();

  // std::vector<const char*> m_shaderNames{"normal", "depth"};
  // std::vector<GLuint> m_programs;
  // int m_currentProgramIndex{-1};
};

#endif