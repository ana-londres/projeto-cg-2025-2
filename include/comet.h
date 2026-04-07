#ifndef COMET_H
#define COMET_H

//módulo do cometa: curva de Bézier cúbica, trajetória e desenho do cometa

// pontos de controle da curva de Bézier
extern float bezierCtrl[4][3];

extern float cometT;

// calcula a posição (x, y, z) do cometa na curva para um dado t
void bezierPoint(float t, float *x, float *y, float *z);

//usada pra orientar a cauda
void bezierTangent(float t, float *dx, float *dy, float *dz);

void drawBezierCurve(void);

void drawComet(void);

#endif