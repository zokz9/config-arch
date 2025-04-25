#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Глобальные переменные для управления движением
float offsetX = 0.0f;
float offsetY = 0.0f;
float speed = 0.05f;  // Скорость движения при нажатии клавиш

// Вершинный шейдер (2D) с матрицей трансформации
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 aPos;  // Теперь vec2 вместо vec3 (только x, y)
    
    uniform mat4 transform; // Матрица трансформации
    
    void main() {
        gl_Position = transform * vec4(aPos, 0.0, 1.0);  // Применяем матрицу трансформации
    }
)glsl";

// Фрагментный шейдер (оранжевый цвет)
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 1.0, 0.2, 1.0);  // Оранжевый
    }
)glsl";

// Функция обработки нажатий клавиш
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Обрабатываем только нажатия и удержания клавиш
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_A:  // Клавиша A - движение влево
                offsetX -= speed;
                break;
            case GLFW_KEY_D:  // Клавиша D - движение вправо
                offsetX += speed;
                break;
            case GLFW_KEY_W:  // Клавиша W - движение вверх
                offsetY += speed;
                break;
            case GLFW_KEY_S:  // Клавиша S - движение вниз
                offsetY -= speed;
                break;
        }
    }
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Ошибка GLFW!" << std::endl;
        return -1;
    }

    // Настройка OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(1000, 600, "2D OpenGL Triangle", NULL, NULL);
    if (!window) {
        std::cerr << "Ошибка создания окна!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Устанавливаем функцию обратного вызова для обработки клавиш
    glfwSetKeyCallback(window, key_callback);

    // Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Ошибка GLEW!" << std::endl;
        return -1;
    }

    // Компиляция шейдеров
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Создание шейдерной программы
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Удаляем шейдеры (они уже в программе)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Координаты вершин (2D)
    float vertices[] = {
        -0.3f, -0.3f,  // Левая нижняя
         0.3f, -0.3f,  // Правая нижняя
         0.0f,  0.3f   // Верхняя
    };
    
    // Находим максимальное и минимальное значение x для определения границ
    float maxX = vertices[0];
    float minX = vertices[0];
    for (int i = 0; i < 6; i += 2) { // Проходим по всем x-координатам (каждый второй элемент)
        if (vertices[i] > maxX) maxX = vertices[i];
        if (vertices[i] < minX) minX = vertices[i];
    }
    
    // Вычисляем ширину треугольника
    float triangleWidth = maxX - minX;
    // Вычисляем границы экрана с учетом ширины треугольника
    float rightBound = 1.0f - maxX;
    float leftBound = -1.0f - minX;

    // Создание VAO и VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Настройка VAO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Указываем, как читать атрибуты вершин
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Отвязываем буферы
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Получаем расположение uniform-переменной
    GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    
    // Главный цикл рендеринга
    while (!glfwWindowShouldClose(window)) {
        // Ограничиваем движение треугольника границами экрана
        if (offsetX > rightBound) offsetX = rightBound;
        if (offsetX < leftBound) offsetX = leftBound;
        
        // Создаем матрицу трансформации
        glm::mat4 transform = glm::mat4(1.0f); // Единичная матрица
        transform = glm::translate(transform, glm::vec3(offsetX, offsetY, 0.0f));
        
        // Очистка экрана (серый фон)
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Рисуем треугольник с применением матрицы трансформации
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Обновляем экран
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
