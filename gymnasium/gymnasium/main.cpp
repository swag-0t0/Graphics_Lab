#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "camera.h"
#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int createCube();
unsigned int createCylinder(int segments);
void drawGym(Shader& shader);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(35.0f, 8.0f, 0.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool lightOn = true;
float fanRotation = 0.0f;
bool fanRunning = false;
float doorAngle = 0.0f;
float windowSlide = 0.0f;
float treadmillBelt = 0.0f;
bool treadmillRunning = false;
float cycleWheel = 0.0f;
bool cycleRunning = false;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Gymnasium", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("vertexshader.vs", "fragmentshader.fs");

    // Start view: place camera at room's right face and look inward (-X)
    camera.Position = glm::vec3(35.0f, 8.0f, 0.0f);
    camera.Front = glm::vec3(-1.0f, 0.0f, 0.0f);

    std::cout << "\n=== GYMNASIUM CONTROLS ===" << std::endl;
    std::cout << "Camera Movement:" << std::endl;
    std::cout << "  W/S - Forward/Backward" << std::endl;
    std::cout << "  A/D - Left/Right" << std::endl;
    std::cout << "  E/R - Up/Down" << std::endl;
    std::cout << "\nCamera Rotation:" << std::endl;
    std::cout << "  X - Pitch (up/down view)" << std::endl;
    std::cout << "  Y - Yaw (left/right view)" << std::endl;
    std::cout << "  Z - Roll (tilt view)" << std::endl;
    std::cout << "  F - Toggle orbit mode" << std::endl;
    std::cout << "\nInteractions:" << std::endl;
    std::cout << "  L - Toggle lights" << std::endl;
    std::cout << "  G - Toggle fan" << std::endl;
    std::cout << "  O/C - Open/Close door" << std::endl;
    std::cout << "  P/K - Slide windows" << std::endl;
    std::cout << "  T - Toggle treadmills" << std::endl;
    std::cout << "  B - Toggle exercise bikes" << std::endl;
    std::cout << "========================\n" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        if (fanRunning) {
            fanRotation += 360.0f * deltaTime;
            if (fanRotation > 360.0f) fanRotation -= 360.0f;
        }

        if (treadmillRunning) {
            treadmillBelt += 3.0f * deltaTime;
            if (treadmillBelt > 2.0f) treadmillBelt -= 2.0f;
        }

        if (cycleRunning) {
            cycleWheel += 180.0f * deltaTime;
            if (cycleWheel > 360.0f) cycleWheel -= 360.0f;
        }

        camera.UpdateOrbit(deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        shader.setVec3("lightPos", 0.0f, 15.0f, 0.0f);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setBool("lightOn", lightOn);

        drawGym(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
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
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_X)
            camera.RotatePitch(10.0f);
        else if (key == GLFW_KEY_Y)
            camera.RotateYaw(10.0f);
        else if (key == GLFW_KEY_Z)
            camera.RotateRoll(10.0f);
        else if (key == GLFW_KEY_F)
            camera.ToggleOrbitMode();
        else if (key == GLFW_KEY_L)
            lightOn = !lightOn;
        else if (key == GLFW_KEY_G)
            fanRunning = !fanRunning;
        else if (key == GLFW_KEY_O) {
            doorAngle += 15.0f;
            if (doorAngle > 90.0f) doorAngle = 90.0f;
        }
        else if (key == GLFW_KEY_C) {
            doorAngle -= 15.0f;
            if (doorAngle < 0.0f) doorAngle = 0.0f;
        }
        else if (key == GLFW_KEY_P) {
            windowSlide += 0.5f;
            if (windowSlide > 2.0f) windowSlide = 2.0f;
        }
        else if (key == GLFW_KEY_K) {
            windowSlide -= 0.5f;
            if (windowSlide < 0.0f) windowSlide = 0.0f;
        }
        else if (key == GLFW_KEY_T)
            treadmillRunning = !treadmillRunning;
        else if (key == GLFW_KEY_B)
            cycleRunning = !cycleRunning;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int createCube()
{
    float vertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

unsigned int createCylinder(int segments)
{
    std::vector<float> vertices;

    // Lateral surface
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = cos(angle);
        float z = sin(angle);

        vertices.push_back(x * 0.5f);
        vertices.push_back(0.5f);
        vertices.push_back(z * 0.5f);
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);

        vertices.push_back(x * 0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(z * 0.5f);
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
    }

    // Top cap (filled circle)
    vertices.push_back(0.0f);
    vertices.push_back(0.5f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = cos(angle);
        float z = sin(angle);

        vertices.push_back(x * 0.5f);
        vertices.push_back(0.5f);
        vertices.push_back(z * 0.5f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
    }

    // Bottom cap (filled circle)
    vertices.push_back(0.0f);
    vertices.push_back(-0.5f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(-1.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = cos(angle);
        float z = sin(angle);

        vertices.push_back(x * 0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(z * 0.5f);
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

void drawGym(Shader& shader)
{
    static unsigned int cubeVAO = createCube();
    static unsigned int cylinderVAO = createCylinder(32);
    glm::mat4 model;

    // Floor
    shader.setVec3("objectColor", 0.35f, 0.35f, 0.38f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(50.0f, 0.2f, 40.0f));
    shader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Back wall
    shader.setVec3("objectColor", 0.82f, 0.82f, 0.85f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 5.0f, -20.0f));
    model = glm::scale(model, glm::vec3(50.0f, 10.0f, 0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Left wall with window openings
    shader.setVec3("objectColor", 0.82f, 0.82f, 0.85f);

    // Left wall - bottom section
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-25.0f, 1.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 3.0f, 40.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Left wall - top section
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-25.0f, 7.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 5.0f, 40.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Middle section of left wall 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-25.0f, 4.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 3.0f, 40.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Right wall (front) with door opening
    // Right wall - left of door
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5.0f, 5.0f, 20.0f));
    model = glm::scale(model, glm::vec3(30.0f, 10.0f, 0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Right wall - right of door
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-17.5f, 5.0f, 20.0f));
    model = glm::scale(model, glm::vec3(15.0f, 10.0f, 0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Right wall - above door
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-11.5f, 8.0f, 20.0f));
    model = glm::scale(model, glm::vec3(3.0f, 4.0f, 0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Ceiling
    shader.setVec3("objectColor", 0.88f, 0.88f, 0.90f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
    model = glm::scale(model, glm::vec3(50.0f, 0.2f, 40.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Sliding Window (single, functional)
    shader.setVec3("objectColor", 0.4f, 0.7f, 0.9f);
    // Window frame
    shader.setVec3("objectColor", 0.3f, 0.3f, 0.3f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-25.0f, 4.5f, -11.0f));
    model = glm::scale(model, glm::vec3(0.6f, 3.0f, 5.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Glass panel 1 (sliding)
    shader.setVec3("objectColor", 0.5f, 0.75f, 0.95f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-24.7f, 4.5f, -12.0f - windowSlide));
    model = glm::scale(model, glm::vec3(0.1f, 3.0f, 2.4f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Glass panel 2 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-24.7f, 4.5f, -12.0f + 2.5f));
    model = glm::scale(model, glm::vec3(0.1f, 3.0f, 2.4f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Door
    shader.setVec3("objectColor", 0.55f, 0.35f, 0.25f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-11.5f, 3.0f, 19.8f));
    model = glm::rotate(model, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(3.0f, 6.0f, 0.2f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Door handle
    shader.setVec3("objectColor", 0.7f, 0.7f, 0.7f);
    glBindVertexArray(cylinderVAO);
    // Build handle model relative to the door transform so it rotates with the door
    glm::mat4 doorModel = glm::mat4(1.0f);
    doorModel = glm::translate(doorModel, glm::vec3(-11.5f, 3.0f, 19.8f));
    doorModel = glm::rotate(doorModel, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    // knob local offset from door center (approx +1.3 on X, slightly inset on Z)
    model = doorModel;
    model = glm::translate(model, glm::vec3(1.3f, 0.0f, -0.2f));
    // orient knob axis and scale
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.3f, 0.1f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
    glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
    glDrawArrays(GL_TRIANGLE_FAN, 100, 34);

    glBindVertexArray(cubeVAO);

    // Ceiling Fan
    shader.setVec3("objectColor", 0.25f, 0.25f, 0.25f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 9.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.8f, 0.4f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Fan blades
    shader.setVec3("objectColor", 0.65f, 0.65f, 0.68f);
    for (int i = 0; i < 4; i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 9.2f, 0.0f));
        model = glm::rotate(model, glm::radians(fanRotation + i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 0.08f, 0.6f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // ===== TREADMILL ZONE (5 treadmills in a row) =====
    for (int t = 0; t < 5; t++) {
        float xPos = -18.0f + t * 4.0f;

        // Treadmill base
        shader.setVec3("objectColor", 0.15f, 0.15f, 0.15f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 0.3f, -15.0f));
        model = glm::scale(model, glm::vec3(2.5f, 0.6f, 5.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Treadmill belt (thin)
        float beltOffset = (treadmillRunning) ? sin(treadmillBelt + t * 0.5f) * 0.1f : 0.0f;
        shader.setVec3("objectColor", 0.08f + beltOffset, 0.08f, 0.08f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 0.65f, -15.0f));
        model = glm::scale(model, glm::vec3(2.3f, 0.05f, 4.5f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Side rails
        shader.setVec3("objectColor", 0.25f, 0.25f, 0.25f);
        for (int side = 0; side < 2; side++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos + (side - 0.5f) * 1.3f, 1.2f, -15.5f));
            model = glm::scale(model, glm::vec3(0.15f, 1.4f, 3.5f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Handle bars
        glBindVertexArray(cylinderVAO);
        shader.setVec3("objectColor", 0.3f, 0.3f, 0.3f);
        for (int side = 0; side < 2; side++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos + (side - 0.5f) * 1.3f, 1.8f, -16.5f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.08f, 1.2f, 0.08f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
            glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
            glDrawArrays(GL_TRIANGLE_FAN, 100, 34);
        }

        // Front support posts
        for (int side = 0; side < 2; side++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos + (side - 0.5f) * 1.3f, 1.0f, -17.0f));
            model = glm::scale(model, glm::vec3(0.08f, 2.0f, 0.08f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
            glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
            glDrawArrays(GL_TRIANGLE_FAN, 100, 34);
        }

        glBindVertexArray(cubeVAO);

        // Console panel
        shader.setVec3("objectColor", 0.2f, 0.2f, 0.22f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 2.2f, -17.2f));
        model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.8f, 0.8f, 0.1f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Console screen
        shader.setVec3("objectColor", 0.1f, 0.3f, 0.5f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 2.2f, -17.15f));
        model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.4f, 0.6f, 0.05f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // ===== EXERCISE BIKE ZONE =====
    for (int b = 0; b < 5; b++) {
        float xPos = -18.0f + b * 4.0f;
        float zPos = -5.0f;

        // Bike base/mat supporting the bike
        shader.setVec3("objectColor", 0.4f, 0.4f, 0.42f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 0.05f, zPos));
        model = glm::scale(model, glm::vec3(0.8f, 0.3f, 3.0f));
        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Seat post
        glBindVertexArray(cylinderVAO);
        shader.setVec3("objectColor", 0.3f, 0.3f, 0.3f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 1.4f, zPos + 0.2f));
        model = glm::scale(model, glm::vec3(0.12f, 1.0f, 0.12f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
        glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
        glDrawArrays(GL_TRIANGLE_FAN, 100, 34);

        // Seat
        glBindVertexArray(cubeVAO);
        shader.setVec3("objectColor", 0.15f, 0.15f, 0.15f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 2.0f, zPos + 0.2f));
        model = glm::scale(model, glm::vec3(0.7f, 0.25f, 1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Handlebar post
        glBindVertexArray(cylinderVAO);
        shader.setVec3("objectColor", 0.3f, 0.3f, 0.3f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 1.0f, zPos - 0.8f));
        model = glm::scale(model, glm::vec3(0.12f, 3.7f, 0.12f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
        glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
        glDrawArrays(GL_TRIANGLE_FAN, 100, 34);

        // Handlebars
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 2.8f, zPos - 0.8f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.08f, 1.2f, 0.08f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);

        // Central axle (thin cylinder under seat)
        shader.setVec3("objectColor", 0.25f, 0.25f, 0.25f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 0.8f, zPos + 0.2f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.08f, 1.5f, 0.08f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);

        // Flywheel (main wheel - positioned at center under seat)
        glBindVertexArray(cylinderVAO);
        shader.setVec3("objectColor", 0.2f, 0.2f, 0.2f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 0.8f, zPos + 0.2f));
        model = glm::rotate(model, glm::radians(90.0f + (cycleRunning ? cycleWheel + b * 30.0f : 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 0.12f, 1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);

        // Pedal crank arms (2 sticks attached to flywheel)
        shader.setVec3("objectColor", 0.35f, 0.35f, 0.35f);
        for (int p = 0; p < 2; p++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos, 0.8f, zPos + 0.2f));
            model = glm::rotate(model, glm::radians(90.0f + (cycleRunning ? cycleWheel + b * 30.0f : 0.0f) + (p * 180.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.06f, 0.5f, 0.06f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
            glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
            glDrawArrays(GL_TRIANGLE_FAN, 100, 34);

            // Pedal foot (at the end of crank)
            glBindVertexArray(cubeVAO);
            shader.setVec3("objectColor", 0.15f, 0.15f, 0.15f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos, 0.8f, zPos + 0.2f));
            model = glm::rotate(model, glm::radians(90.0f + (cycleRunning ? cycleWheel + b * 30.0f : 0.0f) + (p * 180.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, glm::vec3(0.9f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.2f, 0.1f, 0.3f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            glBindVertexArray(cylinderVAO);
        }

        // Console screen
        glBindVertexArray(cubeVAO);
        shader.setVec3("objectColor", 0.1f, 0.3f, 0.5f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 2.9f, zPos - 0.8f));
        model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f, 0.5f, 0.05f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // ===== WEIGHT BENCH AREA (3 benches) =====
    for (int w = 0; w < 3; w++) {
        float xPos = -10.0f + w * 7.0f;
        float zPos = 8.0f;

        // Bench pad
        shader.setVec3("objectColor", 0.12f, 0.12f, 0.12f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 1.2f, zPos));
        model = glm::scale(model, glm::vec3(3.5f, 0.4f, 5.4f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Bench legs
        shader.setVec3("objectColor", 0.25f, 0.25f, 0.25f);
        for (int leg = 0; leg < 2; leg++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos + (leg - 0.5f) * 3.5f, 0.6f, zPos));
            model = glm::scale(model, glm::vec3(0.3f, 1.2f, 1.2f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Barbell rack posts
        glBindVertexArray(cylinderVAO);
        shader.setVec3("objectColor", 0.3f, 0.3f, 0.3f);
        for (int post = 0; post < 2; post++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos + (post - 0.5f) * 3.5f, 2.0f, zPos));
            model = glm::scale(model, glm::vec3(0.15f, 2.5f, 0.15f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);

            // Rack hooks
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos + (post - 0.5f) * 3.5f, 3.2f, zPos));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.12f, 0.5f, 0.12f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
        }

        // Barbell bar
        shader.setVec3("objectColor", 0.5f, 0.5f, 0.55f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, 3.4f, zPos));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.12f, 5.0f, 0.12f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
        glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
        glDrawArrays(GL_TRIANGLE_FAN, 100, 34);

        // Weight plates (3 per side) — draw as thin disks at the bar edges
        shader.setVec3("objectColor", 0.08f, 0.08f, 0.08f);
        float plateRadii[3] = {0.45f, 0.35f, 0.25f};
        for (int side = 0; side < 2; side++) {
            for (int plate = 0; plate < 3; plate++) {
                model = glm::mat4(1.0f);
                // place plate center at the bar end (use bar half-length ~2.5)
                float sideOffset = (side - 0.5f) * 2.5f;
                model = glm::translate(model, glm::vec3(xPos + sideOffset, 3.4f, zPos));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                // small offset so stacked plates don't z-fight (move along bar axis)
                model = glm::translate(model, glm::vec3(0.0f, plate * 0.09f - 0.09f, 0.0f));
                float r = plateRadii[plate];
                // thin disk: radius r, small thickness
                model = glm::scale(model, glm::vec3(r, 0.08f, r));
                shader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
                glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
                glDrawArrays(GL_TRIANGLE_FAN, 100, 34);
            }
        }

        glBindVertexArray(cubeVAO);
    }

    // ===== DUMBBELL RACK (single support, one layer) =====
    // Single solid support attached flush to the back wall
    shader.setVec3("objectColor", 0.18f, 0.18f, 0.18f);
    model = glm::mat4(1.0f);
    // Place the support so its back face sits near the scene back wall (z ~ -12)
    model = glm::translate(model, glm::vec3(7.5f, 0.8f, -11.7f));
    model = glm::scale(model, glm::vec3(8.8f, 1.1f, 1.8f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Single layer of dumbbells sitting on the support, with varied sizes
    glBindVertexArray(cylinderVAO);
    float sizes[5] = {0.14f, 0.18f, 0.16f, 0.20f, 0.12f};
    for (int db = 0; db < 5; db++) {
        float xPos = 4.0f + db * 1.6f;
        float yPos = 1.5f; // slightly above the support
        float zPos = -11.5f; // in front of the support face

        // Dumbbell handle
        shader.setVec3("objectColor", 0.35f, 0.35f, 0.38f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, yPos, zPos));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.08f, 0.6f, 0.08f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
        glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
        glDrawArrays(GL_TRIANGLE_FAN, 100, 34);

        // Dumbbell weights (both ends) with varying radius
        shader.setVec3("objectColor", 0.08f, 0.08f, 0.08f);
        float r = sizes[db];
        for (int end = 0; end < 2; end++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos + (end - 0.5f) * 0.5f, yPos, zPos));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(r, 0.22f, r));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
            glDrawArrays(GL_TRIANGLE_FAN, 66, 34);
            glDrawArrays(GL_TRIANGLE_FAN, 100, 34);
        }
    }

    glBindVertexArray(cubeVAO);

    // ===== YOGA/STRETCHING AREA =====
    // Yoga mats (8 mats in 2 rows)
    for (int row = 0; row < 2; row++) {
        for (int mat = 0; mat < 3; mat++) {
            float colors[4][3] = {
                {0.7f, 0.2f, 0.8f},
                {0.2f, 0.7f, 0.3f},
                {0.8f, 0.5f, 0.2f},
                {0.2f, 0.5f, 0.8f}
            };

            shader.setVec3("objectColor", colors[mat][0], colors[mat][1], colors[mat][2]);
            model = glm::mat4(1.0f);
            // Move yoga mats to the left side along the left wall
            model = glm::translate(model, glm::vec3(-23.0f + mat * 3.5f, 0.12f, 10.0f - row * 2.5f));
            model = glm::scale(model, glm::vec3(1.5f, 0.05f, 4.5f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Exercise balls
    glBindVertexArray(cylinderVAO);
    for (int ball = 0; ball < 4; ball++) {
        shader.setVec3("objectColor", 0.2f, 0.5f + ball * 0.1f, 0.8f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(18.0f, 0.8f, 15.0f - ball * 2.0f));
        model = glm::scale(model, glm::vec3(1.6f, 1.6f, 1.6f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 66);
    }

}