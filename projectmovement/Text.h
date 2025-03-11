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
std::map<FT_ULong, GLuint> glyphTextureCache;
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

    if (FT_New_Face(ft, "..\\Fonts\\Jaywalk.ttf", 0, &face)) {
        std::cerr << "Error loading font!" << std::endl;
        return false;
    }

    return true;
}


GLuint loadCharacterTexture(FT_ULong charCode) {
    if (FT_Load_Char(face, charCode, FT_LOAD_RENDER)) {
        std::cerr << "Error loading character!" << std::endl;
        return 0;
    }

    FT_GlyphSlot g = face->glyph;
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    unsigned int width = g->bitmap.width;
    unsigned int height = g->bitmap.rows;
    unsigned int pitch = g->bitmap.pitch;


    unsigned char* expandedData = new unsigned char[width * height * 4];


    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {

            unsigned int indexSrc = row * pitch + col;

            unsigned int indexDst = row * width + col;


            expandedData[4 * indexDst] = 255;                     // Red
            expandedData[4 * indexDst + 1] = 255;                     // Green
            expandedData[4 * indexDst + 2] = 255;                     // Blue
            expandedData[4 * indexDst + 3] = g->bitmap.buffer[indexSrc]; // Alpha
        }
    }


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, expandedData);


    delete[] expandedData;


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return texture;
}




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

        x += g->advance.x >> 6;


        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDeleteTextures(1, &texture);

    }
    glPopAttrib();
    glPopMatrix();
}
float computeTextWidth(const std::string& text) {
    float width = 0.0f;
    for (char c : text) {

        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Error loading character '" << c << "'" << std::endl;
            continue;
        }

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

            float ypos = -int(g->bitmap.rows - g->bitmap_top);
            float w = g->bitmap.width;
            float h = g->bitmap.rows;

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);

            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, ypos + h);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(w, ypos + h);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(w, ypos);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, ypos);
            glEnd();

            glTranslatef((face->glyph->advance.x >> 6), 0.0f, 0.0f);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
            glDeleteTextures(1, &texture);

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


        for (size_t i = 0; i < lines.size(); i++) {

            float lineWidth = computeTextWidth(lines[i]);

            float startX = centerX - lineWidth / 2.0f;

            glPushMatrix();
            glTranslatef(startX, y - i * newlinedistance, 0.0f);


            for (char c : lines[i]) {
                GLuint texture = loadCharacterTexture(c);
                if (texture == 0)
                    continue;

                FT_GlyphSlot g = face->glyph;

                float ypos = -int(g->bitmap.rows - g->bitmap_top);
                float w = g->bitmap.width;
                float h = g->bitmap.rows;


                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture);

                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, ypos + h);
                glTexCoord2f(1.0f, 0.0f); glVertex2f(w, ypos + h);
                glTexCoord2f(1.0f, 1.0f); glVertex2f(w, ypos);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, ypos);
                glEnd();

                glTranslatef((face->glyph->advance.x >> 6), 0.0f, 0.0f);

                glDisable(GL_TEXTURE_2D);
                glDisable(GL_BLEND);
                glDeleteTextures(1, &texture);

            }
            glPopMatrix();
        }
        glPopMatrix();
    }


