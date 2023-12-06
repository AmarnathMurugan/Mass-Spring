#ifndef INTERACTIONHANDLER_H
#define INTERACTIONHANDLER_H

#include <Camera.h>
#include <GLFW/glfw3.h>
#include <materials/material.h>

#include <iostream>

extern std::vector<std::shared_ptr<material>> materials;
extern std::shared_ptr<Camera> Cam;
extern std::shared_ptr<DirectionalLight> Light;
bool isLeftMouseButtonPressed = false, isRightMouseButtonPressed = false, isCtrlPressed = false;
glm::vec2 prevMousePosition, deltaMousePosition;



void KeyboardInputsCallback(GLFWwindow* window, int key, int scancode,
                            int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (key == GLFW_KEY_F6 && action == GLFW_RELEASE) {
    for (int i = 0; i < materials.size(); i++) materials[i]->RecompileShaders();
    Cam->triggerUpdateMvpEvents();
    std::cerr << "\n Shaders recompiled";
  }

  if (key == GLFW_KEY_P && action == GLFW_RELEASE) Cam->SwitchProjectionType();

  if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) isCtrlPressed = true;
  if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) isCtrlPressed = false;
}

void WindowResizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  WindowWidth = width;
  WindowHeight = height;
  Cam->UpdatePerspectiveMatrix();
}

void MouseInputCallback(GLFWwindow* window, int button, int action, int mods) {
  isLeftMouseButtonPressed = (button == GLFW_MOUSE_BUTTON_LEFT);
  isRightMouseButtonPressed = (button == GLFW_MOUSE_BUTTON_RIGHT);
  if ((isLeftMouseButtonPressed || isRightMouseButtonPressed) &&
      action == GLFW_PRESS) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    prevMousePosition = glm::vec2(x, y);
  }
  if (isLeftMouseButtonPressed && action == GLFW_RELEASE)
    isLeftMouseButtonPressed = false;
  if (isRightMouseButtonPressed && action == GLFW_RELEASE)
    isRightMouseButtonPressed = false;
}

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
  deltaMousePosition = glm::vec2(xpos, ypos) - prevMousePosition;
  prevMousePosition = glm::vec2(xpos, ypos);
  if (!isCtrlPressed)
  {
      if (isLeftMouseButtonPressed)
          Cam->RotateCamera(deltaMousePosition.y * 0.002,
              deltaMousePosition.x * 0.002);
  }
  else
  {
      if (isLeftMouseButtonPressed)
          Light->RotateLight(deltaMousePosition.y * 0.002,
              deltaMousePosition.x * 0.002);
  }
  if (isRightMouseButtonPressed) Cam->MoveAlongRay(deltaMousePosition.y * 0.2);
}

void InitInteractionHandlers(GLFWwindow* window) {
  glfwSetKeyCallback(window, KeyboardInputsCallback);
  glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
  glfwSetMouseButtonCallback(window, MouseInputCallback);
  glfwSetCursorPosCallback(window, MouseMoveCallback);
}

#endif  // !INTERACTIONHANDLER_H
