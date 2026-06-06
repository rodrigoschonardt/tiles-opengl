#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define STBI_HEADER_FILE_ONLY
#include "stb_image.h"

const int MAP_WIDTH = 3;
const int MAP_HEIGHT = 3;
const int SPRITESHEET_TILES_X = 7;
const int SPRITESHEET_TILES_Y = 1;

int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 4},
    {4, 1, 4},
    {4, 4, 1}
};

int cursorX = 1;
int cursorY = 1;
int TILE_WIDTH = 0;
int TILE_HEIGHT = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_W) {
            if (cursorY > 0) { cursorY--; }
        } else if (key == GLFW_KEY_S) {
            if (cursorY < MAP_HEIGHT - 1) { cursorY++; }
        } else if (key == GLFW_KEY_D) {
            if (cursorX < MAP_WIDTH - 1) { cursorX++; }
        } else if (key == GLFW_KEY_A) {
            if (cursorX > 0) { cursorX--; }
        } else if (key == GLFW_KEY_SPACE) { // altera tile
            tilemap[cursorY][cursorX] = (tilemap[cursorY][cursorX] + 1) % SPRITESHEET_TILES_X;
        }
    }
}

GLuint loadTexture(const char* filename) {
    int width, height, channels;
    stbi_uc* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return 0;
    }

    TILE_WIDTH = width / SPRITESHEET_TILES_X;
    TILE_HEIGHT = height / SPRITESHEET_TILES_Y;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texture;
}

void drawTile(int x, int y, int tileIdx, int xOffset) {
    int tx = tileIdx % SPRITESHEET_TILES_X;
    int ty = tileIdx / SPRITESHEET_TILES_X;

    float u0 = (float)tx / SPRITESHEET_TILES_X;
    float v0 = (float)ty / SPRITESHEET_TILES_Y;
    float u1 = (float)(tx + 1) / SPRITESHEET_TILES_X;
    float v1 = (float)(ty + 1) / SPRITESHEET_TILES_Y;

    int xpos = (x - y) * (TILE_WIDTH / 2) + xOffset;
    int ypos = (x + y) * (TILE_HEIGHT / 2);

    glBegin(GL_QUADS);
    glTexCoord2f(u0, v0); glVertex2i(xpos, ypos);
    glTexCoord2f(u1, v0); glVertex2i(xpos + TILE_WIDTH, ypos);
    glTexCoord2f(u1, v1); glVertex2i(xpos + TILE_WIDTH, ypos + TILE_HEIGHT);
    glTexCoord2f(u0, v1); glVertex2i(xpos, ypos + TILE_HEIGHT);
    glEnd();
}

void renderMap(GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    int xOffset = 400 - (TILE_WIDTH / 2);
    int yOffset = 200;

    glPushMatrix();
    glTranslatef(0, (float)yOffset, 0);

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            drawTile(x, y, tilemap[y][x], xOffset);
        }
    }

    glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
    drawTile(cursorX, cursorY, 6, xOffset);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glPopMatrix();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Tilemap Isométrico", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLuint texture = loadTexture("spritesheet.png");
    if (texture == 0) return -1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double lastTime = glfwGetTime();
    bool gameWon = false;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        if (!gameWon && currentTime - lastTime >= 10.0) {
            int rx = rand() % MAP_WIDTH;
            int ry = rand() % MAP_HEIGHT;
            // Reverte para outra cor
            tilemap[ry][rx] = rand() % (SPRITESHEET_TILES_X - 1);
            lastTime = currentTime;
        }

        // Check win condition
        bool allPink = true;
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                if (tilemap[y][x] != 6) {
                    allPink = false;
                    break;
                }
            }
            if (!allPink) break;
        }

        if (allPink && !gameWon) {
            std::cout << "Parabéns, todos os pisos estão rosa!" << std::endl;
            gameWon = true;
        }

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderMap(texture);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteTextures(1, &texture);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
