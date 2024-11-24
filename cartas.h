#ifndef CARTAS_H
#define CARTAS_H

#include <SDL2/SDL.h>

#define NUM_CARTAS 6
#define TAMANHO_CARTA 100

// Definição da estrutura da Carta
typedef struct {
    SDL_Rect rect;    // Posição da carta na tela
    const char* valor; // Valor da carta (ex: "A", "B", "C")
    int virada;        // Estado da carta (0 para virada para baixo, 1 para virada para cima)
} Carta;

// Funções para manipulação das cartas
void embaralhar_cartas(Carta* cartas);
void inicializar_cartas(Carta* cartas);

#endif
