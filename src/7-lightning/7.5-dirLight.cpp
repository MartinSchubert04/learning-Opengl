#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ext/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

#include <clases/Shader.h>
#include <clases/Camera.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void updateFrameRate(GLFWwindow *window);
unsigned int loadTexture(const char *path);

std::vector<float> generateSphere(int xSegments, int ySegments);
std::vector<unsigned int> getSphereIndices(int xSegments, int ySegments);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 700;
const float PI = 3.14159265359f;

glm::vec4 const background_color = {0.2f, 0.3f, 0.3f, 1.0f};

float deltaTime = 0.0f;  // Time between current frame and last frame
float lastFrame = 0.0f;  // Time of last frame
bool firstMouse = true;

bool uiMode = true;

float previousTime = glfwGetTime();
int frameCount = 0;

float lastX = float(SCR_WIDTH) / 2, lastY = float(SCR_HEIGHT) / 2;

Camera camera(glm::vec3(0.0, 0.0, 5.0));

glm::vec3 lightPos(1, 1, 2);
glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, 0.0f);

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  uiMode ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL)
         : glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // mouse input callbacks
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader program
  // ------------------------------------
  Shader shader("shaders/7/5/color.vs", "shaders/7/5/color.fs");
  Shader lightCubeShader("shaders/7/5/lightSource.vs",
                         "shaders/7/5/lightSource.fs");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  unsigned int VBO, cubeVAO, EBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(cubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // light sphere  ----

  std::vector<float> sphereVertices = generateSphere(64, 64);
  std::vector<unsigned int> sphereIndices = getSphereIndices(64, 64);
  unsigned int sphereVAO, sphereVBO, sphereEBO;

  glGenVertexArrays(1, &sphereVAO);
  glGenBuffers(1, &sphereVBO);
  glGenBuffers(1, &sphereEBO);

  glBindVertexArray(sphereVAO);

  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
  glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float),
               sphereVertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sphereIndices.size() * sizeof(unsigned int),
               sphereIndices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  unsigned int diffuseMap = loadTexture("resources/textures/container2.png");
  unsigned int specularMap =
      loadTexture("resources/textures/container2_specular.png");

  unsigned int emisionMap = loadTexture("resources/textures/matrix.jpg");

  // imgui setup
  // -----------
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  float cubeScalar = 1;
  bool reduceCube = false;
  float lightDistance = 3.0f;
  bool rotate = true;
  bool sphereGravitate = false;
  float angle = glm::radians(25.0f);
  glm::vec3 lightAmbient = {0.2f, 0.2f, 0.2f};
  glm::vec3 lightDiffuse = {0.5f, 0.5f, 0.5f};
  glm::vec3 lightSpecular = {1.0f, 1.0f, 1.0f};

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  shader.use();
  shader.setInt("material.diffuse", 0);
  shader.setInt("material.specular", 1);
  shader.setInt("material.emission", 2);

  while (!glfwWindowShouldClose(window)) {

    processInput(window);

    glClearColor(background_color.r, background_color.g, background_color.b,
                 background_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("ImGui");
    ImGui::Text("Object settings");
    ImGui::SliderAngle("AngleRotation", &angle);
    ImGui::SliderFloat("Size", &cubeScalar, 0.1, 2);
    ImGui::Text("Light settings");
    ImGui::Checkbox("Light gravitate", &sphereGravitate);

    if (ImGui::BeginTable("Ambient", 4,
                          ImGuiTableFlags_SizingFixedFit |
                              ImGuiTableFlags_NoPadInnerX)) {

      ImGui::TableSetupColumn("R", ImGuiTableColumnFlags_WidthFixed, 140.0f);
      ImGui::TableSetupColumn("G", ImGuiTableColumnFlags_WidthFixed, 140.0f);
      ImGui::TableSetupColumn("B", ImGuiTableColumnFlags_WidthFixed, 140.0f);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::SliderFloat("R", &lightAmbient.r, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(1);
      ImGui::SliderFloat("G", &lightAmbient.g, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(2);
      ImGui::SliderFloat("B", &lightAmbient.b, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(3);
      ImGui::Text("Ambient");

      ImGui::EndTable();
    }

    if (ImGui::BeginTable("Diffuse", 4,
                          ImGuiTableFlags_SizingFixedFit |
                              ImGuiTableFlags_NoPadInnerX)) {

      ImGui::TableSetupColumn("R", ImGuiTableColumnFlags_WidthFixed, 140.0f);
      ImGui::TableSetupColumn("G", ImGuiTableColumnFlags_WidthFixed, 140.0f);
      ImGui::TableSetupColumn("B", ImGuiTableColumnFlags_WidthFixed, 140.0f);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::SliderFloat("R", &lightDiffuse.r, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(1);
      ImGui::SliderFloat("G", &lightDiffuse.g, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(2);
      ImGui::SliderFloat("B", &lightDiffuse.b, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(3);
      ImGui::Text("Diffuse");

      ImGui::EndTable();
    }
    if (ImGui::BeginTable("Specular", 4,
                          ImGuiTableFlags_SizingFixedFit |
                              ImGuiTableFlags_NoPadInnerX)) {

      ImGui::TableSetupColumn("R", ImGuiTableColumnFlags_WidthFixed, 140.0f);
      ImGui::TableSetupColumn("G", ImGuiTableColumnFlags_WidthFixed, 140.0f);
      ImGui::TableSetupColumn("B", ImGuiTableColumnFlags_WidthFixed, 140.0f);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::SliderFloat("R", &lightSpecular.r, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(1);
      ImGui::SliderFloat("G", &lightSpecular.g, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(2);
      ImGui::SliderFloat("B", &lightSpecular.b, 0.0f, 1.0f);

      ImGui::TableSetColumnIndex(3);
      ImGui::Text("Specular");

      ImGui::EndTable();
    }

    updateFrameRate(window);

    shader.use();
    shader.setVec3("viewPos", camera.Position);
    shader.setFloat("time", glfwGetTime());

    shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    shader.setFloat("material.shininess", 32.0f);

    shader.setVec3("light.ambient", lightAmbient);
    shader.setVec3("light.diffuse",
                   lightDiffuse);  // darken diffuse light a bit
    shader.setVec3("light.specular", lightSpecular);
    shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);

    glBindVertexArray(cubeVAO);

    for (unsigned int i = 0; i < 10; i++) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      model = glm::rotate(model, angle, glm::vec3(3.0f, 6.0f, 1.0f));
      model = glm::scale(model, glm::vec3(cubeScalar));
      shader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glm::mat4 view =
        glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

    // create projection matrix
    glm::mat4 projection;
    projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emisionMap);

    // rotar luz al redetor
    if (sphereGravitate) {
      lightPos.x = sin(glfwGetTime()) * lightDistance;
      lightPos.y = 0;
      lightPos.z = cos(glfwGetTime()) * lightDistance;
    } else {
      lightPos.y = -0.3;
      lightPos.x = 0.7;
      // lightPos.z = -0.5;
    }

    // also draw the lamp object
    // lightCubeShader.use();
    // lightCubeShader.setMat4("projection", projection);
    // lightCubeShader.setMat4("view", view);
    // glm::mat4 model = glm::mat4(1.0);
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, lightPos);
    // model = glm::scale(model, glm::vec3(0.2f));  // a smaller cube
    // lightCubeShader.setMat4("model", model);

    // glBindVertexArray(sphereVAO);
    // glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &sphereVAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  static bool rightMousePressedLastFrame = false;

  bool rightMousePressedNow =
      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

  if (rightMousePressedNow && !rightMousePressedLastFrame) {
    uiMode = !uiMode;

    if (uiMode) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      firstMouse = true;
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      firstMouse = true;
    }
  }

  rightMousePressedLastFrame = rightMousePressedNow;
  if (uiMode)
    return;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.ProcessKeyboard(UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  if (uiMode)
    return;

  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos;  // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void updateFrameRate(GLFWwindow *window) {
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  frameCount++;

  // Actualizar cada 0.25 segundos
  if (currentFrame - previousTime >= 0.25) {
    float fps = frameCount / (currentFrame - previousTime);

    std::string newTitle = "LearnOpenGL - FPS: " + std::to_string((int)fps);
    glfwSetWindowTitle(window, newTitle.c_str());

    previousTime = currentFrame;
    frameCount = 0;
  }
}
// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

std::vector<float> generateSphere(int xSegments, int ySegments) {
  std::vector<float> vertices;

  for (int y{0}; y <= ySegments; y++) {
    for (int x{0}; x <= xSegments; x++) {
      float xSeg = float(x) / xSegments;
      float ySeg = float(y) / ySegments;

      float xPos = std::cos(xSeg * 2.0f * PI) * std::sin(ySeg * PI);
      float yPos = std::cos(ySeg * PI);
      float zPos = std::sin(xSeg * 2.0f * PI) * std::sin(ySeg * PI);

      vertices.push_back(xPos);
      vertices.push_back(yPos);
      vertices.push_back(zPos);
    }
  }

  return vertices;
}

std::vector<unsigned int> getSphereIndices(int xSegments, int ySegments) {
  std::vector<unsigned int> indices;
  for (unsigned int y = 0; y < ySegments; ++y) {
    for (unsigned int x = 0; x < xSegments; ++x) {
      indices.push_back((y + 1) * (xSegments + 1) + x);
      indices.push_back(y * (xSegments + 1) + x);
      indices.push_back(y * (xSegments + 1) + x + 1);

      indices.push_back((y + 1) * (xSegments + 1) + x);
      indices.push_back(y * (xSegments + 1) + x + 1);
      indices.push_back((y + 1) * (xSegments + 1) + x + 1);
    }
  }

  return indices;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
