#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "textures.h"
#include "planets.h"

#include <GL/glut.h>
#include <stdio.h>

// ids de textura globais dos objetos especiais (sol, lua, anel de saturno, estacao espacial)
GLuint moonTexture = 0;
GLuint sunTexture = 0;
GLuint saturnRingTexture = 0;
GLuint stationTexture = 0;
GLuint starsTexture = 0;

void loadPNGTexture(GLuint *texID, const char *path)
{
    int w, h, ch;
    unsigned char *data = stbi_load(path, &w, &h, &ch, 3); // stbi_load decodifica o PNG e retorna pixels em RGB -> 3 canais
    
    if (!data) {
        fprintf(stderr, "arquivo nao encontrado: '%s'\n", path);
        return;
    }

    glGenTextures(1, texID); // cria um id de textura
    glBindTexture(GL_TEXTURE_2D, *texID); // ativa a textura

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // uso da textura com obj longe/menor -> LINEAR suaviza
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // obj perto
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // comportamento na horizontal
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // vertical

    // gera mipmaps automaticamente via pixels carregados
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data); // libera memoria
    printf("'%s' carregado: %dx%d\n", path, w, h);
}

// carrega arquivos png na ordem do enum em planets.h
void setupTextures(void)
{
    loadPNGTexture(&planets[MERCURY].texture, "assets/mercury.png");
    loadPNGTexture(&planets[VENUS].texture,   "assets/venus.png");
    loadPNGTexture(&planets[EARTH].texture,   "assets/earth.png");
    loadPNGTexture(&planets[MARS].texture,    "assets/mars.png");
    loadPNGTexture(&planets[JUPITER].texture, "assets/jupiter.png");
    loadPNGTexture(&planets[SATURN].texture,  "assets/saturn.png");
    loadPNGTexture(&planets[URANUS].texture,  "assets/uranus.png");
    loadPNGTexture(&planets[NEPTUNE].texture, "assets/neptune.png");
    loadPNGTexture(&saturnRingTexture, "assets/saturn_ring.png");
    loadPNGTexture(&moonTexture,       "assets/moon.png");
    loadPNGTexture(&sunTexture,        "assets/sun.png");
    loadPNGTexture(&stationTexture,    "assets/SpaceStation.png");
    loadPNGTexture(&starsTexture, "assets/stars.png");
}