#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>

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
  m_program = createProgramFromFile(getAssetsPath() + "shaders/texture.vert",
                                    getAssetsPath() + "shaders/texture.frag");

  // Load model
  m_gunModel.loadModel(getAssetsPath() + "viking_room.obj",
                       getAssetsPath() + "map/viking_room.png", m_program);
  // m_roomModel.loadModel(getAssetsPath() + "ROOM_V4.obj",
  //                       getAssetsPath() + "map/Gun.png", m_program);
  // m_targetModel.loadModel(getAssetsPath() + "target.obj",
  //                         getAssetsPath() + "map/Gun.png", m_program);
  // m_gunModel.loadObj(getAssetsPath() + "GUN_OBJ_V2.obj", true);
  // m_gunModel.setupVAO(m_program);

  // m_roomModel.loadObj(getAssetsPath() + "ROOM_V4.obj", true);
  // m_roomModel.setupVAO(m_program);

  // m_targetModel.loadObj(getAssetsPath() + "target.obj", true);
  // m_targetModel.setupVAO(m_program);
  m_mappingMode = 3;
  m_camera.initializeCamera();
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // renderRoom();
  // renderTarget();
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
  // m_roomModel.terminateGL();
  // m_targetModel.terminateGL();
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
  glm::mat4 modelMatrix{1.0f};

  // m_axis = {0.0f, 1.0f, 0.0f};
  // Rotation angle
  modelMatrix = glm::translate(modelMatrix, m_camera.m_eye);

  // translate gun forward
  modelMatrix = glm::translate(
      modelMatrix, 0.5f * glm::normalize(m_camera.m_at - m_camera.m_eye));

  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 0.15f));
  modelMatrix = glm::rotate(modelMatrix, 90.0f, m_camera.m_up);
  // modelMatrix =
  //     glm::rotate(modelMatrix, 1.0f, -(m_camera.m_at + m_camera.m_eye));

  // m_gunModel.m_modelMatrix = translateMatrix * rotateMatrix * scalingMatrix;

  render(m_program, modelMatrix, {1.0f, 1.0f, 1.0f, 1.0f}, m_gunModel);
}

void OpenGLWindow::renderRoom() {
  glm::mat4 modelMatrix{1.0f};
  // Rotation angle
  const auto angle = glm::radians(90.0f);
  modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f, 2.0f, 2.0f));
  modelMatrix = glm::rotate(modelMatrix, -angle, m_camera.m_up);

  render(m_program, modelMatrix, {0.8f, 0.8f, 0.8f, 1.0f}, m_roomModel);
  // m_roomModel.render();
  // glUseProgram(0);
}

void OpenGLWindow::renderTarget() {
  glm::mat4 modelMatrix{1.0f};
  // Rotation angle
  modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.13f, 0.13f, 0.13f));

  render(m_program, modelMatrix, {0.0f, 0.0f, 0.7f, 1.0f}, m_targetModel);
  // m_targetModel.render();
  // glUseProgram(0);
}

void OpenGLWindow::render(GLuint m_program, glm::mat4 modelMatrix,
                          glm::vec4 color, Model obj) {
  glUseProgram(m_program);
  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};

  const GLint normalMatrixLoc{
      abcg::glGetUniformLocation(m_program, "normalMatrix")};

  // const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  const GLint lightDirLoc{
      abcg::glGetUniformLocation(m_program, "lightDirWorldSpace")};
  const GLint shininessLoc{abcg::glGetUniformLocation(m_program, "shininess")};
  const GLint IaLoc{abcg::glGetUniformLocation(m_program, "Ia")};
  const GLint IdLoc{abcg::glGetUniformLocation(m_program, "Id")};
  const GLint IsLoc{abcg::glGetUniformLocation(m_program, "Is")};
  const GLint KaLoc{abcg::glGetUniformLocation(m_program, "Ka")};
  const GLint KdLoc{abcg::glGetUniformLocation(m_program, "Kd")};
  const GLint KsLoc{abcg::glGetUniformLocation(m_program, "Ks")};
  const GLint diffuseTexLoc{
      abcg::glGetUniformLocation(m_program, "diffuseTex")};
  const GLint mappingModeLoc{
      abcg::glGetUniformLocation(m_program, "mappingMode")};

  // Set uniform variables used by every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  // abcg::glUniform1i(mappingModeLoc, m_mappingMode);
  abcg::glUniform1i(mappingModeLoc, 3);

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  const auto modelViewMatrix{glm::mat3(m_camera.m_viewMatrix * modelMatrix)};
  const glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
  // abcg::glUniform4f(colorLoc, color);
  // abcg::glUniform4fv(colorLoc, 1, &color.r);

  abcg::glUniform4fv(lightDirLoc, 1, &m_lightDir.x);

  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  m_Ka = obj.getKa();
  m_Kd = obj.getKd();
  m_Ks = obj.getKs();
  m_shininess = obj.getShininess();

  abcg::glUniform1f(shininessLoc, m_shininess);
  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
  obj.render();
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