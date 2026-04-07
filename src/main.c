/*
    "esqueleto" principal: inicializa o OpenGL,
    registra os callbacks do GLUT e cuida do loop de animacao.
*/
/*
    A logica de cada coisa (planetas, camera, cometa...) ficou
    separada em arquivos proprios pra nao virar uma bagunca.
*/

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
// variaveis de controle da simulacao - precisam ser globais -> porque varios callbacks do GLUT precisam acessar
float animSpeed    = 1.0f;
int   animate      = 1;
int   windowWidth  = 1200;
int   windowHeight = 800;

void init(void)
{
    // fundo quase preto com um pouquinho de azul pra parecer espaco 
    glClearColor(0.01f, 0.01f, 0.06f, 1.0f);

    glEnable(GL_DEPTH_TEST); // evita que obejtos de tras fiquem muito pra frente 
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);  // necessario quando usa glScalef com luzes 
    glShadeModel(GL_SMOOTH);
    initCameraStates();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    const char *camName = setupCamera();

    glutSwapBuffers(); // double buffer -> troca os buffers pra nao piscar 
}

void reshape(int w, int h)
{
    if (h == 0) h = 1; // evita divisao por zero

    windowWidth  = w;
    windowHeight = h;

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // near bem pequeno (0.001) pra nao cortar o Sol quando a camera entra no modo POV
    //far 300 cobre as estrelas 
    gluPerspective(55.0, (double)w / h, 0.001, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(80, 40);
    glutCreateWindow("Sistema Solar 3D - Computacao Grafica 2025");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
