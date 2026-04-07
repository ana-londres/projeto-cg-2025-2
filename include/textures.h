#ifndef TEXTURES_H
#define TEXTURES_H

#include <GL/glut.h>

// ids de textura dos objetos especiais -> ficam dentro da struct Planet em planets.h
extern GLuint moonTexture;
extern GLuint sunTexture;
extern GLuint saturnRingTexture;
extern GLuint stationTexture;
extern GLuint starsTexture;

// carrega um arquivo png e gera um id de textura opengl com mipmaps -> versões reduzidas da mesma textura
void loadPNGTexture(GLuint *texID, const char *path);

// carrega todas as texturas de uma vez
void setupTextures(void);

#endif