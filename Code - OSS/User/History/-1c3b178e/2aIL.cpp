#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

// Управление
float offsetX = 0.0f;
float offsetY = 0.0f;
float rotationAngle = 0.0f;
float speed = 0.05f;
float rotationSpeed = 5.0f;
float scale = 0.5f;
int sides = 5;

const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos, 0.0, 1.0);
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
            case GLFW_KEY_A: offsetX -= speed; break;
            case GLFW_KEY_D: offsetX += speed; break;
            case GLFW_KEY_W: offsetY += speed; break;
            case GLFW_KEY_S: offsetY -= speed; break;
            case GLFW_KEY_LEFT: rotationAngle += rotationSpeed; break;
            case GLFW_KEY_RIGHT: rotationAngle -= rotationSpeed; break;
            case GLFW_KEY_PAGE_UP: scale += 0.1f; break;   
            case GLFW_KEY_PAGE_DOWN: scale -= 0.1f; break;
            case GLFW_KEY_UP: sides++; break; // Увеличиваем количество сторон
            case GLFW_KEY_DOWN: if (sides > 3) sides--; break; // Уменьшаем (но не меньше 3)
        }
    }
}

std::vector<float> generateNGonVertices(int n) {
    std::vector<float> vertices;
    const float radius = 0.5f;
    const float centerX = 0.0f;
    const float centerY = 0.0f;
    
    for (int i = 0; i < n; i++) {
        // Первая точка - центр
        vertices.push_back(centerX);
        vertices.push_back(centerY);
        
        // Вторая точка - текущая вершина на окружности
        float angle1 = 2.0f * M_PI * i / n;
        vertices.push_back(centerX + radius * cos(angle1));
        vertices.push_back(centerY + radius * sin(angle1));
        
        // Третья точка - следующая вершина на окружности
        float angle2 = 2.0f * M_PI * (i + 1) / n;
        vertices.push_back(centerX + radius * cos(angle2));
        vertices.push_back(centerY + radius * sin(angle2));
    }
    
    return vertices;
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "N-угольник", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (glewInit() != GLEW_OK) return -1;

    // Компиляция шейдеров
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Генерация вершин n-угольника
    auto vertices = generateNGonVertices(sides);
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");

    while (!glfwWindowShouldClose(window)) {
        // Серый фон
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Обновляем вершины если изменилось количество сторон
        static int prevSides = sides;
        if (sides != prevSides) {
            vertices = generateNGonVertices(sides);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
            prevSides = sides;
        }

        // Матрица преобразования
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(offsetX, offsetY, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(scale, scale, 0.0f));

        // Рендеринг
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}