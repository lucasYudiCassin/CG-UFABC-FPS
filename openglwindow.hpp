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
  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Id{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Is{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f, 1.0f, 1.0f, 1.0f};
  float m_shininess{25.0f};

  void renderGun();
  void renderRoom();
  void renderTarget();
  void render(GLuint m_program, glm::mat4 modelMatrix, glm::vec4 color);

  void update();
  glm::vec2 getMouseRotationSpeed();

  // std::vector<const char*> m_shaderNames{"normal", "depth"};
  // std::vector<GLuint> m_programs;
  // int m_currentProgramIndex{-1};
};

#endif