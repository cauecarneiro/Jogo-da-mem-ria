#include "cartas.h"
#include <stdlib.h>
#include <time.h>

// Função para embaralhar as cartas
void embaralhar_cartas(Carta* cartas) {
    for (int i = 0; i < NUM_CARTAS; i++) {
        int j = rand() % NUM_CARTAS;
        Carta temp = cartas[i];
        cartas[i] = cartas[j];
        cartas[j] = temp;
    }
}

// Função para inicializar as cartas
void inicializar_cartas(Carta* cartas) {
    const char* valores[NUM_CARTAS] = { "A", "B", "C", "A", "B", "C" };
    
    // Definir as cartas e suas posições
    for (int i = 0; i < NUM_CARTAS; i++) {
        cartas[i].rect = (SDL_Rect){ (i % 3) * (TAMANHO_CARTA + 10) + 100, (i / 3) * (TAMANHO_CARTA + 10) + 100, TAMANHO_CARTA, TAMANHO_CARTA };
        cartas[i].valor = valores[i];
        cartas[i].virada = 0; // Inicialmente, todas as cartas estão viradas para baixo
    }
}
