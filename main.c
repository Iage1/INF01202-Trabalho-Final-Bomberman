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
    int pontuacao;
    int nChaves;
    int direcao;
} Player;

typedef struct
{
    Position posic;
    int direcao;
} Enemy;

typedef struct
{
    Position posic;
    int num;
    int estado;
    double tempo; //GetTime() é double
} Bomb;

void leMapa(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[]);

void desenhaMapa(char mapa[][COLUNAS]);

char ehPossivelDireita(Position posic, char mapa[][COLUNAS]);
char ehPossivelEsquerda(Position posic, char mapa[][COLUNAS]);
char ehPossivelCima(Position posic, char mapa[][COLUNAS]);
char ehPossivelBaixo(Position posic, char mapa[][COLUNAS]);

void moveJogador(Player *jogador, char mapa[][COLUNAS]);
void moveInimigos(Enemy inimigo[], char mapa[][COLUNAS]);
void plantaBomba(Player *jogador, char mapa[][COLUNAS], Enemy inimigo[], Bomb bomba[]);

void displayInferior(Player *jogador, Bomb bomba[]);


int main()
{

    Player jogador;
    Enemy inimigo[5];
    Bomb bomba[3];
    char mapa[LINHAS][COLUNAS];
    char nomeArq[] = "mapa01.txt";


    InitWindow(LARGURA, ALTURA, "Bomberman Crakeado");//Inicializa janela, com certo tamanho e título
    SetTargetFPS(60);//Ajusta a execução do jogo para 60 frames por segundo

    leMapa(nomeArq, mapa, &jogador, inimigo); //Le o arquivo do mapa e cria a matriz de caracteres com base no arquivo texto do mapa
    //Tambem atribui os dados iniciais para os inimigos e jogador.

    //Atribui estado 0 as bombas (inativas)
    for (int k=0; k<3; k++)
    {
        bomba[k].estado = 0;
    }

    while (!WindowShouldClose()) // Laço principal do jogo
    {
        //Desenha o mapa com base no arquivo
        desenhaMapa(mapa);

        //Movimento e ações
        moveJogador(&jogador, mapa);
        moveInimigos(inimigo, mapa);
        plantaBomba(&jogador, mapa, inimigo, bomba);

        BeginDrawing();
        ClearBackground(DARKBLUE);

        //Desenha o jogador e os inimigos
        DrawRectangle(jogador.posic.x, jogador.posic.y, LADO, LADO, DARKGREEN);
        for (int i=0; i<5; i++)
        {
            DrawRectangle(inimigo[i].posic.x, inimigo[i].posic.y, LADO, LADO, RED);
        }
        //Desenha o display inferior
        displayInferior(&jogador, bomba);
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
                    mapa[i][j] = ' '; //Fazer com que a posicao inicial dos inimigos nao fique "dura"
                }

                else if(mapa[i][j] == 'J')
                {
                    (*jogador).posic.x = j*20;
                    (*jogador).posic.y = i*20;
                    mapa[i][j] = ' '; //Fazer com que a posicao inicial do jogador nao fique "dura"
                    (*jogador).vida = 3;
                    (*jogador).pontuacao = 0;
                    (*jogador).nChaves = 0;
                    (*jogador).direcao = 0;

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
                DrawRectangle(i*20, j*20, LADO, LADO, BLACK);
            else if(mapa[j][i] == 'D')
                DrawRectangle(i*20, j*20, LADO, LADO, DARKGRAY);
            else if(mapa[j][i] == 'B' || mapa[j][i] == 'K')
                DrawRectangle(i*20, j*20, LADO, LADO, BROWN);
            else if(mapa[j][i] == 'S' || mapa[j][i] == 'S')
                DrawRectangle(i*20, j*20, LADO, LADO, ORANGE);
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

    char veredito;

    //EIXO X
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        veredito = ehPossivelDireita((*jogador).posic, mapa);
        if(veredito == 'V')
            (*jogador).posic.x+=2;   //Parenteses por que o ponteiro eh pra jogador somente
        (*jogador).direcao = 0;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        veredito = ehPossivelEsquerda((*jogador).posic, mapa);
        if(veredito == 'V')
            (*jogador).posic.x-=2;
        (*jogador).direcao = 1;
    }
    //EIXO Y
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        veredito = ehPossivelCima((*jogador).posic, mapa); // Checa se eh possivel se movimentar no eixo y
        if(veredito == 'V')
            (*jogador).posic.y-=2;
        (*jogador).direcao = 2;
    }
    if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        veredito = ehPossivelBaixo((*jogador).posic, mapa);
        if(veredito == 'V')
            (*jogador).posic.y+=2;
        (*jogador).direcao = 3;
    }

}

