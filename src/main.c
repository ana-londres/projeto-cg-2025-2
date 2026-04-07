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

#include "obj_loader.h"
#include "textures.h"
#include "planets.h"
#include "render_utils.h"
#include "camera.h"
#include "comet.h"
#include "station.h"
#include "hud.h"

// variaveis de controle da simulacao - precisam ser globais -> porque varios callbacks do GLUT precisam acessar
float animSpeed    = 1.0f;
int   animate      = 1;
int   windowWidth  = 1200;
int   windowHeight = 800;


// usado pra animar o sol (efeito pulsação) -> apenas pra ficar bonito visualmente
float sunPulse = 0.0f;

// ultima posição registrada do mouse (drag)
static int lastMouseX = -1;
static int lastMouseY = -1;

// indica se o botão esquerdo está pressionado
static int mouseDown  =  0;

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
    glEnable(GL_LIGHTING);// ativa o sistema de iluminacao do OpenGL (precisa disso pra luzes funcionarem)
    glEnable(GL_NORMALIZE);  // necessario quando usa glScalef com luzes 
    glShadeModel(GL_SMOOTH); // suaviza a transicao entre as faces iluminadas e nao iluminadas dos planetas
    initCameraStates();

    // inicializa modulos do sistema
    setupLights();
    initPlanets();
    initAsteroids();
    setupTextures();
    initCameraStates();

    // tenta carregar modelo externo (.obj)
    obj_loaded = loadOBJ("assets/InternationalSpaceStation.obj");
}

