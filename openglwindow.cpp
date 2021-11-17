#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>

void OpenGLWindow::handleEvent(SDL_Event& event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBall.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mousePress(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mouseRelease(mousePosition);
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? 1.0f : -1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 2.0f);
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
  m_gunModel.loadObj(getAssetsPath() + "GUN_OBJ.obj", m_program, true);
  m_gunModel.setupVAO();

  m_roomModel.loadObj(getAssetsPath() + "EmptyRoom.obj", m_program, true);
  m_roomModel.setupVAO();
  setGunPostition();
  // m_trianglesToDraw = m_gunModel.getNumTriangles();

  // glm::ivec2 mousePosition;
  // SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
  // m_trackBall.mousePress(mousePosition);
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  // const GLint modelMatrixLoc{
  //     abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables used by every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);

  // Set uniform variables of the current object
  // abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE,
  //                          &m_gunModel.m_modelMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);  // White

  m_gunModel.render();
  m_roomModel.render();

  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  // Create a window for the other widgets
  {
    const auto widgetSize{ImVec2(222, 90)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    static bool faceCulling{};
    ImGui::Checkbox("Back-face culling", &faceCulling);

    if (faceCulling) {
      abcg::glEnable(GL_CULL_FACE);
    } else {
      abcg::glDisable(GL_CULL_FACE);
    }

    // CW/CCW combo box
    {
      static std::size_t currentIndex{};
      const std::vector<std::string> comboItems{"CCW", "CW"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Front face",
                            comboItems.at(currentIndex).c_str())) {
        for (const auto index : iter::range(comboItems.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      if (currentIndex == 0) {
        abcg::glFrontFace(GL_CCW);
      } else {
        abcg::glFrontFace(GL_CW);
      }
    }

    // Projection combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto index : iter::range(comboItems.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      if (currentIndex == 0) {
        const auto aspect{static_cast<float>(m_viewportWidth) /
                          static_cast<float>(m_viewportHeight)};
        m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
      } else {
        m_projMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 5.0f);
      }
    }

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_trackBall.resizeViewport(width, height);
}

void OpenGLWindow::terminateGL() {
  m_gunModel.terminateGL();
  m_roomModel.terminateGL();
  abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  // m_gunModel.m_modelMatrix =
  //     glm::rotate(m_gunModel.m_modelMatrix, glm::radians(0.05f), m_axis);
  // m_modelMatrix = m_modelMatrix;
  // m_gunModel.m_modelMatrix =
  //     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  const auto currentPosition{m_trackBall.project(mousePosition)};
  m_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f /*+ m_zoom*/),
                             currentPosition, glm::vec3(0.0f, 1.0f, 0.0f));

  // m_viewMatrix =
  //     glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom),
  //                 glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // m_gunModel.m_modelMatrix = m_viewMatrix;
}

void OpenGLWindow::setGunPostition() {
  m_axis = {0.0f, 1.0f, 0.0f};
  // Rotation angle
  const auto angle = glm::radians(100.0f);
  glm::mat4 scalingMatrix =
      glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
  glm::mat4 translateMatrix =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -0.12f, 1.4f));
  glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), angle, m_axis);

  m_gunModel.m_modelMatrix = translateMatrix * rotateMatrix * scalingMatrix;
}