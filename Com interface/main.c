#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    char palavra[20];   // Representa a palavra de uma carta
    int revelada;       // Indica se a carta já foi revelada (1) ou não (0)
} Carta;

typedef struct {
    Carta* cartas;      // Array dinâmico de cartas
    int tamanho;        // Quantidade de cartas no tabuleiro
} Tabuleiro;

typedef struct {
    char nome[50];      // Nome do jogador
    int score;          // Pontuação do jogador
} Jogador;

void embaralhar(Carta* cartas, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        int j = rand() % tamanho;
        Carta temp = cartas[i];
        cartas[i] = cartas[j];
        cartas[j] = temp;
    }
}

Tabuleiro* carregarTabuleiro(const char* nomeArquivo) {
    FILE* file = fopen(nomeArquivo, "r");
    if (!file) {
        printf("Erro ao carregar a fase: %s\n", nomeArquivo);
        return NULL;
    }

    int tamanho;
    fscanf(file, "%d", &tamanho);

    Tabuleiro* tabuleiro = (Tabuleiro*)malloc(sizeof(Tabuleiro));
    tabuleiro->tamanho = tamanho;
    tabuleiro->cartas = (Carta*)malloc(tamanho * sizeof(Carta));

    for (int i = 0; i < tamanho; i++) {
        fscanf(file, "%s", tabuleiro->cartas[i].palavra);
        tabuleiro->cartas[i].revelada = 0;
    }

    fclose(file);
    embaralhar(tabuleiro->cartas, tamanho);
    return tabuleiro;
}

void salvarEstatisticas(const char* arquivo, Jogador* jogador) {
    FILE* file = fopen(arquivo, "ab");
    if (!file) {
        printf("Erro ao salvar as estatísticas.\n");
        return;
    }
    fwrite(jogador, sizeof(Jogador), 1, file);
    fclose(file);
}

void mostrarEstatisticas(const char* arquivo) {
    FILE* file = fopen(arquivo, "rb");
    if (!file) {
        printf("Nenhuma estatística encontrada.\n");
        return;
    }

    Jogador jogador;
    printf("Placar dos Jogadores:\n");
    while (fread(&jogador, sizeof(Jogador), 1, file)) {
        printf("Nome: %s, Score: %d\n", jogador.nome, jogador.score);
    }
    fclose(file);
}

void desenharTexto(SDL_Renderer* renderer, TTF_Font* font, const char* texto, int x, int y, int largura, int altura) {
    SDL_Color cor = {255, 255, 255}; // Cor branca
    SDL_Surface* surfaceTexto = TTF_RenderText_Blended(font, texto, cor);
    SDL_Texture* texturaTexto = SDL_CreateTextureFromSurface(renderer, surfaceTexto);

    SDL_Rect destino = {x + largura / 4, y + altura / 4, largura / 2, altura / 2};
    SDL_RenderCopy(renderer, texturaTexto, NULL, &destino);

    SDL_FreeSurface(surfaceTexto);
    SDL_DestroyTexture(texturaTexto);
}

void desenharTabuleiro(SDL_Renderer* renderer, TTF_Font* font, Tabuleiro* tabuleiro, int linhas, int colunas, int largura, int altura) {
    for (int i = 0; i < tabuleiro->tamanho; i++) {
        int x = (i % colunas) * largura;
        int y = (i / colunas) * altura;

        SDL_Rect rect = {x, y, largura, altura};

        if (tabuleiro->cartas[i].revelada) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Verde para cartas reveladas
            SDL_RenderFillRect(renderer, &rect);
            desenharTexto(renderer, font, tabuleiro->cartas[i].palavra, x, y, largura, altura); // Exibe o texto da carta
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Azul para cartas ocultas
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Borda branca
        SDL_RenderDrawRect(renderer, &rect);
    }
}

