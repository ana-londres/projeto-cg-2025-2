#ifndef HUD_H
#define HUD_H

//exibe informacoes de estado da simulacao sobre a cena 3D,

extern int showLegend; // Flag global: 1 = mostrar HUD, 0 = ocultar (controlado pela tecla H) 

void drawHUD(int windowWidth, int windowHeight, const char *camName);

#endif 