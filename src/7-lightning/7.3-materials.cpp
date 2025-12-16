#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ext/matrix_transform.hpp"

#include <clases/Shader.h>
#include <clases/Camera.h>
#define STB_IMAGE_IMPLEMENTATION
#define SPECULAR_IN_VS

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void updateFrameRate(GLFWwindow *window);
std::vector<float> generateSphere(int xSegments, int ySegments);
std::vector<unsigned int> getSphereIndices(int xSegments, int ySegments);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float PI = 3.14159265359f;

glm::vec4 const background_color = {0.2f, 0.3f, 0.3f, 1.0f};

float deltaTime = 0.0f;  // Time between current frame and last frame
float lastFrame = 0.0f;  // Time of last frame
bool firstMouse = true;

float previousTime = glfwGetTime();
int frameCount = 0;

float lastX = float(SCR_WIDTH) / 2, lastY = float(SCR_HEIGHT) / 2;

Camera camera(glm::vec3(0.0, 0.0, 5.0));

glm::vec3 lightPos(1.2, 1, 2);
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
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
#ifdef SPECULAR_IN_VS
  Shader shader("shaders/7/2.1/color.vs", "shaders/7/2.1/color.fs");
  Shader lightCubeShader("shaders/7/2.1/lightSource.vs",
                         "shaders/7/2.1/lightSource.fs");
#else
  Shader shader("shaders/7/2/color.vs", "shaders/7/2/color.fs");
  Shader lightCubeShader("shaders/7/2/lightSource.vs",
                         "shaders/7/2/lightSource.fs");
#endif
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

  unsigned int VBO, cubeVAO, EBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(cubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

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

  // imgui setup
  // -----------
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  float cubeScalar = 2;
  bool reduceCube = false;
  const float radius = 3.0f;
  bool rotate = true;
  // render loop
  // -----------

  while (!glfwWindowShouldClose(window)) {
    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(background_color.r, background_color.g, background_color.b,
                 background_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Test ImGui");
    ImGui::Text("Hello world");
    ImGui::Checkbox("Rotate", &rotate);
    ImGui::SliderFloat("Size", &cubeScalar, 0.1, 2);

    updateFrameRate(window);

    shader.use();
    shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("viewPos", camera.Position);
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    glm::mat4 model = glm::mat4(1.0f);

    // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f),
    //                     glm::vec3(0.5f, 1.0f, 0.0f));

    if (rotate) {
      model = glm::rotate(model, 20.0f, glm::vec3(3.0f, 6.0f, 1.0f));
    }

    // achicar cubo
    reduceCube ? cubeScalar -= 0.1 *deltaTime : cubeScalar += 0.1 * deltaTime;
    if (cubeScalar >= 2)
      reduceCube = true;
    if (cubeScalar <= 1)
      reduceCube = false;
    model = glm::scale(model, glm::vec3(cubeScalar));  // a smaller cube

    glm::mat4 view =
        glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

    // create projection matrix
    glm::mat4 projection;
    projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    shader.setMat4("projection", projection);
    shader.setMat4("model", model);
    shader.setMat4("view", view);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // rotar luz al redetor
#if 1
    lightPos.x = sin(glfwGetTime()) * radius;
    lightPos.y = 0;
    lightPos.z = cos(glfwGetTime()) * radius;
#else
    lightPos.y = 0;
    lightPos.x = 0;
#endif

    // also draw the lamp object
    lightCubeShader.use();
    lightCubeShader.setMat4("projection", projection);
    lightCubeShader.setMat4("view", view);

    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));  // a smaller cube
    lightCubeShader.setMat4("model", model);

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

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
