#define STB_PERLIN_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <random>
#include "stb_perlin.h"
#include <iostream>

const int WIDTH = 600;
const int HEIGHT = 600;

int option;

void CreatePixel(int x, int y, float r, float g, float b) {
    // Convert color values from 0-255 range to 0.0-1.0 range
    glColor3f(r, g, b);

    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

void FillPixels() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 1);

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            int randomNumb = distr(gen);
            if (randomNumb == 1)
                CreatePixel(x, y, 1.0f, 1.0f, 1.0f);
        }
    }
}

// Function to generate Perlin noise
float perlinNoise(int x, int y, float time) {
    float noise = stb_perlin_noise3((float)x * 0.05f, (float)y * 0.05f, time * 0.1f, 0.1f, 0.1f, 0.1f);
    return noise;
}

// Function to generate a Perlin noise texture
void generatePerlinTexture(unsigned char* data, int width, int height, float time) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Get the Perlin noise value for each pixel
            float noise = perlinNoise(x, y, time);

            // Normalize the noise value to be in the range [0, 255]
            int color = (int)((noise + 1.0f) * 127.5f);  // Map -1..1 to 0..255

            // Set the pixel color in the data array (RGB)
            data[(y * width + x) * 3 + 0] = color; // Red
            data[(y * width + x) * 3 + 1] = color; // Green
            data[(y * width + x) * 3 + 2] = color; // Blue
        }
    }
}

int main(void) {
    std::cin >> option;

    GLFWwindow* window;
    unsigned char* textureData;

    // Initialize GLFW
    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(WIDTH, HEIGHT, "Perlin Noise Texture", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set up OpenGL for texture rendering
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Allocate texture memory
    textureData = (unsigned char*)malloc(WIDTH * HEIGHT * 3);
    if (textureData == nullptr) {
        std::cerr << "Failed to allocate memory for texture data!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Projection setup for orthogonal view
    if (option == 1) {
        glViewport(0, 0, WIDTH, HEIGHT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    }

    while (!glfwWindowShouldClose(window)) {
        // Get the current time to animate the noise
        float time = (float)glfwGetTime() * 1;

        // Generate Perlin noise texture with current time
        generatePerlinTexture(textureData, WIDTH, HEIGHT, time);

        // Update the texture with new data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering different options based on user input
        if (option == 1) {
            FillPixels();
        }

        if (option == 2) {
            // Render the texture on a quad
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
            glEnd();
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    free(textureData);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
