// implementacao do parser de arquivos .obj

#include "obj_loader.h"

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// guarda tudo que vem do .obj em arrays visíveis apenas aqui
static Vec3 obj_verts[OBJ_MAX_VERTS]; // posicoes dos vertices
static Vec3 obj_norms[OBJ_MAX_VERTS]; // normais de superficie
static Vec2 obj_texcs[OBJ_MAX_VERTS]; // coordenadas de textura UV
static ObjFace obj_faces[OBJ_MAX_FACES]; // faces triangulares 

// contadores de quantos elementos foram lidos de cada tipo
static int obj_nv = 0; // num de vertices
static int obj_nn = 0; // num de normais
static int obj_nt = 0; // num de coords de textura
static int obj_nf = 0; // num de faces

// flag pra dizer se obj carregou
int obj_loaded = 0;

// converte um token de face (ex: "12/34/56") em um ObjIndex com os indices estruturados
static ObjIndex parseFaceIndex(const char *tok)
{
    ObjIndex idx = { 0, -1, -1 }; // default -> sem textura e sem normal
    int vi = 0, ti = 0, ni = 0;

    if (sscanf(tok, "%d/%d/%d", &vi, &ti, &ni) == 3)
        { idx.vi=vi-1; idx.ti=ti-1; idx.ni=ni-1; } // fomato completo -> v/vt/vn
    else if (sscanf(tok, "%d//%d", &vi, &ni) == 2)
        { idx.vi=vi-1; idx.ni=ni-1; } // sem textura -> v//vn
    else if (sscanf(tok, "%d/%d", &vi, &ti) == 2)
        { idx.vi=vi-1; idx.ti=ti-1; } // sem normal -> v/vt
    else if (sscanf(tok, "%d", &vi) == 1)
        { idx.vi=vi-1; } // apenas vertice -> v

    return idx;
}

// lê um arquivo .obj e preenche os arrays
int loadOBJ(const char *path)
{
    FILE *f = fopen(path, "r");

    if (!f) {
        fprintf(stderr, "arquivo de obj nao encontrado: '%s'\n", path);
        return 0;
    }

    // limpa dados antigos
    obj_nv = obj_nn = obj_nt = obj_nf = 0;

    char line[512];

    while (fgets(line, sizeof(line), f))
    {
        // remove \n e \r do final da linha
        int ll = (int)strlen(line);
        if (ll > 0 && line[ll-1] == '\n') line[--ll] = '\0';
        if (ll > 0 && line[ll-1] == '\r') line[--ll] = '\0';

        // posicao de vertice: "v x y z"
        if (strncmp(line, "v ", 2) == 0 && obj_nv < OBJ_MAX_VERTS) {
            Vec3 v;
            sscanf(line+2, "%f %f %f", &v.x, &v.y, &v.z);
            obj_verts[obj_nv++] = v;
        }
        // normal de superficie: "vn x y z"
        else if (strncmp(line, "vn ", 3) == 0 && obj_nn < OBJ_MAX_VERTS) {
            Vec3 n;
            sscanf(line+3, "%f %f %f", &n.x, &n.y, &n.z);
            obj_norms[obj_nn++] = n;
        }
        // coordenada de textura: "vt u v"
        else if (strncmp(line, "vt ", 3) == 0 && obj_nt < OBJ_MAX_VERTS) {
            Vec2 t;
            sscanf(line+3, "%f %f", &t.u, &t.v);
            obj_texcs[obj_nt++] = t;
        }
        // face: "f a b c [d]" (triangulo ou quad)
        else if (strncmp(line, "f ", 2) == 0) {
            char copy[512];
            strncpy(copy, line+2, sizeof(copy)-1);
            copy[sizeof(copy)-1] = '\0';

            ObjIndex idx[4];
            int count = 0;

            // lê até 4 tokens (suporte a quads)
            char *tok = strtok(copy, " \t");
            while (tok && count < 4) {
                idx[count++] = parseFaceIndex(tok);
                tok = strtok(NULL, " \t");
            }

            // triangulo
            if (count >= 3 && obj_nf < OBJ_MAX_FACES) {
                ObjFace fc = { idx[0], idx[1], idx[2] };
                obj_faces[obj_nf++] = fc;
            }
            // quad -> converte em dois triangulos (0,1,2) e (0,2,3)
            if (count == 4 && obj_nf < OBJ_MAX_FACES) {
                ObjFace fc = { idx[0], idx[2], idx[3] };
                obj_faces[obj_nf++] = fc;
            }
        }
    }
    fclose(f);
    printf("objs: '%s': %d verts, %d normais, %d UVs, %d faces\n", path, obj_nv, obj_nn, obj_nt, obj_nf);
    return 1;
}

// desenha o modelo carregado usando glBegin/glEnd
void drawOBJModel(void)
{
    if (!obj_loaded || obj_nf == 0) return;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < obj_nf; i++) { // loop nas faces
        // cada face tem 3 vertices, cada um com seus indices de vertice, normal e textura
        ObjIndex *idx[3] = {
            &obj_faces[i].a,
            &obj_faces[i].b,
            &obj_faces[i].c
        };

        for (int j = 0; j < 3; j++) {
            ObjIndex *ix = idx[j];

            if (ix->ni >= 0 && ix->ni < obj_nn) // normal -> iluminação
                glNormal3f(obj_norms[ix->ni].x, obj_norms[ix->ni].y, obj_norms[ix->ni].z);

            if (ix->ti >= 0 && ix->ti < obj_nt) // textura -> mapeamento UV
                glTexCoord2f(obj_texcs[ix->ti].u, obj_texcs[ix->ti].v);

            if (ix->vi >= 0 && ix->vi < obj_nv) // vertice -> posição no espaço
                glVertex3f(obj_verts[ix->vi].x, obj_verts[ix->vi].y, obj_verts[ix->vi].z);
        }
    }
    glEnd();
}