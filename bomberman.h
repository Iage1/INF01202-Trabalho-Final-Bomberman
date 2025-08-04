// Header guard
#ifndef BOMBERMAN_H
#define BOMBERMAN_H

// Bibliotecas e constantes
#include "raylib.h"
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#define LINHAS 25
#define COLUNAS 60
#define ALTURA 600
#define LARGURA 1200
#define LADO 20
#define MAXVIDA 3
#define MAXBOMBA 3
#define NINIMIGOS 5
#define TAMTEXTO 40
#define NMAPAS 5

// Structs
typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    Position posic;
    int vida;
    int pontuacao;
    int nBombas;
    int nChaves;
    int direcao;
    double delay;
} Player;

typedef struct
{
    Position posic;
    int direcao;
} Enemy;

typedef struct
{
    Position posic;
    int estadoAtiva;
    double tempo; //GetTime() � double
    int estadoExplosao;
    double tempoExplosao;
} Bomb;

typedef struct{
    Player jogador;
    Enemy inimigo[NINIMIGOS];
    Bomb bomba[MAXBOMBA];
    char mapa[LINHAS][COLUNAS];
    char nomeArq[50];
}Save;


// Protótipos
void leMapa(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[]);

void desenhaMapa(char mapa[][COLUNAS]);

char ehPossivelDireita(Position posic, char mapa[][COLUNAS]);
char ehPossivelEsquerda(Position posic, char mapa[][COLUNAS]);
char ehPossivelCima(Position posic, char mapa[][COLUNAS]);
char ehPossivelBaixo(Position posic, char mapa[][COLUNAS]);

void moveJogador(Player *jogador, char mapa[][COLUNAS]);
void moveInimigos(Enemy inimigo[], char mapa[][COLUNAS], Player *jogador);

void plantaBomba(Player *jogador, char mapa[][COLUNAS], Enemy inimigo[], Bomb bomba[]);
void propagaExplosao(char mapa[][COLUNAS], Enemy inimigo[], int x, int y, int direcx, int direcy, Player *jogador);
void desenhaExplosao(Bomb bomba[], char mapa[][COLUNAS]);
void desenhaPropagacao(char mapa[][COLUNAS], int x, int y, int direcx, int direcy);

void chave(Player *jogador, char mapa[][COLUNAS], Enemy inimigo[], Bomb bomba[], int *pausa);
void morte(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], int *pausa);

void displayInferior(Player *jogador, Bomb bomba[]);

void menu(int *pausa, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], char *nome_arq, Save *optimus);
void novoJogo(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], int *pausa);
void saveGame(char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], char *nome_arq, Save *optimus);
void loadGame(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], int *pausa, Save *optimus);


#endif 