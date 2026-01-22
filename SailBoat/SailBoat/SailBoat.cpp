#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

//2d transformation er jonno matrix
struct Mat4 {
    float m[16];
    Mat4() {
        for (int i = 0; i < 16; i++) m[i] = 0;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
};

Mat4 translate(float x, float y) {
    Mat4 r;
    r.m[12] = x;
    r.m[13] = y;
    return r;
}

Mat4 rotateZ(float deg) {
    Mat4 r;
    float rad = deg * 3.14159f / 180.0f;
    r.m[0] = cos(rad);   r.m[1] = sin(rad);
    r.m[4] = -sin(rad);  r.m[5] = cos(rad);
    return r;
}

Mat4 scale(float s) {
    Mat4 r;
    r.m[0] = r.m[5] = s;
    return r;
}

Mat4 multiply(Mat4 a, Mat4 b) {
    Mat4 r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                r.m[i * 4 + j] += a.m[i * 4 + k] * b.m[k * 4 + j];
    return r;
}

// shaders
const char* vs =
"#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"uniform mat4 transform;\n"
"void main(){ gl_Position = transform * vec4(aPos,1.0); }\0";

const char* fs =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 color;\n"
"void main(){ FragColor = vec4(color,1.0); }\0";


float posX = 0.0f;
float scl = 1.0f;
float angle = 0.0f;

// Pivot point -- > mast er top 
const float PIVOT_X = 0.0f;
const float PIVOT_Y = 0.45f;

// sensitivity
const float MOVE = 0.002f;
const float SCALE = 0.002f;
const float ROTATE = 0.05f;  
const float MIN_ANGLE = -90.0f;
const float MAX_ANGLE = 90.0f;


void input(GLFWwindow* w) {
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) posX -= MOVE;
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) posX += MOVE;
    if (glfwGetKey(w, GLFW_KEY_X) == GLFW_PRESS) scl += SCALE;
    if (glfwGetKey(w, GLFW_KEY_C) == GLFW_PRESS) scl -= SCALE;

    // R = clockwise rotation
    if (glfwGetKey(w, GLFW_KEY_R) == GLFW_PRESS) angle -= ROTATE;

    // T = anti-clockwise rotation
    if (glfwGetKey(w, GLFW_KEY_T) == GLFW_PRESS) angle += ROTATE;

    //scalilng e criteria
    if (scl < 0.1f) scl = 0.1f;
    if (scl > 2.6f) scl = 2.6f;
	//rotation e criteria
    if (angle < MIN_ANGLE) angle = MIN_ANGLE;
    if (angle > MAX_ANGLE) angle = MAX_ANGLE;


    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);
}


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Sailboat", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Shader somuho
    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vs, NULL);
    glCompileShader(v);

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fs, NULL);
    glCompileShader(f);

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);
    glDeleteShader(v);
    glDeleteShader(f);


    float hull[] = {
        // main hull er coordinates
        -0.45f, -0.05f, 0.0f,
         0.45f, -0.05f, 0.0f,
         0.45f, -0.30f, 0.0f,

        -0.45f, -0.05f, 0.0f,
         0.45f, -0.30f, 0.0f,
        -0.45f, -0.30f, 0.0f
    };

    // left side panel er coordinates
    float leftPanel[] = {
        -0.45f, -0.05f, 0.0f,
        -0.75f, 0.10f, 0.0f,
        -0.45f, -0.30f, 0.0f
    };

    // Right side panel er coordinates
    float rightPanel[] = {
        0.45f, -0.05f, 0.0f,
        0.45f, -0.30f, 0.0f,
        0.75f, 0.10f, 0.0f
    };

    // Mast er coordinates
    float mast[] = {
        -0.015f, -0.05f, 0.0f,
         0.015f, -0.05f, 0.0f,
         0.015f, 0.45f, 0.0f,

        -0.015f, -0.05f, 0.0f,
         0.015f, 0.45f, 0.0f,
        -0.015f, 0.45f, 0.0f
    };

    // Flag er coordinates  
    float flag[] = {
        0.015f, 0.40f, 0.0f,
        0.015f, 0.04f, 0.0f,
        0.30f, 0.25f, 0.0f
    };

    unsigned int VAO[5], VBO[5];
    glGenVertexArrays(5, VAO);
    glGenBuffers(5, VBO);

    auto setup = [&](int i, float* v, int sz) {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sz, v, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        };

    setup(0, hull, sizeof(hull));
    setup(1, leftPanel, sizeof(leftPanel));
    setup(2, rightPanel, sizeof(rightPanel));
    setup(3, mast, sizeof(mast));
    setup(4, flag, sizeof(flag));


    while (!glfwWindowShouldClose(window)) {
        input(window);
        glClearColor(0.6f, 0.8f, 1.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        int tLoc = glGetUniformLocation(prog, "transform");
        int cLoc = glGetUniformLocation(prog, "color");

        Mat4 S = scale(scl);
        Mat4 T = translate(posX, 0.0f);
        Mat4 R = rotateZ(angle);

        Mat4 M = multiply(R, multiply(T, S));

        glUniformMatrix4fv(tLoc, 1, false, M.m);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Main hull 
        glUniform3f(cLoc, 0.6f, 0.35f, 0.1f);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Left panel
        glUniform3f(cLoc, 0.5f, 0.28f, 0.08f);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Right panel
        glBindVertexArray(VAO[2]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Mast 
        glUniform3f(cLoc, 0.1f, 0.1f, 0.1f);
        glBindVertexArray(VAO[3]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Flag 
        glUniform3f(cLoc, 0.0f, 0.0f, 0.0f);
        glBindVertexArray(VAO[4]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
