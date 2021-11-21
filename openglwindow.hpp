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
  bool m_screenFocus{false};
  abcg::ElapsedTimer m_mouseTimer{};

  void renderGun();
  void renderRoom();
  void renderTarget();

  void update();
  glm::vec2 getRotationSpeedFromMouse();
};

#endif