#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

// Управление камерой
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
float rotationAngleY = 0.0f;
float rotationAngleX = 0.0f;
float speed = 0.05f;
float rotationSpeed = 7.0f;
float scale = 0.5f;

const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos, 1.0);
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(0.5, 0.8, 1.0, 1.0); // Голубой цвет
    }
)glsl";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_A: cameraPos.x -= speed; break;    // Влево по X
            case GLFW_KEY_D: cameraPos.x += speed; break;    // Вправо по X
            case GLFW_KEY_W: cameraPos.z -= speed; break;    // Вперед по Z
            case GLFW_KEY_S: cameraPos.z += speed; break;    // Назад по Z
            case GLFW_KEY_Q: cameraPos.y += speed; break;    // Вверх по Y
            case GLFW_KEY_E: cameraPos.y -= speed; break;    // Вниз по Y
            case GLFW_KEY_LEFT: rotationAngleY += rotationSpeed; break;
            case GLFW_KEY_RIGHT: rotationAngleY -= rotationSpeed; break;
            case GLFW_KEY_UP: rotationAngleX += rotationSpeed; break;
            case GLFW_KEY_DOWN: rotationAngleX -= rotationSpeed; break;
            case GLFW_KEY_PAGE_UP: scale += 0.1f; break;   
            case GLFW_KEY_PAGE_DOWN: scale -= 0.1f; break;
        }
    }
}

// ... (остальные функции остаются без изменений, включая generateCubeVertices())

int main() {
    // ... (инициализация окна и OpenGL остается без изменений)

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Матрица модели (теперь без перемещения)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotationAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale));

        // Матрица вида с новой позицией камеры
        glm::mat4 view = glm::lookAt(
            cameraPos,                   // Позиция камеры
            glm::vec3(0.0f, 0.0f, 0.0f), // Смотрим в центр
            glm::vec3(0.0f, 1.0f, 0.0f)  // Вектор "вверх"
        );

        // ... (остальная часть кода с матрицей проекции и рендерингом остается без изменений)
    }

    // ... (очистка ресурсов остается без изменений)
}