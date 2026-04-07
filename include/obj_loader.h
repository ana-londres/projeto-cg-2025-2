#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

// limites maximos de elementos suportados pelo loader
#define OBJ_MAX_VERTS  200000
#define OBJ_MAX_FACES  200000

// vetor 3d -> posicao de vertice/normal de superficie
typedef struct { float x, y, z; } Vec3;

// coord de textura 2d -> (u, v)
typedef struct { float u, v; } Vec2;

// indice de um vertice dentro de uma face obj
typedef struct { int vi, ti, ni; } ObjIndex;

// face triangular composta por tres indices obj
typedef struct { ObjIndex a, b, c; } ObjFace;

// flag de carregamento
extern int obj_loaded;

// arrays de dados do modelo OBJ
int loadOBJ(const char *path);

// desenha o modelo OBJ usando glBegin(GL_TRIANGLES) e os dados carregados
void drawOBJModel(void);

#endif