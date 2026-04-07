#ifndef CAMERA_H
#define CAMERA_H

/* o sistema de camera tem tres modos distintos:

   CAM_OVERVIEW -> Visao Geral
    - camera orbitando o Sol num angulo livre
    - o usuario pode girar (yaw/pitch) e dar zoom
    - modo padrao ao iniciar o programa

   CAM_SUN -> POV Sol
    - camera posicionada no centro do Sol, olhando para fora
    - o usuario pode girar livremente em 360 graus
    - util para ver todos os planetas ao redor
*/

/* 
   Representa o estado de uma camera esferica:
    - yaw = rotacao horizontal (graus)
    - pitch = rotacao vertical (graus)
    - dist = distancia ao alvo (unidades de cena)
*/
typedef struct {
    float yaw;
    float pitch;
    float dist;
} CameraState;


//identifica qual dos modos esta ativo
typedef enum {
    CAM_OVERVIEW = 0, // orbita livre ao redor do Sol    
    CAM_SUN      = 1, // POV a partir do centro do Sol
    CAM_ORBIT    = 2, // POV orbitando um planeta selecionado
    CAM_COUNT    = 3 // CAM_COUNT dimensiona o array camStates[]
} CameraMode;

// Variaveis globais de camera -> ja definidas em camera.c
extern CameraState camStates[CAM_COUNT]; // estado de cada modo de camera   
extern CameraMode  cameraMode; // modo atualmente ativo

// vars do modo CAM_ORBIT
extern int   orbitTarget;
extern float orbitWalkAngle;
extern float orbitViewDist;
extern float orbitViewPitch; 
          
//initCameraStates() -> Define os valores iniciais de cada CameraState -> Deve ser chamada uma vez em init()
void initCameraStates(void);

/* Aplica a transformacao de visao (gluLookAt) de acordo com o modo atual (cameraMode)
- Deve ser chamada no inicio de display(), apos glLoadIdentity()
- Retorna uma string com o nome do modo atual para exibicao no HUD (ex: "Visao Geral", "Sol", "Orbita")
*/
const char *setupCamera(void);


//Muda para o modo CAM_SUN, orientando a camera para apontar inicialmente em direcao a Terra.
void enterSunPOV(void);

// muda pro modo CAM_ORBIT, com o planeta especificado como alvo
void enterOrbit(int planetIdx);

#endif /* CAMERA_H */
