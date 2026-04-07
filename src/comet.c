#include "comet.h"
#include "planets.h"

#include <GL/glut.h>
#include <math.h>

float bezierCtrl[4][3] = {
    { -22.0f,  6.0f, -14.0f },
    {  -8.0f, 14.0f,  15.0f },
    {  14.0f, -9.0f, -10.0f },
    {  24.0f,  7.0f,  12.0f }
};

// progresso do cometa ao longo da curva [0..1]
float cometT = 0.0f;

//calcula o ponto da curva de Bézier cúbica usando a fórmula de Bernstein - diz onde está
void bezierPoint(float t, float *x, float *y, float *z)
{
    float it = 1.0f - t;

    float b0 = it * it * it;
    float b1 = 3.0f * t * it * it;
    float b2 = 3.0f * t * t * it;
    float b3 = t * t * t; // a fórmula mistura os 4 pontos de controle com pesos que dependem de t

    *x = b0 * bezierCtrl[0][0] + b1 * bezierCtrl[1][0] + b2 * bezierCtrl[2][0] + b3 * bezierCtrl[3][0];
    *y = b0 * bezierCtrl[0][1] + b1 * bezierCtrl[1][1] + b2 * bezierCtrl[2][1] + b3 * bezierCtrl[3][1];
    *z = b0 * bezierCtrl[0][2] + b1 * bezierCtrl[1][2] + b2 * bezierCtrl[2][2] + b3 * bezierCtrl[3][2]; 
}

// calcula a derivada da curva, ou seja, a direção instantânea do movimento - diz para onde está indo
void bezierTangent(float t, float *dx, float *dy, float *dz)
{
    float it = 1.0f - t;

    float d0 = -3.0f * it * it;
    float d1 =  3.0f * it * it - 6.0f * t * it;
    float d2 =  6.0f * t * it - 3.0f * t * t;
    float d3 =  3.0f * t * t;

    *dx = d0 * bezierCtrl[0][0] + d1 * bezierCtrl[1][0] + d2 * bezierCtrl[2][0] + d3 * bezierCtrl[3][0];
    *dy = d0 * bezierCtrl[0][1] + d1 * bezierCtrl[1][1] + d2 * bezierCtrl[2][1] + d3 * bezierCtrl[3][1];
    *dz = d0 * bezierCtrl[0][2] + d1 * bezierCtrl[1][2] + d2 * bezierCtrl[2][2] + d3 * bezierCtrl[3][2];
}

// desenha a curva de Bézier como guia visual da trajetória como uma linha pontilhada
void drawBezierCurve(void)
{
    glDisable(GL_LIGHTING);

    glLineStipple(2, 0xF0F0);
    glEnable(GL_LINE_STIPPLE); //ativa linha pontilhada
    glLineWidth(1.5f);
    glColor4f(0.8f, 0.8f, 0.3f, 0.6f);

    glBegin(GL_LINE_STRIP); //liga os pontos
    for (int i = 0; i <= 300; i++) {
        float t = (float)i / 300.0f;
        float x, y, z;
        bezierPoint(t, &x, &y, &z); //para cada valor de t, calcula um ponto com bezierPoint
        glVertex3f(x, y, z);
    }
    glEnd();

    glLineWidth(1.0f);
    glDisable(GL_LINE_STIPPLE);
    glEnable(GL_LIGHTING);
}

// desenha o cometa: núcleo, halo e cauda
void drawComet(void)
{
    float x, y, z, dx, dy, dz;

    //pega posição e direção no ponto atual da curva
    bezierPoint(cometT, &x, &y, &z);
    bezierTangent(cometT, &dx, &dy, &dz);

    //normaliza a tangente, a direção tem tamanho 1. Para usar só a direção, sem distorcer por comp
    float len = sqrtf(dx * dx + dy * dy + dz * dz);
    if (len > 0.001f) {
        dx /= len;
        dy /= len;
        dz /= len;
    }

    glPushMatrix();
    glTranslatef(x, y, z);
    //depois disso, tudo que for desenhado sai centrado no cometa
    // núcleo ("corpo")
    glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.95f, 0.75f);
    glutSolidSphere(0.28f, 20, 20);

    // coma: halo mais suave ao redor
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); //esse modo faz um efeito mais brilhante
    glColor4f(0.8f, 0.9f, 1.0f, 0.25f);
    glutSolidSphere(0.55f, 16, 16);
    glDisable(GL_BLEND);

    // cauda do cometa
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // vetor perpendicular no plano XZ pra dar largura à cauda
    float px = -dz;
    float pz = dx;
    float pm = sqrtf(px * px + pz * pz);
    if (pm > 0.001f) {
        px /= pm;
        pz /= pm;
    }

    const float TAIL_LENGTH = 3.5f;
    const int TAIL_SEGS = 20; //a cauda é dividida em 20 pedaços e cada pedaço é um quad
    //então a cauda fica alongada, afunilando e com alpha variando.

    for (int s = 0; s < TAIL_SEGS; s++) {
        float frac0 = (float)s / TAIL_SEGS;
        float frac1 = (float)(s + 1) / TAIL_SEGS;

        float alpha0 = 0.55f * (1.0f - frac0);
        float alpha1 = 0.55f * (1.0f - frac1);

        float w0 = 0.22f * (1.0f - frac0) + 0.02f;
        float w1 = 0.22f * (1.0f - frac1) + 0.02f;

        // a cauda vai na direção contrária ao movimento, pq a cauda fica para trás em relação ao mov
        float tx0 = -dx * frac0 * TAIL_LENGTH;
        float ty0 = -dy * frac0 * TAIL_LENGTH;
        float tz0 = -dz * frac0 * TAIL_LENGTH;

        float tx1 = -dx * frac1 * TAIL_LENGTH;
        float ty1 = -dy * frac1 * TAIL_LENGTH;
        float tz1 = -dz * frac1 * TAIL_LENGTH;

        glBegin(GL_QUADS);
            glColor4f(0.7f, 0.85f, 1.0f, alpha0);
            glVertex3f(tx0 + px * w0, ty0, tz0 + pz * w0);
            glVertex3f(tx0 - px * w0, ty0, tz0 - pz * w0);

            glColor4f(0.7f, 0.85f, 1.0f, alpha1);
            glVertex3f(tx1 - px * w1, ty1, tz1 - pz * w1);
            glVertex3f(tx1 + px * w1, ty1, tz1 + pz * w1);
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}