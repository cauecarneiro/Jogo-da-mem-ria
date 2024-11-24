#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <stdio.h>
#include "cartas.h"

#define LARGURA_TELA 800
#define ALTURA_TELA 600

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();  // Inicializar o SDL_ttf

    SDL_Window* janela = SDL_CreateWindow("Jogo da Memória",
                                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         LARGURA_TELA, ALTURA_TELA, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);

    srand(time(NULL));

    Carta cartas_jogo[NUM_CARTAS];
    inicializar_cartas(cartas_jogo); // Inicializa as cartas
    embaralhar_cartas(cartas_jogo); // Embaralha as cartas

    int jogo_rodando = 1;
    SDL_Event evento;
    int carta1_virada = -1, carta2_virada = -1;

    // Inicializa a fonte
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);  // Caminho para a fonte e tamanho
    if (font == NULL) {
        printf("Erro ao carregar fonte: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color cor_texto = { 0, 0, 255 };  // Cor do texto (azul)

    while (jogo_rodando) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                jogo_rodando = 0;
            }

            if (evento.type == SDL_MOUSEBUTTONDOWN) {
                int x = evento.button.x, y = evento.button.y;
                for (int i = 0; i < NUM_CARTAS; i++) {
                    if (x >= cartas_jogo[i].rect.x && x <= cartas_jogo[i].rect.x + TAMANHO_CARTA &&
                        y >= cartas_jogo[i].rect.y && y <= cartas_jogo[i].rect.y + TAMANHO_CARTA) {
                        // Virar a carta
                        if (carta1_virada == -1) {
                            carta1_virada = i;
                            cartas_jogo[i].virada = 1;
                        } else if (carta2_virada == -1 && i != carta1_virada) {
                            carta2_virada = i;
                            cartas_jogo[i].virada = 1;
                        }
                    }
                }
            }
        }

        // Verificar se as cartas viradas são iguais
        if (carta1_virada != -1 && carta2_virada != -1) {
            if (cartas_jogo[carta1_virada].valor != cartas_jogo[carta2_virada].valor) {
                SDL_Delay(1000);  // Espera 1 segundo antes de virar as cartas novamente
                cartas_jogo[carta1_virada].virada = 0;
                cartas_jogo[carta2_virada].virada = 0;
            }
            carta1_virada = carta2_virada = -1;  // Reseta as cartas viradas
        }

        // Renderizar a tela
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Cor de fundo (branco)
        SDL_RenderClear(renderer);

        // Desenhar as cartas
        for (int i = 0; i < NUM_CARTAS; i++) {
            // Desenhar o retângulo da carta
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Cor da borda (preto)
            SDL_RenderFillRect(renderer, &cartas_jogo[i].rect);

            if (cartas_jogo[i].virada) {
                // Renderizar o texto com a SDL_ttf
                SDL_Surface* text_surface = TTF_RenderText_Solid(font, cartas_jogo[i].valor, cor_texto);
                SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
                SDL_FreeSurface(text_surface);  // Liberar a superfície

                // Posição do texto
                SDL_Rect text_rect = { cartas_jogo[i].rect.x + 20, cartas_jogo[i].rect.y + 20, text_surface->w, text_surface->h };

                SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
                SDL_DestroyTexture(text_texture);  // Liberar a textura
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // Controla o FPS (60 FPS)
    }

    // Liberar a fonte e fechar o SDL_ttf
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(janela);
    SDL_Quit();
    return 0;
}
