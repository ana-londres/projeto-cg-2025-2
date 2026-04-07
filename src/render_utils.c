// implementa renderizacao reutilizavel: materiais, esfera texturizada, orbita e fundo de estrelas

#include "render_utils.h"
#include "planets.h"

#include <GL/glut.h>
#include <math.h>

int showOrbits = 1; // 1 exibe orbitas, 0 oculta

// config ambient, diffuse, specular e shininess pro pipeline de iluminacao de Phong
void setMaterial(float r, float g, float b, float shininess)
{
    GLfloat ambient[]  = { r * 0.2f, g * 0.2f, b * 0.2f, 1.0f }; // luz fraca que sempre aparece 
    GLfloat diffuse[]  = { r, g, b, 1.0f }; // cor principal do obj que depende da luz incidente
    GLfloat specular[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // brilho sempre claro simulando reflexo

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf (GL_FRONT, GL_SHININESS, shininess); // foco do brilho -> baixo = espalhado / alto = concentrado
}

// define a componente emissiva (luz própria) do componente atual
void setEmission(float r, float g, float b)
{
    GLfloat emission[] = { r, g, b, 1.0f }; // cor de emissão
    glMaterialfv(GL_FRONT, GL_EMISSION, emission); // faz o obj parecer que emite luz
}

void clearEmission(void)
{
    GLfloat emission[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // zera emissão 
    glMaterialfv(GL_FRONT, GL_EMISSION, emission); // remove o efeito pra não vazar pro próx obj
}

void drawTexturedSphere(float radius, int slices, int stacks, GLuint tex)
{
    // desenha os objs
    GLUquadric *q = gluNewQuadric();
    gluQuadricDrawStyle(q, GLU_FILL);
    gluQuadricNormals(q, GLU_SMOOTH); // melhora iluminação
    gluQuadricTexture(q, GL_TRUE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    gluSphere(q, radius, slices, stacks);
    glDisable(GL_TEXTURE_2D);

    gluDeleteQuadric(q); // libera a memória
}

void drawOrbit(float radius)
{
    if (!showOrbits) return; // só desenha se estiver ativado

    glDisable(GL_LIGHTING);
    glLineStipple(1, 0xAAAA); // padrao de pontilhado -> alternado
    glEnable(GL_LINE_STIPPLE);
    glColor4f(0.6f, 0.6f, 0.6f, 0.5f); // cor cinza bem claro

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 256; i++) { 
        float a = 2.0f * PI * i / 256.0f; // angulo para distribuir 256 pontos uniformemente 
        glVertex3f(radius * cosf(a), 0.0f, radius * sinf(a)); // ponto no plano XZ formando o círculo
    }
    glEnd();

    glDisable(GL_LINE_STIPPLE);
    glEnable(GL_LIGHTING);
}

void drawStars(GLuint tex)
{
    glDisable(GL_LIGHTING); // fundo não recebe luz
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLUquadric *q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    glPushMatrix();
    glScalef(-1.0f, 1.0f, 1.0f); // INVERTE a esfera -> fica dentro dela
    gluSphere(q, 100.0f, 50, 50); // esfera gigante
    glPopMatrix();

    gluDeleteQuadric(q);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}