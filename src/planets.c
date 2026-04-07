#include "planets.h"

#include <math.h>

Planet planets[NUM_PLANETS];

Asteroid asteroids[NUM_ASTEROIDS];

// Ângulos da Lua
float moonOrbitAngle = 0.0f;
float moonSelfAngle  = 0.0f;

// Inicializa os parâmetros básicos de cada planeta, foram ajustados para deixar a visualização melhor na cena.
//distância ao Sol na cena, tamanho da esfera, velocidade da órbita, 
//velocidade de rotação própria, posição inicial na órbita, rotação inicial, inclinação axial, cor base, textura ainda não ligada, nome
void initPlanets(void)
{
    planets[MERCURY] = (Planet){ 3.5f,  0.22f, 2.00f, 1.8f,   0.0f, 0.0f,   2.0f, {0.65f, 0.65f, 0.65f}, 0, "Mercurio" };
    planets[VENUS]   = (Planet){ 5.5f,  0.36f, 1.50f, 1.2f,  40.0f, 0.0f, 177.0f, {0.88f, 0.72f, 0.38f}, 0, "Venus"    };
    planets[EARTH]   = (Planet){ 7.5f,  0.40f, 1.10f, 3.0f,  80.0f, 0.0f,  23.5f, {0.20f, 0.50f, 1.00f}, 0, "Terra"    };
    planets[MARS]    = (Planet){ 9.5f,  0.30f, 0.85f, 2.5f, 130.0f, 0.0f,  25.0f, {0.85f, 0.28f, 0.18f}, 0, "Marte"    };
    planets[JUPITER] = (Planet){13.5f,  0.90f, 0.55f, 5.0f, 180.0f, 0.0f,   3.0f, {0.82f, 0.65f, 0.45f}, 0, "Jupiter"  };
    planets[SATURN]  = (Planet){17.0f,  0.78f, 0.40f, 4.5f, 225.0f, 0.0f,  26.7f, {0.88f, 0.82f, 0.58f}, 0, "Saturno"  };
    planets[URANUS]  = (Planet){20.5f,  0.56f, 0.28f, 2.5f, 270.0f, 0.0f,  97.8f, {0.45f, 0.88f, 0.92f}, 0, "Urano"    };
    planets[NEPTUNE] = (Planet){24.0f,  0.54f, 0.20f, 2.2f, 315.0f, 0.0f,  28.3f, {0.25f, 0.45f, 0.95f}, 0, "Netuno"   };
}

// posição atual do planeta no plano XZ. Aqui usamos o ângulo orbital pra converter movimento circular em coordenadas cartesianas com cosseno e seno
//pega um planeta e calcula onde ele está no espaço da cena.
void getPlanetWorldPos(int i, float pos[3])
{
    float a = planets[i].orbitAngle * PI / 180.0f;

    pos[0] = planets[i].orbitRadius * cosf(a);
    pos[1] = 0.0f;
    pos[2] = planets[i].orbitRadius * sinf(a);
}