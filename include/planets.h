#ifndef PLANETS_H
#define PLANETS_H

//dados usados para representar cada planeta na cena como distância orbital, tamanho, rotação, inclinação e nome. 

#include <GL/glut.h>

#define PI 3.14159265358979323846f
#define NUM_PLANETS 8

#define NUM_ASTEROIDS         180
#define ASTEROID_BELT_RADIUS  10.5f
#define ASTEROID_BELT_SPREAD  1.2f

#define SATURN_RING_INNER     1.35f
#define SATURN_RING_OUTER     2.20f

#define SUN_RADIUS            1.7f
#define MOON_ORBIT_RADIUS     0.85f

// Cada planeta guarda seus parâmetros principais de órbita, rotação, inclinação, cor, textura e nome.
typedef struct {
    float orbitRadius;
    float size;
    float orbitSpeed;
    float rotationSpeed;
    float orbitAngle;
    float selfAngle;
    float tilt;
    float color[3];
    GLuint texture;
    const char *name;
} Planet;

/* Estrutura reservada pro cinturão de asteroides.
   Ela já fica definida aqui porque vai ser usada depois. */
typedef struct {
    float orbitRadius;
    float orbitAngle;
    float orbitSpeed;
    float size;
    float y;
} Asteroid;

/* índices nomeados pro acesso no vetor planets[] */
typedef enum {
    MERCURY = 0,
    VENUS,
    EARTH,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE
} PlanetIndex;

// Vetores globais definidos em planets.c
extern Planet planets[NUM_PLANETS];
extern Asteroid asteroids[NUM_ASTEROIDS];

/* Ângulos da Lua, usados na animação */
extern float moonOrbitAngle;
extern float moonSelfAngle;

/* Inicializa os dados básicos dos planetas */
void initPlanets(void);

// Essas funções serão implementadas nas próximas etapas 
void initAsteroids(void);
void drawPlanet(int i);
void drawAsteroids(void);
void drawSaturnRings(void);

//posição atual do planeta no espaço da cena 
void getPlanetWorldPos(int i, float pos[3]);

#endif