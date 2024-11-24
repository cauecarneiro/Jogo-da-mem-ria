#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estruturas do jogo
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

// Função para embaralhar as cartas do tabuleiro
void embaralhar(Carta* cartas, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        int j = rand() % tamanho;  // Gera um índice aleatório
        Carta temp = cartas[i];    // Troca as cartas nas posições i e j
        cartas[i] = cartas[j];
        cartas[j] = temp;
    }
}

// Função para carregar as cartas do tabuleiro de um arquivo
Tabuleiro* carregarTabuleiro(const char* nomeArquivo) {
    FILE* file = fopen(nomeArquivo, "r");  // Abre o arquivo para leitura
    if (!file) {  // Verifica se o arquivo foi aberto com sucesso
        printf("Erro ao carregar a fase: %s\n", nomeArquivo);
        return NULL;
    }

    int tamanho;
    fscanf(file, "%d", &tamanho);  // Lê o número de cartas do tabuleiro

    Tabuleiro* tabuleiro = (Tabuleiro*)malloc(sizeof(Tabuleiro));  // Aloca memória para o tabuleiro
    tabuleiro->tamanho = tamanho;
    tabuleiro->cartas = (Carta*)malloc(tamanho * sizeof(Carta));  // Aloca memória para as cartas

    // Lê as palavras das cartas do arquivo
    for (int i = 0; i < tamanho; i++) {
        fscanf(file, "%s", tabuleiro->cartas[i].palavra);
        tabuleiro->cartas[i].revelada = 0;  // Inicialmente, todas as cartas estão ocultas
    }

    fclose(file);  // Fecha o arquivo
    embaralhar(tabuleiro->cartas, tamanho);  // Embaralha as cartas
    return tabuleiro;
}

// Função para salvar as estatísticas do jogador em um arquivo binário
void salvarEstatisticas(const char* arquivo, Jogador* jogador) {
    FILE* file = fopen(arquivo, "ab");  // Abre o arquivo no modo de adição binária
    if (!file) {
        printf("Erro ao salvar as estatísticas.\n");
        return;
    }
    fwrite(jogador, sizeof(Jogador), 1, file);  // Escreve os dados do jogador no arquivo
    fclose(file);
}

// Função para exibir as estatísticas de todos os jogadores
void mostrarEstatisticas(const char* arquivo) {
    FILE* file = fopen(arquivo, "rb");  // Abre o arquivo no modo de leitura binária
    if (!file) {
        printf("Nenhuma estatística encontrada.\n");
        return;
    }

    Jogador jogador;
    printf("Placar dos Jogadores:\n");
    // Lê e exibe os dados de cada jogador até o final do arquivo
    while (fread(&jogador, sizeof(Jogador), 1, file)) {
        printf("Nome: %s, Score: %d\n", jogador.nome, jogador.score);
    }
    fclose(file);
}

// Função para exibir o tabuleiro na tela
void exibirTabuleiro(Tabuleiro* tabuleiro) {
    printf("\nTabuleiro:\n");
    for (int i = 0; i < tabuleiro->tamanho; i++) {
        if (tabuleiro->cartas[i].revelada)
            printf("[%s] ", tabuleiro->cartas[i].palavra);  // Exibe a palavra da carta revelada
        else
            printf("[*] ");  // Exibe um asterisco para as cartas ocultas
    }
    printf("\n");
}

// Função que implementa a lógica de uma fase do jogo
int jogarFase(Tabuleiro* tabuleiro, Jogador* jogador) {
    int tentativas = tabuleiro->tamanho;  // Número inicial de tentativas
    int acertos = 0;  // Número de pares encontrados

    while (tentativas > 0 && acertos < tabuleiro->tamanho / 2) {
        exibirTabuleiro(tabuleiro);

        int escolha1, escolha2;
        printf("Escolha duas cartas (0 a %d) ou digite -1 para voltar ao menu: ", tabuleiro->tamanho - 1);
        scanf("%d", &escolha1);

        if (escolha1 == -1) {
            return 0;  // Sinaliza para voltar ao menu
        }

        scanf("%d", &escolha2);
        if (escolha2 == -1) {
            return 0;  // Sinaliza para voltar ao menu
        }

        // Valida as escolhas do jogador
        if (escolha1 < 0 || escolha1 >= tabuleiro->tamanho || 
            escolha2 < 0 || escolha2 >= tabuleiro->tamanho || 
            escolha1 == escolha2 || 
            tabuleiro->cartas[escolha1].revelada || 
            tabuleiro->cartas[escolha2].revelada) {
            printf("Escolha inválida!\n");
            continue;
        }

        // Revela as cartas escolhidas
        tabuleiro->cartas[escolha1].revelada = 1;
        tabuleiro->cartas[escolha2].revelada = 1;

        exibirTabuleiro(tabuleiro);

        // Verifica se as cartas formam um par
        if (strcmp(tabuleiro->cartas[escolha1].palavra, tabuleiro->cartas[escolha2].palavra) == 0) {
            printf("Você encontrou um par!\n");
            acertos++;
            jogador->score += 10;  // Incrementa a pontuação do jogador
        } else {
            printf("Não é um par.\n");
            tabuleiro->cartas[escolha1].revelada = 0;
            tabuleiro->cartas[escolha2].revelada = 0;  // Esconde novamente as cartas
            tentativas--;
        }
    }

    return acertos == tabuleiro->tamanho / 2;  // Retorna se o jogador completou a fase
}

// Função principal
int main() {
    const char* arquivosFases[] = {"fase1.txt", "fase2.txt", "fase3.txt", "fase4.txt", "fase5.txt"};
    const char* arquivoEstatisticas = "estatisticas.bin";
    int escolha;

    srand(time(NULL));  // Inicializa o gerador de números aleatórios
    Jogador jogador;
    printf("Bem-vindo ao Jogo da Memória!\n");
    printf("Digite seu nome: ");
    fgets(jogador.nome, sizeof(jogador.nome), stdin);
    jogador.nome[strcspn(jogador.nome, "\n")] = '\0';  // Remove o caractere de nova linha
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
                if (jogarFase(tabuleiro, &jogador)) {
                    printf("Você completou a fase!\n");
                } else {
                    printf("Você voltou ao menu.\n");
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