static void drawSun(void)
{
    // evita desenhar se a câmera estiver dentro dele
    if (cameraMode == CAM_SUN) return;

    // calcula pequena variação (pulsação)
    float pulse = 1.0f + 0.03f * sinf(sunPulse);

    glPushMatrix();

        // aplica escala variável
        glScalef(pulse, pulse, pulse);

        // material do sol
        setMaterial(1.0f, 0.85f, 0.20f, 150.0f);

        // emissão -> faz brilhar sem depender de luz externa
        float em = 0.85f + 0.15f * sinf(sunPulse);
        setEmission(em, em * 0.48f, 0.0f);

        // esfera com textura
        drawTexturedSphere(SUN_RADIUS, 48, 48, sunTexture);

        clearEmission();

    glPopMatrix();

    // halo externo (efeito glow)
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    // blend aditivo -> mais brilho
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(1.0f, 0.65f, 0.10f, 0.08f + 0.04f * sinf(sunPulse));

    // esfera maior para simular corona
    glutSolidSphere(2.5f * pulse, 32, 32);

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void display(void)
{
    // limpa tela e buffer de profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    const char *camName = setupCamera();// aplica câmera atual
    
    drawStars(starsTexture);
    
    // luz precisa ser reposicionada depois da câmera, senao ela "anda" junto com a camera
    GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // desenha orbitas (se ativado)
    if (showOrbits) {
        for (int i = 0; i < NUM_PLANETS; i++)
            drawOrbit(planets[i].orbitRadius);
        drawStationOrbit();
    }

    drawAsteroids();
    drawSun();

    // desenha todos os planetas
    for (int i = 0; i < NUM_PLANETS; i++)
        drawPlanet(i);

    drawSpaceStation();
    drawBezierCurve();
    drawComet();

    // desenha interface na tela
    drawHUD(windowWidth, windowHeight, camName);

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

/* ============================================================
   TIMER - loop de animacao
   ============================================================ */

void timer(int value)
{
    (void)value; // parametro exigido pelo GLUT, mas aqui nao preciso usar

    if (animate)
    {
        float dt = animSpeed; // tudo usa o mesmo fator de velocidade

        // atualiza orbita e rotacao propria de cada planeta
        for (int i = 0; i < NUM_PLANETS; i++) {
            planets[i].orbitAngle += planets[i].orbitSpeed * dt;
            if (planets[i].orbitAngle >= 360.0f) planets[i].orbitAngle -= 360.0f;

            planets[i].selfAngle += planets[i].rotationSpeed * dt;
            if (planets[i].selfAngle >= 360.0f) planets[i].selfAngle -= 360.0f;
        }

        // lua da terra -> orbita e rotacao separadas dos planetas
        moonOrbitAngle += 4.5f * dt;
        if (moonOrbitAngle >= 360.0f) moonOrbitAngle -= 360.0f;

        moonSelfAngle += 3.0f * dt;
        if (moonSelfAngle >= 360.0f) moonSelfAngle -= 360.0f;

        // estacao espacial -> gira ao redor da terra e em torno dela mesma
        shipOrbitAngle += 3.2f * dt;
        if (shipOrbitAngle >= 360.0f) shipOrbitAngle -= 360.0f;

        shipSelfAngle += 2.0f * dt;
        if (shipSelfAngle >= 360.0f) shipSelfAngle -= 360.0f;

        /*
            cometT vai de 0 ate 1 ao longo da curva de bezier
            quando chega no fim, volta pro inicio
        */
        cometT += 0.0015f * dt;
        if (cometT > 1.0f) cometT = 0.0f;

        // usado no sinf() pra pulsacao visual do sol
        sunPulse += 0.04f * dt;

        // atualiza a posicao angular dos asteroides
        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            asteroids[i].orbitAngle += asteroids[i].orbitSpeed * dt;
            if (asteroids[i].orbitAngle >= 360.0f) asteroids[i].orbitAngle -= 360.0f;
        }
    }

    glutPostRedisplay(); // pede novo desenho da tela
    glutTimerFunc(16, timer, 0); // agenda proximo frame (~60 fps)
}

void keyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y; // GLUT manda essas posicoes, mas nao preciso aqui

    switch (key)
    {
        case 27:
            exit(0); // ESC fecha o programa
            break;

        case ' ':
            animate = !animate; // pausa/retoma animacao
            break;

        case '+':
        case '=':
            animSpeed += 0.2f; // acelera a simulacao
            break;

        case '-':
            if (animSpeed > 0.21f) animSpeed -= 0.2f; // evita velocidade zerar
            break;

        case 'o':
        case 'O':
            showOrbits = !showOrbits; // mostra/esconde orbitas
            break;

        case 'h':
        case 'H':
            showLegend = !showLegend; // mostra/esconde legenda
            break;

        // 1 a 8 -> entra no modo orbital do planeta escolhido
        case '1':
            enterOrbit(MERCURY);
            break;
        case '2':
            enterOrbit(VENUS);
            break;
        case '3':
            enterOrbit(EARTH);
            break;
        case '4':
            enterOrbit(MARS);
            break;
        case '5':
            enterOrbit(JUPITER);
            break;
        case '6':
            enterOrbit(SATURN);
            break;
        case '7':
            enterOrbit(URANUS);
            break;
        case '8':
            enterOrbit(NEPTUNE);
            break;

        /*
            W/S fazem zoom, mas o comportamento muda
            dependendo do modo de camera
        */
        case 'w':
        case 'W':
            if (cameraMode == CAM_ORBIT) {
                orbitViewDist -= 0.3f; // aproxima do planeta-alvo
                if (orbitViewDist < 0.3f) orbitViewDist = 0.3f;
            } else if (cameraMode == CAM_OVERVIEW) {
                camStates[CAM_OVERVIEW].dist -= 1.5f; // aproxima do centro
                if (camStates[CAM_OVERVIEW].dist < 8.0f)
                    camStates[CAM_OVERVIEW].dist = 8.0f;
            }
            break;

        case 's':
        case 'S':
            if (cameraMode == CAM_ORBIT) {
                orbitViewDist += 0.3f; // afasta do planeta-alvo
            } else if (cameraMode == CAM_OVERVIEW) {
                camStates[CAM_OVERVIEW].dist += 1.5f; // afasta da cena
                if (camStates[CAM_OVERVIEW].dist > 80.0f)
                    camStates[CAM_OVERVIEW].dist = 80.0f;
            }
            break;

        /*
            A/D giram horizontalmente
            no modo orbital elas andam ao redor do anel
        */
        case 'a':
        case 'A':
            if (cameraMode == CAM_ORBIT)
                orbitWalkAngle -= 3.0f;
            else
                camStates[cameraMode].yaw -= 4.0f;
            break;

        case 'd':
        case 'D':
            if (cameraMode == CAM_ORBIT)
                orbitWalkAngle += 3.0f;
            else
                camStates[cameraMode].yaw += 4.0f;
            break;

        /*
            Q/E controlam o angulo vertical da camera
            uso limite pra camera nao inverter
        */
        case 'q':
        case 'Q':
            if (cameraMode == CAM_ORBIT) {
                orbitViewPitch += 2.0f;
                if (orbitViewPitch > 80.0f) orbitViewPitch = 80.0f;
            } else {
                CameraState *cs = &camStates[cameraMode];
                cs->pitch += 2.5f;

                float pMax = (cameraMode == CAM_OVERVIEW) ? 89.0f : 85.0f;
                if (cs->pitch > pMax) cs->pitch = pMax;
            }
            break;

        case 'e':
        case 'E':
            if (cameraMode == CAM_ORBIT) {
                orbitViewPitch -= 2.0f;
                if (orbitViewPitch < -80.0f) orbitViewPitch = -80.0f;
            } else {
                CameraState *cs = &camStates[cameraMode];
                cs->pitch -= 2.5f;

                float pMin = (cameraMode == CAM_OVERVIEW) ? 2.0f : -85.0f;
                if (cs->pitch < pMin) cs->pitch = pMin;
            }
            break;

        // volta pra visao geral
        case 'v':
        case 'V':
        case '9':
            cameraMode = CAM_OVERVIEW;
            printf("[CAM] Visao Geral\n");
            break;

        // entra no POV do Sol
        case '0':
            enterSunPOV();
            break;

        default:
            break;
    }

    glutPostRedisplay(); // atualiza a tela depois do input
}


