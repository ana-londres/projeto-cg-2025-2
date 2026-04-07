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

/*
    LIGHT0 representa o sol-> fica na origem (0,0,0) com atenuacao quadratica para que planetas mais distantes recebam menos luz, como acontece de verdade 
    Cor -> amarelo-quente (1.0, 0.95, 0.80)
    w=1 na posicao indica luz pontual (nao direcional)
*/

/*  LIGHT1 e uma luz ambiente fraca azulada, sem componente diffuse, so para garantir que o lado escuro dos planetas nao fique completamente preto e consiga ser visualizado na simulacao
    w=0 na posicao indica luz direcional (raios paralelos).
 */
static void setupLights(void)
{
    glEnable(GL_LIGHT0);
    GLfloat l0_ambient[]  = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat l0_diffuse[]  = { 1.0f,  0.95f, 0.80f, 1.0f };
    GLfloat l0_specular[] = { 1.0f,  1.0f,  1.0f,  1.0f };
    GLfloat l0_position[] = { 0.0f,  0.0f,  0.0f,  1.0f }; // w=1 -> luz pontual 

    glLightfv(GL_LIGHT0, GL_AMBIENT,  l0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  l0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, l0_position);

    /* esses valores foram ajustados na mao pra ficar visualmente bom -> a atenuacao quadratica eh a mais realista das tres ->intensidade cai com o quadrado da distancia, igual a fisica */
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.008f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0002f);

    glEnable(GL_LIGHT1);
    GLfloat l1_ambient[]  = { 0.08f, 0.08f, 0.12f, 1.0f };
    GLfloat l1_diffuse[]  = { 0.0f,  0.0f,  0.0f,  1.0f }; // sem diffuse intencional 
    GLfloat l1_position[] = { 0.0f, 50.0f,  0.0f,  0.0f }; // w=0 = direcional       

    glLightfv(GL_LIGHT1, GL_AMBIENT,  l1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  l1_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, l1_position);
}

void init(void)
{
    // fundo quase preto com um pouquinho de azul pra parecer espaco 
    glClearColor(0.01f, 0.01f, 0.06f, 1.0f);

    glEnable(GL_DEPTH_TEST); // evita que obejtos de tras fiquem muito pra frente 
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);  // necessario quando usa glScalef com luzes 
    glShadeModel(GL_SMOOTH);
    initCameraStates();

     setupLights();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    const char *camName = setupCamera();

    // reposiciona a luz 0 na origem (Sol) apos setupCamera() 
    GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

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
