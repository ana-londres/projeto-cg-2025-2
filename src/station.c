// renderizacao da Estacao Espacial em orbita ao redor da Terra

#include "station.h"
#include "obj_loader.h"
#include "planets.h"  
#include "render_utils.h"
#include "textures.h"

#include <GL/glut.h>
#include <math.h>

// var globais de animação
float shipOrbitAngle = 0.0f; // posicao na orbita em torno da Terra
float shipSelfAngle  = 0.0f; // rotacao propria do modelo

// desenha a estacao espacial em sua posicao atual
void drawSpaceStation(void)
{
    if (!obj_loaded) return;

    Planet *earth = &planets[EARTH]; // pega a Terra do array -> pra seguir sua órbita

    glPushMatrix();
        // segue a Terra na orbita
        glRotatef(earth->orbitAngle, 0, 1, 0);
        glTranslatef(earth->orbitRadius, 0, 0);

        // orbita propria em torno da Terra
        glRotatef(shipOrbitAngle, 0, 1, 0);
        glTranslatef(1.0f, 0.25f, 0); // altitude um pouco acima da Terra

        glRotatef(90.0f, 0, 1, 0); // corrige orientação do obj
        glRotatef(shipSelfAngle * 0.3f, 1, 0, 0); // rotação própria suave

        // escala o modelo obj
        glScalef(0.01f, 0.01f, 0.01f);

        setMaterial(1.0f, 1.0f, 1.0f, 80.0f); // material branco brilhante -> "metal"
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, stationTexture);
        drawOBJModel();
        glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// desenha a linha de orbita da estacao em torno da Terra
void drawStationOrbit(void)
{
    if (!showOrbits) return;

    Planet *earth = &planets[EARTH];

    glPushMatrix();
        // posiciona o anel de orbita junto com a Terra
        glRotatef(earth->orbitAngle, 0, 1, 0);
        glTranslatef(earth->orbitRadius, 0.25f, 0);

        glDisable(GL_LIGHTING);
        glLineStipple(1, 0x8888); // pontilhado esparso
        glEnable(GL_LINE_STIPPLE);
        glColor4f(0.4f, 0.8f, 1.0f, 0.4f); // cor ciano transparente

        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 128; i++) {
            float a = 2.0f * PI * i / 128.0f; // ângulo para distribuir 128 pontos uniformemente
            glVertex3f(cosf(a), 0.0f, sinf(a)); // raio = 1.0 em torno da Terra
        }
        glEnd();

        glDisable(GL_LINE_STIPPLE);
        glEnable(GL_LIGHTING);
    glPopMatrix();
}