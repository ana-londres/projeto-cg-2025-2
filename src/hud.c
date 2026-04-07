/*Salva as matrizes GL_PROJECTION e GL_MODELVIEW
     2. Configura projecao ortografica (pixels exatos)
     3. Desativa iluminacao e depth test
     4. Desenha texto com GLUT bitmap fonts
     5. Restaura matrizes e estados anteriores*/

#include "hud.h"
#include "camera.h"  
#include "planets.h" 
#include "obj_loader.h" 
#include <GL/glut.h>
#include <stdio.h>

int showLegend = 1;

extern float animSpeed;
extern int   animate;

// desenha texto simples na tela usando GLUT bitmap
static void drawText(float x, float y, const char *str)
{
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *str++);
    }
}

void drawHUD(int w, int h, const char *camName)
{
    if (!showLegend) return;

    //salva estados atuais
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h); //muda para modo 2D (origem no canto inferior esquerdo)

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor3f(0.85f, 0.85f, 0.85f);
    drawText(10, h - 20, "SISTEMA SOLAR 3D");

    //velocidade e estado
    char buf[256];
    snprintf(buf, sizeof(buf), "Velocidade: %.1fx  %s",
             animSpeed, animate ? "" : "[PAUSADO]");
    glColor3f(0.75f, 0.95f, 0.75f);
    drawText(10, h - 38, buf);

    //status da estacao espacial
    extern int obj_loaded;
    if (obj_loaded) {
        glColor3f(0.5f, 0.85f, 1.0f);
        drawText(10, h - 56, "Estacao Espacial: orbitando a Terra [OBJ]");
    } else {
        glColor3f(1.0f, 0.5f, 0.4f);
        drawText(10, h - 56, "Estacao Espacial: OBJ nao encontrado");
    }

    //modo de camera
    snprintf(buf, sizeof(buf), "Camera: %s", camName);
    glColor3f(1.0f, 0.90f, 0.20f);
    drawText(10, h - 74, buf);

    //instrucao (muda conforme o modo de camera)
    if (cameraMode == CAM_OVERVIEW) {
        glColor3f(0.80f, 0.70f, 1.0f);
        drawText(10, h - 92, "Visao Geral: drag = girar  |  scroll / W/S = zoom");
    }
    else if (cameraMode == CAM_ORBIT && orbitTarget >= 0) {
        snprintf(buf, sizeof(buf),
                 "Planeta: %s  |  A/D = caminhar  |  W/S = zoom  |  Q/E = altura",
                 planets[orbitTarget].name);
        glColor3f(0.4f, 1.0f, 0.6f);
        drawText(10, h - 92, buf);
    }
    else { /* CAM_SUN */
        glColor3f(1.0f, 0.85f, 0.30f);
        drawText(10, h - 92, "POV Sol: voce E o Sol - gire e veja os planetas ao redor");
    }

    //teclas disponiveis
    glColor3f(0.65f, 0.65f, 0.65f);
    drawText(10, 60, "V/9 = Visao Geral  |  0 = POV Sol  |  1-8 = Orbita (pausa primeiro)");
    drawText(10, 44, "A/D = girar  |  Q/E = cima/baixo  |  Mouse drag = olhar 360");
    drawText(10, 28, "W/S = zoom  |  +/- = velocidade  |  SPACE = pausar");
    drawText(10, 12, "O = orbitas  |  H = legenda  |  ESC = sair");

    //restaura estados
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}