#pragma once
#include "entities.h"
GLuint floortile;
GLuint duplicate(GLuint floortile, int repeatX, int repeatY) {
    // Bind the original texture
    glBindTexture(GL_TEXTURE_2D, floortile);

    // Get the original texture size
    int texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Allocate memory for the original texture
    unsigned char* originalImage = new unsigned char[texWidth * texHeight * 4]; // RGBA (4 bytes per pixel)
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, originalImage);

    // Calculate new texture size
    int newWidth = texWidth * repeatX;
    int newHeight = texHeight * repeatY;

    // Create a buffer for the repeated texture
    unsigned char* repeatedImage = new unsigned char[newWidth * newHeight * 4];

    // Copy and tile the texture
    for (int i = 0; i < repeatY; i++) {
        for (int j = 0; j < repeatX; j++) {
            for (int y = 0; y < texHeight; y++) {
                for (int x = 0; x < texWidth; x++) {
                    int origIndex = (y * texWidth + x) * 4;
                    int newIndex = ((i * texHeight + y) * newWidth + (j * texWidth + x)) * 4;

                    repeatedImage[newIndex] = originalImage[origIndex];     // Red
                    repeatedImage[newIndex + 1] = originalImage[origIndex + 1]; // Green
                    repeatedImage[newIndex + 2] = originalImage[origIndex + 2]; // Blue
                    repeatedImage[newIndex + 3] = originalImage[origIndex + 3]; // Alpha
                }
            }
        }
    }

    // Generate a new OpenGL texture
    GLuint newTexture;
    glGenTextures(1, &newTexture);
    glBindTexture(GL_TEXTURE_2D, newTexture);

    // Upload to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, repeatedImage);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Free CPU memory
    delete[] originalImage;
    delete[] repeatedImage;

    return newTexture;
}



int bgx, bgy;
int x=1512 , y = 1512;
void init_bg() {
	loadTexture(floortile, "..\\Textures\\background2.jpg", bgx, bgy);
    floortile = duplicate(floortile, 5, 5);
}
void background(int posx, int posy) {
    glColor3f(1, 1, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floortile);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPushMatrix();
    int xdis = posx % 302, ydis = posy % 302;
    glTranslatef(-xdis ,-ydis, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-x, -y);
    glTexCoord2f(1, 0); glVertex2f(x, -y);
    glTexCoord2f(1, 1); glVertex2f(x, y);
    glTexCoord2f(0, 1); glVertex2f(-x, y);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}


