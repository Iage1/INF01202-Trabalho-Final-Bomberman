/* Esse programa visa ser uma reprodução do jogo chamado "Bomberman" para NES */

#include <raylib.h>
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


typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    Position posic;
    int vida;
    int nBomb;
    int points;
    int nChaves;
} Player;

typedef struct
{
    Position posic;
    int direcao;
} Enemy;


void leMapa(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[]);

void desenhaMapa(char mapa[][COLUNAS]);

char ehPossivelDireita(Position posic, char mapa[][COLUNAS]);
char ehPossivelEsquerda(Position posic, char mapa[][COLUNAS]);
char ehPossivelCima(Position posic, char mapa[][COLUNAS]);
char ehPossivelBaixo(Position jogador, char mapa[][COLUNAS]);

void moveJogador(Player *jogador, char mapa[][COLUNAS]);
void moveInimigos(Enemy inimigo, char mapa[][COLUNAS]);


int main()
{

    Player jogador;
    Enemy inimigo[5];
    char mapa[LINHAS][COLUNAS];
    char nomeArq[] = "mapa01.txt";


    InitWindow(LARGURA, ALTURA, "Bomberman Crakeado");//Inicializa janela, com certo tamanho e título
    SetTargetFPS(60);// Ajusta a execução do jogo para 60 frames por segundo

    leMapa(nomeArq, mapa, &jogador, inimigo); //Le o arquivo do mapa e cria a matriz de caracteres com base no arquivo texto do mapa; ja pega a posicao do jogador e dos inimigos
                                              //Tambem atribui uma direcao inicial aleatoria aos inimigos (entre 0 e 3)

    while (!WindowShouldClose()) // Laço principal do jogo
    {
        //Desenha o mapa com base no arquivo
        desenhaMapa(mapa);

        //Movimento
        moveJogador(&jogador, mapa);
      //moveInimigo

        BeginDrawing();
        ClearBackground(DARKBLUE);

        //Desenha o jogador e os inimigos
        DrawRectangle(jogador.posic.x, jogador.posic.y, LADO, LADO, DARKGREEN);
        for (int i=0; i<5; i++)
        {
            DrawRectangle(inimigo[i].posic.x, inimigo[i].posic.y, LADO, LADO, RED);
        }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}

//Aqui ele pega i<LINHAS E j<COLUNAS, e na hora de desenhar o mapa eh o contrario!?
void leMapa(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[])
{
    Position posic;
    FILE *arq;
    int i, j, d=0;
    srand(time(NULL));


    arq = fopen(nome_arq, "r");
    if(arq == NULL)
        printf("Erro ao abrir o arquivo desejado.");
    else
    {
        for(i=0; i<LINHAS; i++)
        {
            for(j=0; j<COLUNAS; j++)
            {
                mapa[i][j] = fgetc(arq);
                if (mapa[i][j] == 'E')
                {
                    inimigo[d].posic.x = j*20;
                    inimigo[d].posic.y = i*20;
                    inimigo[d].direcao = rand()%4;
                    d++;
                    //mapa[i][j] = ' '; //Fazer com que a posicao inicial dos inimigos nao fique "dura"
                }

                else if(mapa[i][j] == 'J')
                {
                    (*jogador).posic.x = j*20;
                    (*jogador).posic.y = i*20;
                    mapa[i][j] = ' '; //Fazer com que a posicao inicial do jogador nao fique "dura"
                }
            }
            fgetc(arq);
        }
        fclose(arq);
    }

}

void desenhaMapa(char mapa[][COLUNAS])
{
    int i, j;

    for(i=0; i<COLUNAS; i++)
    {
        for(j=0; j<LINHAS; j++)
        {
            if(mapa[j][i] == 'W')
                DrawRectangle(i*20, j*20, LADO, LADO, DARKGRAY);
            else if(mapa[j][i] == 'D')
                DrawRectangle(i*20, j*20, LADO, LADO, LIGHTGRAY);
            else if(mapa[j][i] == 'B' || mapa[j][i] == 'K')
                DrawRectangle(i*20, j*20, LADO, LADO, BROWN);
        }
    }
}

//Foi preciso trocar linhas por colunas
char ehPossivelDireita(Position posic, char mapa[][COLUNAS])
{

    int flx, fly;
    char veredito;

    flx = floor(posic.x/20);
    fly = floor(posic.y/20);

    if(posic.y % 20 == 0)
    {
        if(mapa[fly][flx+1] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else
    {
        if(mapa[fly][flx+1] == ' ' && mapa[fly+1][flx+1] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }

    return veredito;
}

char ehPossivelEsquerda (Position posic, char mapa[][COLUNAS])
{

    int clx, fly;
    char veredito;

    clx = ceil(posic.x/20);
    fly = floor(posic.y/20);

    if(posic.y % 20 == 0 && posic.x % 20 == 0)
    {
        if(mapa[fly][clx-1] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.y % 20 != 0 && posic.x % 20 == 0)
    {
        if(mapa[fly][clx-1] == ' ' && mapa[fly+1][clx-1] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.y % 20 == 0 && posic.x % 20 != 0)
    {
        if(mapa[fly][clx] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else
    {
        if(mapa[fly][clx] == ' ' && mapa[fly+1][clx] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }

    return veredito;
}

char ehPossivelBaixo(Position posic, char mapa[][COLUNAS])
{

    int flx, fly;
    char veredito;

    flx = floor(posic.x/20);
    fly = floor(posic.y/20);

    if(posic.x % 20 == 0)
    {
        if(mapa[fly+1][flx] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else
    {
        if(mapa[fly+1][flx] == ' ' && mapa[fly+1][flx+1] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }

    return veredito;
}

char ehPossivelCima (Position posic, char mapa[][COLUNAS])
{

    int flx, cly;
    char veredito;

    flx = floor(posic.x/20);
    cly = ceil(posic.y/20);

    if(posic.x % 20 == 0 && posic.y % 20 == 0)
    {
        if(mapa[cly-1][flx] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.x % 20 != 0 && posic.y % 20 == 0)
    {
        if(mapa[cly-1][flx] == ' ' && mapa[cly-1][flx+1] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.x % 20 == 0 && posic.y % 20 != 0)
    {
        if(mapa[cly][flx] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else
    {
        if(mapa[cly][flx] == ' '  && mapa[cly][flx+1] == ' ')
            veredito = 'V';
        else
            veredito = 'F';
    }

    return veredito;
}

void moveJogador(Player *jogador, char mapa[][COLUNAS])
{
    //Movimento
    char veredito;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        veredito = ehPossivelDireita((*jogador).posic, mapa); // Checa se eh possivel se movimentar no eixo x
        if(veredito == 'V')
            (*jogador).posic.x+=2;   //Parenteses por que o meu ponteiro eh pra jogador somente
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        veredito = ehPossivelEsquerda((*jogador).posic, mapa);
        if(veredito == 'V')
            (*jogador).posic.x-=2;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        veredito = ehPossivelCima((*jogador).posic, mapa); // Checa se eh possivel se movimentar no eixo y
        if(veredito == 'V')
            (*jogador).posic.y-=2;
    }
    if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        veredito = ehPossivelBaixo((*jogador).posic, mapa);
        if(veredito == 'V')
            (*jogador).posic.y+=2;
    }
}

void moveInimigos(Enemy inimigo, char mapa[][COLUNAS])
{
    int i=0;
    char veredito;
    srand(time(NULL));  //(0-direita)(1-esquerda)(2-cima)(3-baixo)

   /* switch(inimigo[i].direcao)
   {
        case 0:
                veredito = ehPossivelDireita(inimigo[i].posic, mapa);
                if (veredito == 'V'){
                    inimigo[i].posic.x+=2;
                }

}*/






}


















