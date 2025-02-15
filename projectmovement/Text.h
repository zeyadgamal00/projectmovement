#pragma once
#define GL_CLAMP_TO_EDGE 0x812F
#include <gl/glut.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <sstream>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "freetype.lib")
FT_Library ft;
FT_Face face;
GLuint textTexture; 
vector<GLubyte> hextorgb(string hex) {
    if (hex[0] == '#') {
        hex = hex.substr(1);
    }
    transform(hex.begin(), hex.end(), hex.begin(), ::toupper);
    map<char, int> hexadec = { {'0',0}, {'1',1},{'2',2},{'3',3},{'4',4},{'5',5},{'6',6},{'7',7},{'8',8},{'9',9},{'A',10},{'B',11},{'C',12},{'D',13},{'E',14},{'F',15} };
    GLubyte red = hexadec[hex[0]] * 16 + hexadec[hex[1]], green = hexadec[hex[2]] * 16 + hexadec[hex[3]], blue = hexadec[hex[4]] * 16 + hexadec[hex[5]];
    return vector<GLubyte>({ red,green,blue });
}
bool initFreeType() {
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Error initializing FreeType library!" << std::endl;
        return false;
    }

    if (FT_New_Face(ft, "C:\\Users\\Dell\\Downloads\\Jaywalk.ttf", 0, &face)) {
        std::cerr << "Error loading font!" << std::endl;
        return false;
    }
    FT_Set_Pixel_Sizes(face, 0, 200);

    return true;
}

// Function to load a character into a texture
GLuint loadCharacterTexture(FT_ULong charCode) {
    if (FT_Load_Char(face, charCode, FT_LOAD_RENDER)) {
        std::cerr << "Error loading character!" << std::endl;
        return 0;
    }

    FT_GlyphSlot g = face->glyph;
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Ensure OpenGL doesn't expect extra padding in rows.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Get the dimensions and pitch of the bitmap.
    unsigned int width = g->bitmap.width;
    unsigned int height = g->bitmap.rows;
    unsigned int pitch = g->bitmap.pitch;

    // Allocate an RGBA buffer (4 channels per pixel)
    unsigned char* expandedData = new unsigned char[width * height * 4];

    // Fill the RGBA buffer row-by-row, taking pitch into account.
    // FreeType's buffer may have extra padding at the end of each row.
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            // Calculate the index in the source buffer.
            unsigned int indexSrc = row * pitch + col;
            // Calculate the index in the destination buffer.
            unsigned int indexDst = row * width + col;

            // Set R, G, and B channels to 255 (white), and use the bitmap's value for alpha.
            expandedData[4 * indexDst] = 255;                     // Red
            expandedData[4 * indexDst + 1] = 255;                     // Green
            expandedData[4 * indexDst + 2] = 255;                     // Blue
            expandedData[4 * indexDst + 3] = g->bitmap.buffer[indexSrc]; // Alpha
        }
    }

    // Upload the RGBA data to OpenGL.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, expandedData);

    // Free the temporary buffer.
    delete[] expandedData;

    // Set texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return texture;
}



// Function to render text
void renderText(const std::string& text, float x, float y,int fontstize,string colors) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glPushAttrib(GL_TEXTURE_BIT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    FT_Set_Pixel_Sizes(face, 0, fontstize);
    glColor3ubv(hextorgb(colors).data());
    for (char c : text) {

        GLuint texture = loadCharacterTexture(c);
        if (texture == 0) continue;

        FT_GlyphSlot g = face->glyph;

        float xpos = x;
        float ypos = y - (g->bitmap.rows - g->bitmap_top);

        float w = g->bitmap.width;
        float h = g->bitmap.rows;

        // Render texture on the screen as a quad
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos, ypos+h);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(xpos + w, ypos+h);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos + w, ypos);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(xpos, ypos);
        glEnd();

        x += g->advance.x >> 6;  // Advance cursor by the width of the character


        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
    glPopAttrib();
    glPopMatrix();
}
float computeTextWidth(const std::string& text) {
    float width = 0.0f;
    for (char c : text) {
        // Load the character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Error loading character '" << c << "'" << std::endl;
            continue;
        }
        // Advance is stored in 1/64th of a pixel
        width += (face->glyph->advance.x >> 6);
    }
    return width;
}
void renderTextCenter(const std::string& text, float centerX, float y, int fontsize, string colors) {
    float textWidth = computeTextWidth(text);
    float startX = centerX - textWidth / 2.0f;
    FT_Set_Pixel_Sizes(face, 0, fontsize);
    int currline = 0;
    glPushMatrix();
    glTranslatef(startX, y, 0.0f);
    glColor3ubv(hextorgb(colors).data());
    for (char c : text) {
            GLuint texture = loadCharacterTexture(c);
            if (texture == 0) continue;
            FT_GlyphSlot g = face->glyph;
            // Adjust ypos based on the glyph metrics.
            // If you flipped the texture coordinates as suggested earlier,
            // you might need to use:
            // float ypos = - (g->bitmap.rows - g->bitmap_top);
            float ypos = -int(g->bitmap.rows - g->bitmap_top);
            float w = g->bitmap.width;
            float h = g->bitmap.rows;

            // Enable blending and texturing
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);

            glBegin(GL_QUADS);
            // Swap texture coordinates if needed to flip the glyph vertically
            glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, ypos + h);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(w, ypos + h);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(w, ypos);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, ypos);
            glEnd();
            // Advance the current position for the next glyph.
            glTranslatef((face->glyph->advance.x >> 6), 0.0f, 0.0f);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
    }

    glPopMatrix();
}
void renderCenteredText(const std::string& text, float centerX, float y,int fontsize,string colors) { 
        FT_Set_Pixel_Sizes(face, 0, fontsize);
        float newlinedistance = (face->size->metrics.height >> 6)*3;
        std::istringstream iss(text);
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        glPushMatrix();
        glColor3ubv(hextorgb(colors).data());

        // Loop over each line.
        for (size_t i = 0; i < lines.size(); i++) {
            // Compute the width for this line.
            float lineWidth = computeTextWidth(lines[i]);
            // Compute the starting x position for this line so that it is centered.
            float startX = centerX - lineWidth / 2.0f;

            // Save the matrix for this line and translate to the correct position.
            glPushMatrix();
            glTranslatef(startX, y - i * newlinedistance, 0.0f);

            // Render each character of the line.
            for (char c : lines[i]) {
                GLuint texture = loadCharacterTexture(c);
                if (texture == 0)
                    continue;

                FT_GlyphSlot g = face->glyph;
                // Calculate vertical offset for the glyph.
                float ypos = -int(g->bitmap.rows - g->bitmap_top);
                float w = g->bitmap.width;
                float h = g->bitmap.rows;

                // Enable blending and texturing.
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture);

                glBegin(GL_QUADS);
                // Adjust the texture coordinates if needed to flip vertically.
                glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, ypos + h);
                glTexCoord2f(1.0f, 0.0f); glVertex2f(w, ypos + h);
                glTexCoord2f(1.0f, 1.0f); glVertex2f(w, ypos);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, ypos);
                glEnd();

                // Advance horizontally by the glyph's advance value.
                glTranslatef((face->glyph->advance.x >> 6), 0.0f, 0.0f);

                glDisable(GL_TEXTURE_2D);
                glDisable(GL_BLEND);
            }
            // Restore the matrix for this line.
            glPopMatrix();
        }
        // Restore the original transformation matrix.
        glPopMatrix();
    }


