#pragma once
#define STB_IMAGE_IMPLEMENTATION
#define GL_CLAMP_TO_EDGE 0x812F
#include "stb_image.h"
#include <gl/glut.h>
#include <iostream>

// Global variables

float posX = -0.5f, posY = -0.5f; // Position of the image
float sizeX = 0.5f, sizeY = 0.5f; // Size of the image
    int width, height, nrChannels;

    void loadTexture(GLuint& texture, const char* filename, int& width, int& height) {
        int nrChannels;
        unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Failed to load texture: " << filename << std::endl;
            exit(1);
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Use appropriate texture format
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        stbi_image_free(data);
    }

    void tdisplay(GLuint& texture, float scale, float width, float height , float sprites, float selected) {
        glColor3f(1, 1 ,1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);



        // Define the region of the texture to display
        float sprite_size = 1/sprites;
        float texX1 = 0; // Start X (normalized: 0.0 to 1.0)
        float texY1 = sprite_size*selected; // Start Y (normalized: 0.0 to 1.0)
        float texX2 = 1; // End X (normalized: 0.0 to 1.0)
        float texY2 = sprite_size * (selected+1); // End Y (normalized: 0.0 to 1.0)
        height /= sprites;

        glBegin(GL_QUADS);
        glTexCoord2f(texX1, texY1); glVertex2f(-width * scale, -height * scale);
        glTexCoord2f(texX2, texY1); glVertex2f(width * scale, -height * scale);
        glTexCoord2f(texX2, texY2); glVertex2f(width * scale, height * scale);
        glTexCoord2f(texX1, texY2); glVertex2f(-width * scale, height * scale);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

    }