int jogarFaseSDL(Tabuleiro* tabuleiro, Jogador* jogador) {
    const int larguraCarta = 100;
    const int alturaCarta = 150;
    const int colunas = 4;
    const int linhas = (tabuleiro->tamanho + colunas - 1) / colunas;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() < 0) {
        printf("Erro ao inicializar SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Erro ao carregar fonte: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 0;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Jogo da Memória",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        larguraCarta * colunas,
        alturaCarta * linhas,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Erro ao criar a janela: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 0;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erro ao criar o renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 0;
    }

    int running = 1, tentativas = tabuleiro->tamanho, acertos = 0, escolha1 = -1, escolha2 = -1;

    while (running && tentativas > 0 && acertos < tabuleiro->tamanho / 2) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                break;
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int coluna = e.button.x / larguraCarta;
                int linha = e.button.y / alturaCarta;
                int indice = linha * colunas + coluna;

                if (indice < tabuleiro->tamanho && !tabuleiro->cartas[indice].revelada) {
                    if (escolha1 == -1) {
                        escolha1 = indice;
                        tabuleiro->cartas[indice].revelada = 1;
                    } else if (escolha2 == -1 && indice != escolha1) {  // Impede que a mesma carta seja selecionada duas vezes
                        escolha2 = indice;
                        tabuleiro->cartas[indice].revelada = 1;

                        // Verificação se as palavras das cartas são iguais
                        if (strcmp(tabuleiro->cartas[escolha1].palavra, tabuleiro->cartas[escolha2].palavra) == 0) {
                            acertos++;
                            jogador->score += 10;
                            escolha1 = escolha2 = -1; // Reseta as escolhas após acerto
                        } else {
                            SDL_Delay(1000); // Espera antes de virar as cartas de volta
                            tabuleiro->cartas[escolha1].revelada = 0;
                            tabuleiro->cartas[escolha2].revelada = 0;
                            escolha1 = escolha2 = -1; // Reseta as escolhas após erro
                        }
                        tentativas--;
                    }
                }
            }

        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        desenharTabuleiro(renderer, font, tabuleiro, linhas, colunas, larguraCarta, alturaCarta);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return acertos == tabuleiro->tamanho / 2;
}

int main() {
    const char* arquivosFases[] = {"fase1.txt", "fase2.txt", "fase3.txt", "fase4.txt", "fase5.txt"};
    const char* arquivoEstatisticas = "estatisticas.bin";
    int escolha;

    srand(time(NULL));
    Jogador jogador;
    printf("Bem-vindo ao Jogo da Memória!\n");
    printf("Digite seu nome: ");
    fgets(jogador.nome, sizeof(jogador.nome), stdin);
    jogador.nome[strcspn(jogador.nome, "\n")] = '\0';
    jogador.score = 0;

    while (1) {
        printf("\nMenu Principal:\n");
        printf("1. Iniciar Jogo\n");
        printf("2. Estatísticas\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &escolha);

        if (escolha == 1) {
            for (int i = 0; i < 5; i++) {
                Tabuleiro* tabuleiro = carregarTabuleiro(arquivosFases[i]);
                if (!tabuleiro) break;

                printf("\nFase %d:\n", i + 1);
                if (jogarFaseSDL(tabuleiro, &jogador)) {
                    printf("Você completou a fase!\n");
                } else {
                    printf("Você saiu da fase.\n");
                    free(tabuleiro->cartas);
                    free(tabuleiro);
                    break;
                }

                free(tabuleiro->cartas);
                free(tabuleiro);
            }
            printf("Jogo concluído! Seu score final: %d\n", jogador.score);
            salvarEstatisticas(arquivoEstatisticas, &jogador);
        } else if (escolha == 2) {
            mostrarEstatisticas(arquivoEstatisticas);
        } else if (escolha == 3) {
            printf("Saindo do jogo. Até logo!\n");
            break;
        } else {
            printf("Opção inválida.\n");
        }
    }

    return 0;
}