//Faz a mudanca de direcao ser aleatoria
void moveInimigos(Enemy inimigo[], char mapa[][COLUNAS])
{
    srand(time(NULL));

    for (int i = 0; i < 5; i++)
    {

        if (rand() % 5 == 0 )//20% de chance de mudar a direção sempre que o inimigo estiver alinhado com a matriz.
        {
            //Condicao de alinhamento comprometendo a aleatoriedade!
            inimigo[i].direcao = rand() % 4; //(0-Direita)(1-Esquerda)(2-Cima)(3-Baixo)
        }

        //switch da movimentacao dos inimigos, cada caso eh uma direcao; tem q
        switch (inimigo[i].direcao)
        {
        case 0:
            if (ehPossivelDireita(inimigo[i].posic, mapa) == 'V')
            {
                inimigo[i].posic.x += 1;
            }
            else
            {
                inimigo[i].direcao = rand() % 4; // Troca a direção se não for possível
            }
            break;
        case 1:
            if (ehPossivelEsquerda(inimigo[i].posic, mapa) == 'V')
            {
                inimigo[i].posic.x -= 1;
            }
            else
            {
                inimigo[i].direcao = rand() % 4;
            }
            break;
        case 2:
            if (ehPossivelCima(inimigo[i].posic, mapa) == 'V')
            {
                inimigo[i].posic.y -= 1;
            }
            else
            {
                inimigo[i].direcao = rand() % 4;
            }
            break;
        case 3:
            if (ehPossivelBaixo(inimigo[i].posic, mapa) == 'V')
            {
                inimigo[i].posic.y += 1;
            }
            else
            {
                inimigo[i].direcao = rand() % 4;
            }
            break;
        }
    }
}

//Funcao determina posicionamento da bomba, detona e faz alteracoes no mapa.
void plantaBomba(Player *jogador, char mapa[][COLUNAS], Enemy inimigo[], Bomb bomba[])
{
    int i = ((*jogador).posic.x);
    int j = ((*jogador).posic.y);
    int k;


    //Planta a bomba
    if (IsKeyPressed(KEY_B))
    {
        for (k=0; k<3; k++)
        {
            //Percorre as bombas até achar uma inativa (estado 0)
            if (bomba[k].estado == 0)
            {
                //Planta a bomba
                bomba[k].posic.x = i;
                bomba[k].posic.y = j;
                bomba[k].estado = 1;
                bomba[k].tempo = GetTime(); //Registra o momento em que a bomba é plantada (em s)
                mapa[bomba[k].posic.y/20][bomba[k].posic.x/20] = 'S';
                break;
            }
        }
    }

    //Explosao
    int z, d;
    for (k=0; k<3; k++)
    {
        //Percorre as bombas até achar uma ativa (estado 1)
        if (bomba[k].estado == 1)
        {
            double temp = GetTime(); //variavel pra pegar o tempo atual

            if (temp - bomba[k].tempo >= 3)  //GetTime() fica pegando o tempo atual. Quando a diferenca entre o tempo atual e o momento em que a bomba
                                             //foi plantada for igual a 3, 3 segundos se passaram e a bomba explode.
            {
                int x = bomba[k].posic.x/20;
                int y = bomba[k].posic.y/20;

                for (z=0; z<5; z++) //explosao da bomba abrange 100 pixels (5 células)
                {
                    //Direita
                    if (mapa[y][x+z] != 'W')
                    {
                        mapa[y][x+z] = ' ';
                    }
                    else   //se chegar em uma parede, o efeito da bomba não a ultrapassa
                    {
                        break;
                    }
                }
                for (z=0; z<5; z++)
                {
                    //Esquerda
                    if (mapa[y][x-z] != 'W')
                    {
                        mapa[y][x-z] = ' ';
                    }
                    else
                    {
                        break;
                    }
                }
                for (z=0; z<5; z++)
                {
                    //Cima
                    if (mapa[y-z][x] != 'W')
                    {
                        mapa[y-z][x] = ' ';
                    }
                    else
                    {
                        break;
                    }
                }
                for (z=0; z<5; z++)
                {
                    //Baixo
                    if (mapa[y+z][x] != 'W')
                    {
                        mapa[y+z][x] = ' ';
                    }
                    else
                    {
                        break;
                    }
                }
                for (d = 0; d < 5; d++)
                {
                    if ((inimigo[d].posic.x / LADO == x && inimigo[d].posic.y / LADO == y) ||
                            (inimigo[d].posic.x / LADO == x + 1 && inimigo[d].posic.y / LADO == y) ||
                            (inimigo[d].posic.x / LADO == x - 1 && inimigo[d].posic.y / LADO == y) ||
                            (inimigo[d].posic.x / LADO == x && inimigo[d].posic.y / LADO == y + 1) ||
                            (inimigo[d].posic.x / LADO == x && inimigo[d].posic.y / LADO == y - 1))
                    {
                        inimigo[d].posic.x = -1; // Remover inimigo
                        inimigo[d].posic.y = -1;
                    }
                }


                mapa [y][x] = ' ';       // Apaga a bomba
                bomba[k].estado = 0;    //Retorna a bomba ao estado 0
            }
        }
    }



}

void displayInferior(Player *jogador, Bomb bomba[])
{
    char displayVida[20] = "Vidas: 0";
    char displayBomba[20] = "Bombas: 0";
    char displayPontuacao[20] = "Pontuacao: 0";

    //Como fazer pra imprimir os valores?
    (*jogador).vida = 1;
    // vida[7] = printf("%d", (*jogador).vida);

    DrawText(displayVida, 20, 525, 40, WHITE);
    DrawText(displayBomba, 375, 525, 40, WHITE);
    DrawText(displayPontuacao, 750, 525, 40, WHITE);
}
