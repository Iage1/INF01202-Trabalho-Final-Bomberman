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
    int nBombas;
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
    int estadoAtiva;
    double tempo; //GetTime() é double
    int estadoExplosao;
    double tempoExplosao;
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
void propagaExplosao(char mapa[][COLUNAS], Enemy inimigo[], int x, int y, int direcx, int direcy);
void desenhaExplosao(Bomb bomba[], char mapa[][COLUNAS]);
void desenhaPropagacao(char mapa[][COLUNAS], int x, int y, int direcx, int direcy);

void chave(Player *jogador, char mapa[][COLUNAS]);

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
        bomba[k].estadoAtiva = 0;
    }

    while (!WindowShouldClose()) // Laço principal do jogo
    {
        //Desenha o mapa com base no arquivo
        desenhaMapa(mapa);

        //Movimento e mecanicas
        moveJogador(&jogador, mapa);
        moveInimigos(inimigo, mapa);
        plantaBomba(&jogador, mapa, inimigo, bomba);
        chave(&jogador, mapa);



        BeginDrawing();
        ClearBackground(DARKBLUE);

        //Desenha o jogador e os inimigos
        DrawRectangle(jogador.posic.x, jogador.posic.y, LADO, LADO, DARKGREEN);
        for (int i=0; i<5; i++)
        {
            DrawRectangle(inimigo[i].posic.x, inimigo[i].posic.y, LADO, LADO, RED);
        }
        //Desenha a explosao das bombas
        desenhaExplosao(bomba, mapa);
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
                    (*jogador).nBombas = 3;
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
            else if(mapa[j][i] == 'S')
                DrawRectangle(i*20, j*20, LADO, LADO, ORANGE);
            else if (mapa[j][i] == 'C')
                DrawRectangle(i*20, j*20, LADO, LADO, GOLD);
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
        if(mapa[fly][flx+1] == ' ' || mapa[fly][flx+1] == 'C')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else
    {
        if((mapa[fly][flx+1] == ' ' || mapa[fly][flx+1] == 'C') && (mapa[fly+1][flx+1] == ' ' || mapa[fly+1][flx+1] == 'C'))
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
        if(mapa[fly][clx-1] == ' ' || mapa[fly][clx-1] == 'C')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.y % 20 != 0 && posic.x % 20 == 0)
    {
        if((mapa[fly][clx-1] == ' ' || mapa[fly][clx-1] == 'C') && (mapa[fly+1][clx-1] == ' ' || mapa[fly+1][clx-1] == 'C'))
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.y % 20 == 0 && posic.x % 20 != 0)
    {
        if(mapa[fly][clx] == ' ' || mapa[fly][clx] == 'C')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else
    {
        if((mapa[fly][clx] == ' ' || mapa[fly][clx] == 'C') && (mapa[fly+1][clx] == ' ' || mapa[fly+1][clx] == 'C'))
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
        if(mapa[fly+1][flx] == ' ' || mapa[fly+1][flx] == 'C')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else
    {
        if((mapa[fly+1][flx] == ' ' || mapa[fly+1][flx] == 'C') && (mapa[fly+1][flx+1] == ' ' || mapa[fly+1][flx+1] == 'C'))
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
        if(mapa[cly-1][flx] == ' ' || mapa[cly-1][flx] == 'C')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.x % 20 != 0 && posic.y % 20 == 0)
    {
        if((mapa[cly-1][flx] == ' ' || mapa[cly-1][flx] == 'C') && (mapa[cly-1][flx+1] == ' ' || mapa[cly-1][flx+1] == 'C'))
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.x % 20 == 0 && posic.y % 20 != 0)
    {
        if(mapa[cly][flx] == ' ' || mapa[cly][flx] == 'C')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else
    {
        if((mapa[cly][flx] == ' ' || mapa[cly][flx] == 'C')  && (mapa[cly][flx+1] == ' ' || mapa[cly][flx+1] == 'C'))
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
        if (inimigo[i].posic.x % LADO == 0 && inimigo[i].posic.y % LADO == 0)
        {
            if (rand() % 5 == 0 )//20% de chance de mudar a direção sempre que o inimigo estiver alinhado com a matriz.
            {
                //Condicao de alinhamento comprometendo a aleatoriedade!
                inimigo[i].direcao = rand() % 4; //(0-Direita)(1-Esquerda)(2-Cima)(3-Baixo)
            }
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


// Função principal para manipular bombas
void plantaBomba(Player *jogador, char mapa[][COLUNAS], Enemy inimigo[], Bomb bomba[])
{
    if (IsKeyPressed(KEY_B))
    {
        for (int k = 0; k < 3; k++)
        {
            if (bomba[k].estadoAtiva == 0)   //Percorre as bombas até encontrar uma inativa
            {
                bomba[k].posic.x = (*jogador).posic.x;
                bomba[k].posic.y = (*jogador).posic.y;
                bomba[k].estadoAtiva = 1;
                (*jogador).nBombas--;
                bomba[k].tempo = GetTime();  //Registra o tempo em que a bomba foi plantada
                mapa[bomba[k].posic.y / 20][bomba[k].posic.x / 20] = 'S';
                break;
            }
        }
    }


    for (int k = 0; k < 3; k++)
    {
        if (bomba[k].estadoAtiva == 1)  //Percorre as bombas até achar uma ativa
        {
            double tempAtual = GetTime();  //Pega o tempo atual
            if (tempAtual - bomba[k].tempo >= 3)   //Quando o tempoAtual passa em 3 segundos o tempo em que a bomba foi plantada, explode
            {
                int x = bomba[k].posic.x/20;
                int y = bomba[k].posic.y/20;

                propagaExplosao(mapa, inimigo, x, y, 1, 0); // Direita (x+1)
                propagaExplosao(mapa, inimigo, x, y, -1, 0); // Esquerda (x-1)
                propagaExplosao(mapa, inimigo, x, y, 0, -1); // Cima (y-1)
                propagaExplosao(mapa, inimigo, x, y, 0, 1); // Baixo (y+1)

                mapa[y][x] = ' '; // Remove a bomba do mapa

                //Inicia as variaveis necessarias para a execucao da funcao desenhaExplosao
                bomba[k].estadoExplosao = 1;
                bomba[k].tempoExplosao = GetTime(); //Registra o tempo do comeco da explosao
                bomba[k].estadoAtiva = 0;
                (*jogador).nBombas++;
            }
        }
    }
}


// Função para propagar a explosão em uma direcao
void propagaExplosao(char mapa[][COLUNAS], Enemy inimigo[], int x, int y, int direcx, int direcy)
{
    for (int z = 0; z < 3; z++)   //Bomba com alcance de 100 pixels -> 3 pixels pra todas as direcoes (incluindo o do meio)
    {
        //Celulas correspondentes do mapa que vao sendo incrementadas dentro do loop
        int nx = x + (z*direcx);
        int ny = y + (z*direcy);

        if (mapa[ny][nx] == 'W')
        {
             break;    //Nao permite a propagacao apos bater em uma parede
        }
        else if (mapa[ny][nx] == 'D' || mapa[ny][nx] == 'B')
        {
            mapa[ny][nx] = ' ';        //Se a bomba enconstar em um objeto, quebra ele e para a propagacao
            break;
        }
        else if (mapa[ny][nx] == 'K')
        {
            mapa[ny][nx] = 'C';   //Condicao especial para as chaves
            break;
        }

        // Atualiza inimigos atingidos
        for (int i = 0; i < 5; i++)
        {
            if (inimigo[i].posic.x/20 == nx && inimigo[i].posic.y/20 == ny) //Se o inimigo estiver dentro das coordenadas de propagacao
            {
                inimigo[i].posic.x = -1; //Joga o inimigo pra fora do mapa
                inimigo[i].posic.y = -1;
            }
        }
    }
}


// Função para desenhar a explosão
void desenhaExplosao(Bomb bomba[], char mapa[][COLUNAS])
{
    for (int k = 0; k < 3; k++)
    {
        if (bomba[k].estadoExplosao == 1)
        {
            double duracaoExplosao = GetTime() - bomba[k].tempoExplosao;
            int x = bomba[k].posic.x/20;
            int y = bomba[k].posic.y/20;

            // Propaga o desenho da explosão em todas as direções
            desenhaPropagacao(mapa, x, y, 1, 0); // Direita
            desenhaPropagacao(mapa, x, y, -1, 0); // Esquerda
            desenhaPropagacao(mapa, x, y, 0, -1); // Cima
            desenhaPropagacao(mapa, x, y, 0, 1); // Baixo

            if (duracaoExplosao >= 0.3)
            {
                bomba[k].estadoExplosao = 0; // Termina a explosão
            }
        }
    }
}

// Funcao para desenhar cada direcao da explosao
void desenhaPropagacao(char mapa[][COLUNAS], int x, int y, int direcx, int direcy)
{
    for (int z = 0; z < 3; z++)
    {
        int nx = x + (z*direcx);
        int ny = y + (z*direcy);

        if (mapa[ny][nx] == 'W')
        {
            break; //Se bate em uma parede, nao continua o desenho
        }
        else if (mapa[ny][nx] == 'D' || mapa[ny][nx] == 'B' || mapa[ny][nx] == 'K')
        {
            //DrawRectangle(nx*20, ny*20, 20, 20, MAGENTA);   //Se bate em um objeto, quebra ele e para o desenho
            break;                                              //Por algum motivo, nao funciona quando a proxima celula eh um espaco vazio
        }
        else
        {
            DrawRectangle(nx*20, ny*20, 20, 20, MAGENTA); //Espaco vazio continua a propagacao do desenho
        }
    }
}

void displayInferior(Player *jogador, Bomb bomba[])
{
    char displayVida[20] = "Vidas: 0";
    char displayBomba[20] = "Bombas: 0";
    char displayPontuacao[20] = "Pontuacao: 0";

    printf("%d\n",(*jogador).nBombas);

    //Como fazer pra imprimir os valores?
    (*jogador).vida = 1;
    // vida[7] = printf("%d", (*jogador).vida);

    DrawText(displayVida, 20, 525, 40, WHITE);
    DrawText(displayBomba, 375, 525, 40, WHITE);
    DrawText(displayPontuacao, 750, 525, 40, WHITE);
}


void chave(Player *jogador, char mapa[][COLUNAS])
{
    int x = (*jogador).posic.x/20;
    int y = (*jogador).posic.y/20;

    if (mapa[y][x] == 'C')
    {
        mapa[y][x] = ' ';
        (*jogador).nChaves++;
        printf("%d", (*jogador).nChaves);
    }

    if ((*jogador).nChaves == 5)
    {

    }
}
