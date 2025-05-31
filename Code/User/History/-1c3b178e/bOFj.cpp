#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

// Параметры камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float yaw = -90.0f;
float pitch = 1.0f;
float lastX = 400.0f;
float lastY = 400.0f;
float fov = 65.0f;
bool firstMouse = true;

// Шейдеры
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    uniform mat3 normalMatrix;
    out vec3 Normal;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        Normal = normalize(normalMatrix * aNormal);
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    in vec3 Normal;
    uniform vec3 triangleColor;
    out vec4 FragColor;
    void main() {
        float lightStrength = 1.0 - dot(Normal, normalize(vec3(0.5, 0.5, -1.0)));
        FragColor = vec4(triangleColor * lightStrength, 1.0);
    }
)glsl";

// Ввод данных от пользователя
void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}

// Обработка мыши
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

std::vector<Vertex> createCubeVerticesWithNormals() {
    return {
        // Передняя грань
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},

        // Задняя грань
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

        // Левая грань
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},

        // Правая грань
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},

        // Верхняя грань
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},

        // Нижняя грань
        {{-0.5f, 0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}, {{0.5f, 0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}, {{0.5f, 0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}}, {{-0.5f, 0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}}, {{-0.5f, 0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}
    };
}

struct Triangle {
    int index;
    float distance;
    glm::vec3 center;
    glm::vec3 normal;
    float gradientStep;
};

std::vector<Triangle> generateTrianglesInfo(const std::vector<Vertex>& vertices) {
    std::vector<Triangle> triangles;
    for (int i = 0; i < 12; ++i) {
        int base = i * 3;
        glm::vec3 a = vertices[base].position;
        glm::vec3 b = vertices[base + 1].position;
        glm::vec3 c = vertices[base + 2].position;

        // Центр треугольника
        glm::vec3 center = (a + b + c) / 3.0f;

        // Нормаль треугольника (фронтальная плоскость)
        glm::vec3 edge1 = b - a;
        glm::vec3 edge2 = c - a;
        glm::vec3 triangleNormal = glm::normalize(glm::cross(edge1, edge2));

        float distance = glm::dot(cameraPos - center, glm::normalize(triangleNormal));
        triangles.push_back({i, distance, center, triangleNormal, 0.0f});
    }

    // Сортировка по расстоянию
    std::sort(
        triangles.begin(),
        triangles.end(),
        [](const Triangle& a, const Triangle& b) { return a.distance > b.distance; }
    );

    return triangles;
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "3D Тени с Градацией", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK) return -1;
    glEnable(GL_DEPTH_TEST);

    // Шейдеры
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Вертексы с нормалями
    std::vector<Vertex> vertices = createCubeVerticesWithNormals();
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint normalMatrixLoc = glGetUniformLocation(shaderProgram, "normalMatrix");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "triangleColor");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Матрицы
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 800.0f, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        // Вычисление нормалей
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        glBindVertexArray(VAO);

        // Треугольники
        std::vector<Triangle> triangles = generateTrianglesInfo(vertices);
        int total = triangles.size();
        for (int i = 0; i < total; ++i) {
            float gradient = static_cast<float>(i) / static_cast<float>(total);
            glm::vec3 color = glm::vec3(0.5f, 0.5f, 1.0f) * (1.0f - gradient) + glm::vec3(0.0f, 0.3f, 0.6f) * gradient;
            glUniform3fv(colorLoc, 1, glm::value_ptr(color));

            glDrawArrays(GL_TRIANGLES, triangles[i].index * 3, 3);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
