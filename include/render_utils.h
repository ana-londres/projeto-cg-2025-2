#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include <GL/glut.h>

// config material pro pipeline de iluminacao
void setMaterial(float r, float g, float b, float shininess);

// define emissão do obj que brilha por conta própria
void setEmission(float r, float g, float b);

// evita que a emissao de um obj vaze para os proximos objs desenhados
void clearEmission(void);

// desenha uma esfera texturizada usando GLUquadric
void drawTexturedSphere(float radius, int slices, int stacks, GLuint tex);

// desenha uma linha circular no plano xz para representar a orbita
void drawOrbit(float radius);

// desenha um fundo de estrelas usando uma esfera gigante texturizada
void drawStars(GLuint tex);

// flag global para mostrar ou ocultar as orbitas
extern int showOrbits;

#endif