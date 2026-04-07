#include "planets.h"
#include "render_utils.h"
#include "textures.h"

#include <math.h>

Planet planets[NUM_PLANETS];

Asteroid asteroids[NUM_ASTEROIDS];

// Ângulos da Lua
float moonOrbitAngle = 0.0f;
float moonSelfAngle  = 0.0f;

// Inicializa os parâmetros básicos de cada planeta, foram ajustados para deixar a visualização melhor na cena:
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

//gera os asteroides do cinturão com pequenas variações de raio, velocidade, tamanho e altura, tudo calculado a partir do índice
void initAsteroids(void)
{
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        float rv = ((i * 31) % 100) / 100.0f - 0.5f;
        //valores pseudoaleatórios baseados no índice i, sem usar rand(). Pro resultado ficar smp igual em cada execução,
        //entao o cinturao fica smp igual a cada execucao
        asteroids[i].orbitRadius = ASTEROID_BELT_RADIUS + rv * ASTEROID_BELT_SPREAD; //distância ao centro do cinturão
        asteroids[i].orbitAngle = (float)i / NUM_ASTEROIDS * 360.0f; //em que ponto do círculo ele começa
        asteroids[i].orbitSpeed = 0.70f + ((i * 17) % 30) / 100.0f; //velocidade angular
        asteroids[i].size = 0.04f + ((i * 13) % 10) / 200.0f; //tamanho previsto
        asteroids[i].y = ((i * 7) % 20) / 100.0f - 0.1f; //altura pequena pro cinturão não ficar 100% chapado
    }
}

// posição atual do planeta no plano XZ. Aqui usamos o ângulo orbital pra converter
// movimento circular em coordenadas cartesianas com cosseno e seno.
void getPlanetWorldPos(int i, float pos[3])
{
    float a = planets[i].orbitAngle * PI / 180.0f;

    pos[0] = planets[i].orbitRadius * cosf(a);
    pos[1] = 0.0f;
    pos[2] = planets[i].orbitRadius * sinf(a);
}

//faz os anéis de Saturno como uma faixa circular texturizada. O blend é ativado para permitir a transparência da textura
void drawSaturnRings(void)
{
    glEnable(GL_BLEND); //tem blend pq a textura do anel pode usar transparência
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //então permite misturar o anel com o fundo

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, saturnRingTexture);

    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 512; i++) {
        float a = 2.0f * PI * i / 512.0f;
        float ca = cosf(a);
        float sa = sinf(a);
        float v = (float)i / 512.0f;

        glTexCoord2f(0.0f, v);
        glVertex3f(SATURN_RING_INNER * ca, 0.0f, SATURN_RING_INNER * sa); // vértice da borda interna

        glTexCoord2f(1.0f, v);
        glVertex3f(SATURN_RING_OUTER * ca, 0.0f, SATURN_RING_OUTER * sa); // vértice da borda externa
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// Desenha um planeta aplicando a hierarquia básica das transformações:
//primeiro ele gira na órbita, depois vai pra sua distância ao Sol, inclina o eixo e por fim gira em torno dele msm
void drawPlanet(int i)
{
    Planet *p = &planets[i];

    drawOrbit(p->orbitRadius); // só desenha a trilha da órbita

    glPushMatrix();

        // coloca o planeta na posição atual da órbita
        glRotatef(p->orbitAngle, 0, 1, 0); // gira o sistema em torno do eixo Y
        glTranslatef(p->orbitRadius, 0, 0); // afasta o planeta do Sol

        // inclinação do eixo e rotação própria
        glRotatef(p->tilt, 0, 0, 1);
        glRotatef(p->selfAngle, 0, 1, 0);

        setMaterial(p->color[0], p->color[1], p->color[2], 60.0f);
        drawTexturedSphere(p->size, 36, 36, p->texture);

        // a Lua é desenhada como filha da Terra
        if (i == EARTH) {
            glRotatef(-p->selfAngle, 0, 1, 0); //“zera” temporariamente a orientação própria da Terra
            glRotatef(-p->tilt, 0, 0, 1); //aí aplica a órbita da Lua num plano mais controlado e ela não fica inclinada

            glPushMatrix();
                glRotatef(moonOrbitAngle, 0, 1, 0); //gira ao redor da Terra
                glTranslatef(MOON_ORBIT_RADIUS, 0, 0); //afasta pela distância orbital
                glRotatef(moonSelfAngle, 0, 1, 0); //gira sobre si mesma

                setMaterial(0.70f, 0.70f, 0.70f, 20.0f);
                drawTexturedSphere(0.12f, 24, 24, moonTexture); //esfera menor com textura própria
            glPopMatrix();
        }

        // os anéis são desenhados só em Saturno
        if (i == SATURN) {
            glRotatef(-p->selfAngle, 0, 1, 0);
            glRotatef(-p->tilt, 0, 0, 1); //evita que os anéis girem “junto com a casca” do planeta de um jeito estranho

            glPushMatrix();
                glRotatef(26.7f, 1, 0, 0); //reaplica uma inclinação controlada
                drawSaturnRings();
            glPopMatrix();
        }

    glPopMatrix(); // restaura a matriz original pro próximo planeta ser desenhado sem herdar a transformação do anterior
}

// Desenha os asteroides do cinturão como pontos. Usa GL_POINTS porque é bem mais leve do que desenhar uma esfera pra cada um.
void drawAsteroids(void)
{
    glDisable(GL_LIGHTING);
    glPointSize(2.5f);

    glBegin(GL_POINTS); //cada asteroide vira um ponto
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        float a = asteroids[i].orbitAngle * PI / 180.0f;

        float gray = 0.45f + ((i * 11) % 30) / 100.0f; // pequena variação no tom de cinza
        glColor3f(gray, gray * 0.95f, gray * 0.90f);

        glVertex3f(
            asteroids[i].orbitRadius * cosf(a),
            asteroids[i].y,
            asteroids[i].orbitRadius * sinf(a)
        );
    }
    glEnd();

    glPointSize(1.0f);
    glEnable(GL_LIGHTING); //ponto simples com iluminação fica estranho ou nem aparece do jeito esperado
}