void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        mouseDown = (state == GLUT_DOWN); // 1 se apertou, 0 se soltou
        lastMouseX = x;
        lastMouseY = y;
    }

    /*
        no GLUT, scroll geralmente chega como botao 3 e 4
        aqui deixei so pra visao geral
    */
    if (cameraMode == CAM_OVERVIEW) {
        CameraState *cs = &camStates[CAM_OVERVIEW];

        if (button == 3) { // scroll pra cima -> aproxima
            cs->dist -= 1.5f;
            if (cs->dist < 8.0f) cs->dist = 8.0f;
            glutPostRedisplay();
        }

        if (button == 4) { // scroll pra baixo -> afasta
            cs->dist += 1.5f;
            if (cs->dist > 80.0f) cs->dist = 80.0f;
            glutPostRedisplay();
        }
    }
}

void mouseMotion(int x, int y)
{
    if (!mouseDown) return; // so gira camera durante arrasto

    int dx = x - lastMouseX;
    int dy = y - lastMouseY;

    CameraState *cs = &camStates[cameraMode];

    // visao geral um pouco mais suave
    float sensitivity = (cameraMode == CAM_OVERVIEW) ? 0.30f : 0.35f;

    cs->yaw += dx * sensitivity;
    cs->pitch -= dy * sensitivity; // inverti porque no GLUT o y cresce pra baixo

    // limita pitch pra camera nao virar de cabeca pra baixo
    float pMax = (cameraMode == CAM_OVERVIEW) ? 89.0f : 85.0f;
    float pMin = (cameraMode == CAM_OVERVIEW) ? 2.0f  : -85.0f;

    if (cs->pitch > pMax) cs->pitch = pMax;
    if (cs->pitch < pMin) cs->pitch = pMin;

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay(); // redesenha com a nova orientacao
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//  // double buffer evita flicker, RGB define cor, DEPTH ativa z-buffer
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(80, 40);
    glutCreateWindow("Sistema Solar 3D - Projeto Final ICG 2025.2");

    init(); // configura OpenGL e estados iniciais

    // registra funcoes que o GLUT vai chamar
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(16, timer, 0);
    
    glutMainLoop();// loop principal da aplicacao
    return 0;
}
