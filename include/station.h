#ifndef STATION_H
#define STATION_H

extern float shipOrbitAngle; // angulo na orbita ao redor da Terra
extern float shipSelfAngle;  // rotacao propria da estacao

// desenha a estação seguindo a Terra em sua orbita, com rotação própria e textura
void drawSpaceStation(void);

// desenha a linha de orbita da estacao em torno da Terra
void drawStationOrbit(void);

#endif