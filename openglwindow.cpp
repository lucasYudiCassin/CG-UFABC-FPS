#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>

void OpenGLWindow::handleEvent(SDL_Event& event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;

    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;

    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_truckSpeed = -1.0f;

    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_truckSpeed = 1.0f;

    if (event.key.keysym.sym == SDLK_ESCAPE) {
      SDL_SetRelativeMouseMode(SDL_FALSE);
      m_relativeMouse = false;
    }
  }

  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;

    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;

    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_truckSpeed < 0)
      m_truckSpeed = 0.0f;

    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
  }

  if (event.type == SDL_MOUSEBUTTONDOWN) {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    m_relativeMouse = true;
  }

  if (event.type == SDL_MOUSEMOTION) {
    m_mouseMovement = glm::vec2{
        m_mouseMovement.x + event.motion.xrel,
        m_mouseMovement.y + event.motion.yrel,
    };
  }
}

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "depth.vert",
                                    getAssetsPath() + "depth.frag");

  // Load model
  m_gunModel.loadObj(getAssetsPath() + "GUN_OBJ.obj", true);
  m_gunModel.setupVAO(m_program);

  m_roomModel.loadObj(getAssetsPath() + "ROOM_V3.obj", true);
  m_roomModel.setupVAO(m_program);

  m_targetModel.loadObj(getAssetsPath() + "target.obj", true);
  m_targetModel.setupVAO(m_program);

  m_camera.initializeCamera();
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  renderRoom();
  renderTarget();
  renderGun();
}

void OpenGLWindow::paintUI() { abcg::OpenGLWindow::paintUI(); }

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  m_gunModel.terminateGL();
  m_roomModel.terminateGL();
  m_targetModel.terminateGL();
  abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // const auto currentPosition{m_trackBall.project(mousePosition)};
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);

  if (m_mouseMovement.x == 0 && m_mouseMovement.y == 0) return;
  glm::vec2 rotationSpeed = getMouseRotationSpeed();
  m_camera.pan(rotationSpeed.x * deltaTime);
  m_camera.tilt(rotationSpeed.y * deltaTime);
}

void OpenGLWindow::renderGun() {
  glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables used by every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);

  glm::mat4 modelMatrix{1.0f};

  // m_axis = {0.0f, 1.0f, 0.0f};
  // Rotation angle
  modelMatrix = glm::translate(modelMatrix, m_camera.m_eye);

  // translate gun forward
  modelMatrix = glm::translate(
      modelMatrix, 0.5f * glm::normalize(m_camera.m_at - m_camera.m_eye));

  // translate gun horizontally

  // translate gun vertically

  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 0.15f));
  modelMatrix = glm::rotate(modelMatrix, 90.0f, m_camera.m_up);
  // modelMatrix =
  //     glm::rotate(modelMatrix, 1.0f, -(m_camera.m_at + m_camera.m_eye));

  // m_gunModel.m_modelMatrix = translateMatrix * rotateMatrix * scalingMatrix;

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);  // White
  m_gunModel.render();
  glUseProgram(0);
}

void OpenGLWindow::renderRoom() {
  glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};

  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables used by every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);

  glm::mat4 modelMatrix{1.0f};

  // Rotation angle
  const auto angle = glm::radians(90.0f);
  modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f, 2.0f, 2.0f));
  modelMatrix = glm::rotate(modelMatrix, -angle, m_camera.m_up);

  // glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), angle, m_axis);

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 0.8f, 0.8f, 0.8f, 1.0f);

  m_roomModel.render();
  glUseProgram(0);
}

void OpenGLWindow::renderTarget() {
  glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables used by every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);

  glm::mat4 modelMatrix{1.0f};
  // Rotation angle
  modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.13f, 0.13f, 0.13f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 0.0f, 0.0f, 0.7f, 1.0f);

  m_targetModel.render();
  glUseProgram(0);
}

glm::vec2 OpenGLWindow::getMouseRotationSpeed() {
  if (m_relativeMouse) {
    SDL_WarpMouseInWindow(nullptr, m_viewportWidth / 2, m_viewportHeight / 2);
  }

  float speedScale{5.0f};

  glm::vec2 mouseMovement{m_mouseMovement.x, -m_mouseMovement.y};

  m_mouseMovement = glm::vec2{0, 0};

  return glm::vec2{mouseMovement.x * speedScale, mouseMovement.y * speedScale};
}