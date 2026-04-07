/* 
   implementacao dos tres modos de camera do Sistema Solar.
   todos os angulos armazenados em graus; convertidos para
   radianos so no momento do calculo de posicao/direcao
*/

#include "camera.h"
#include "planets.h" 

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

//variaveis globais (estado da camera)
CameraState camStates[CAM_COUNT];
CameraMode  cameraMode = CAM_OVERVIEW;

void initCameraStates(void)
{
    // visao geral-> acima e levemente afastado do sol 
    camStates[CAM_OVERVIEW] = (CameraState){ 30.0f, 35.0f, 42.0f };

    // POV sol -> camera na origem, olhando para frente 
    camStates[CAM_SUN]      = (CameraState){  0.0f,  0.0f,  2.5f };

}

/* 
   camera esferica ao redor da origem (Sol).
   O usuario orbita livremente com mouse.
   pitch: limitado entre 2 e 89 graus (nunca rasa nem invertida)
*/
static const char *setupCameraOverview(void)
{
    CameraState *cs = &camStates[CAM_OVERVIEW];

    // clampeia pitch para evitar gimbal lock -> dois ou três eixos de rotação se alinham 
    if (cs->pitch > 89.0f) cs->pitch = 89.0f;
    if (cs->pitch <  2.0f) cs->pitch =  2.0f;

    float yr = cs->yaw   * PI / 180.0f;
    float pr = cs->pitch * PI / 180.0f;

    /* converte coordenadas esfericas para cartesianas ->
       o olho se move numa casca esferica de raio dist ao redor
       do sol; yaw gira no plano horizontal, pitch sobe/desce */
    float ex = cs->dist * cosf(pr) * sinf(yr);
    float ey = cs->dist * sinf(pr);
    float ez = cs->dist * cosf(pr) * cosf(yr);

    gluLookAt(ex, ey, ez,  // posicao do olho                  
              0,  0,  0,   // ponto alvo: centro do sol        
              0,  1,  0);  // vetor "cima": eixo Y             
    return "Visao Geral";
}

/*
   camera posicionada no centro do Sol, olhando para fora.
   permite ao usuario girar 360 graus e ver todos os planetas.
   ao ativar este modo, a camera e pre-orientada para a terra
*/
static const char *setupCameraSun(void)
{
    CameraState *cs = &camStates[CAM_SUN];

    float yr = cs->yaw   * PI / 180.0f;
    float pr = cs->pitch * PI / 180.0f;

    // camera fica em (0, dist, 0), ou seja, dentro do sol 
    float ex = 0.0f, ey = cs->dist, ez = 0.0f;

    // direcao de visao calculada a partir de yaw e pitch 
    float dx = cosf(pr) * sinf(yr);
    float dy = sinf(pr);
    float dz = cosf(pr) * cosf(yr);

    /* vetor cima muda quando olhamos quase verticalmente:
       proximo de +-90 graus o eixo Y deixa de ser "cima"
       e precisamos usar Z invertido para nao tremer a camera */
    float ux = 0.0f, uy = 1.0f, uz = 0.0f;
    if (fabsf(pr) > 0.95f * (PI / 2.0f)) {
        ux = 0.0f;
        uy = 0.0f;
        uz = (pr > 0.0f) ? -1.0f : 1.0f;
    }

    gluLookAt(ex,    ey,    ez,
              ex+dx, ey+dy, ez+dz,
              ux,    uy,    uz);
    return "Sol";
}


//despacha para o sub-handler correto conforme cameraMode
const char *setupCamera(void)
{
    if (cameraMode == CAM_OVERVIEW) return setupCameraOverview();
    return setupCameraSun();
}
