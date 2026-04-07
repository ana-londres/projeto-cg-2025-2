#include "hud.h"

#include <GL/glut.h>

//desenha texto simples na tela usando GLUT bitmap
void drawText(float x, float y, const char *text)
{
    glRasterPos2f(x, y); //define onde o texto começa

    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text); //desenha carctere por caractere
        text++;
    }
}

//desenha o HUD completo na tela
void drawHUD(int width, int height)
{
    // salva estados atuais
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // muda para modo 2D (origem no canto inferior esquerdo)
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING); // desliga iluminação pra não afetar o texto

    glColor3f(1.0f, 1.0f, 1.0f);

    // textos na tela 
    drawText(10, height - 20, "Sistema Solar 3D");
    drawText(10, height - 40, "Controles:");
    drawText(10, height - 60, "WASD - mover camera");
    drawText(10, height - 80, "Mouse - olhar ao redor");
    drawText(10, height - 100, "ESC - sair");

    // restaura estados
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}