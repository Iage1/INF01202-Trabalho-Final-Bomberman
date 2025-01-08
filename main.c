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
#define MAXVIDA 3
#define MAXBOMBA 3
#define NINIMIGOS 5
#define TAMTEXTO 40


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
    double tempo; //GetTime() é double
    int estadoExplosao;
    double tempoExplosao;
} Bomb;

typedef struct{
    Player jogador;
    Enemy inimigo;
    Bomb bomba;
    char mapa[LINHAS][COLUNAS];
    char nomeArq;
}Save;

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

void chave(Player *jogador, char mapa[][COLUNAS]);

void displayInferior(Player *jogador, Bomb bomba[]);

void menu(int *pausa, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], char *nome_arq, Save *optimus);
void novoJogo(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], int *pausa);
void saveGame(char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], char *nome_arq, Save *optimus);




int main()
{

    Player jogador;
    Enemy inimigo[NINIMIGOS];
    Bomb bomba[MAXBOMBA];
    char mapa[LINHAS][COLUNAS];
    char nomeArq[] = "Mapa01.txt";
    int pausa = 0;
    Save optimus;


    InitWindow(LARGURA, ALTURA, "Bomberman");//Inicializa janela, com certo tamanho e título
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
        if (IsKeyPressed(KEY_TAB))
            pausa = 1;

        if(pausa == 0){
        //Desenha o mapa com base no arquivo
        desenhaMapa(mapa);

        //Movimento e mecanicas
        moveJogador(&jogador, mapa);
        moveInimigos(inimigo, mapa, &jogador);
        plantaBomba(&jogador, mapa, inimigo, bomba);
        chave(&jogador, mapa);
        }

        BeginDrawing();
        ClearBackground(DARKBLUE);

        //Desenha o jogador e os inimigos
        DrawRectangle(jogador.posic.x, jogador.posic.y, LADO, LADO, DARKGREEN);
        for (int i=0; i<NINIMIGOS; i++)
        {
            DrawRectangle(inimigo[i].posic.x, inimigo[i].posic.y, LADO, LADO, RED);
        }
        //Desenha a explosao das bombas
        desenhaExplosao(bomba, mapa);
        //Desenha o display inferior
        displayInferior(&jogador, bomba);
        if(pausa == 1)
            //double tempoPausa = GetTime(); //Variavel pra impedir que a bomba contabilize o tempo dentro do pause
            menu(&pausa, mapa, &jogador, inimigo, bomba, nomeArq, &optimus);
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
                    inimigo[d].posic.x = j*LADO;
                    inimigo[d].posic.y = i*LADO;
                    inimigo[d].direcao = rand()%4;
                    d++;
                    mapa[i][j] = ' '; //Fazer com que a posicao inicial dos inimigos nao fique "dura"
                }

                else if(mapa[i][j] == 'J')
                {
                    jogador->posic.x = j*LADO;
                    jogador->posic.y = i*LADO;
                    mapa[i][j] = ' '; //Fazer com que a posicao inicial do jogador nao fique "dura"
                    jogador->vida = MAXVIDA;
                    jogador->pontuacao = 0;
                    jogador->nChaves = 0;
                    jogador->nBombas = MAXBOMBA;
                    jogador->direcao = 0;
                    jogador->delay = 0;

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
                DrawRectangle(i*LADO, j*LADO, LADO, LADO, BLACK);
            else if(mapa[j][i] == 'D')
                DrawRectangle(i*LADO, j*LADO, LADO, LADO, DARKGRAY);
            else if(mapa[j][i] == 'B' || mapa[j][i] == 'K')
                DrawRectangle(i*LADO, j*LADO, LADO, LADO, BROWN);
            else if(mapa[j][i] == 'S')
                DrawRectangle(i*LADO, j*LADO, LADO, LADO, ORANGE);
            else if (mapa[j][i] == 'C')
                DrawRectangle(i*LADO, j*LADO, LADO, LADO, GOLD);
        }
    }
}

//Foi preciso trocar linhas por colunas
char ehPossivelDireita(Position posic, char mapa[][COLUNAS])
{

    int flx, fly;
    char veredito;

    flx = floor(posic.x/LADO);
    fly = floor(posic.y/LADO);

    if(posic.y % LADO == 0)
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

    clx = ceil(posic.x/LADO);
    fly = floor(posic.y/LADO);

    if(posic.y % LADO == 0 && posic.x % LADO == 0)
    {
        if(mapa[fly][clx-1] == ' ' || mapa[fly][clx-1] == 'C')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.y % LADO != 0 && posic.x % LADO == 0)
    {
        if((mapa[fly][clx-1] == ' ' || mapa[fly][clx-1] == 'C') && (mapa[fly+1][clx-1] == ' ' || mapa[fly+1][clx-1] == 'C'))
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.y % LADO == 0 && posic.x % LADO != 0)
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

    flx = floor(posic.x/LADO);
    fly = floor(posic.y/LADO);

    if(posic.x % LADO == 0)
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

    flx = floor(posic.x/LADO);
    cly = ceil(posic.y/LADO);

    if(posic.x % LADO == 0 && posic.y % LADO == 0)
    {
        if(mapa[cly-1][flx] == ' ' || mapa[cly-1][flx] == 'C')
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.x % LADO != 0 && posic.y % LADO == 0)
    {
        if((mapa[cly-1][flx] == ' ' || mapa[cly-1][flx] == 'C') && (mapa[cly-1][flx+1] == ' ' || mapa[cly-1][flx+1] == 'C'))
            veredito = 'V';
        else
            veredito = 'F';
    }
    else if(posic.x % LADO == 0 && posic.y % LADO != 0)
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
        veredito = ehPossivelDireita(jogador->posic, mapa);
        if(veredito == 'V')
            jogador->posic.x+=2;   //Parenteses por que o ponteiro eh pra jogador somente
        jogador->direcao = 0;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        veredito = ehPossivelEsquerda(jogador->posic, mapa);
        if(veredito == 'V')
            jogador->posic.x-=2;
        jogador->direcao = 1;
    }
    //EIXO Y
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        veredito = ehPossivelCima(jogador->posic, mapa); // Checa se eh possivel se movimentar no eixo y
        if(veredito == 'V')
            jogador->posic.y-=2;
        jogador->direcao = 2;
    }
    if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        veredito = ehPossivelBaixo(jogador->posic, mapa);
        if(veredito == 'V')
            jogador->posic.y+=2;
        jogador->direcao = 3;
    }

}

//Faz a mudanca de direcao dos inimigos ser aleatoria, e deteccao de jogadores pra tirar vida
void moveInimigos(Enemy inimigo[], char mapa[][COLUNAS], Player *jogador)
{
    srand(time(NULL));

    for (int i = 0; i < NINIMIGOS; i++)
    {
        //if (inimigo[i].posic.x % LADO == 0 && inimigo[i].posic.y % LADO == 0)
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

    //Deteccao dos jogadores
    Rectangle retanguloJogador = {jogador->posic.x, jogador->posic.y, LADO, LADO};

    //Variavel de tempo para controlar as vidas perdidas pelo jogador

    for (int i=0; i<NINIMIGOS; i++)
    {
        Rectangle retanguloInimigo = {inimigo[i].posic.x, inimigo[i].posic.y, LADO, LADO};
        if (CheckCollisionRecs(retanguloJogador, retanguloInimigo) == 1)
        {
            if (GetTime() - jogador->delay >= 1.5)  //delay atua como o tempo da ultima vez que o jogador perdeu vida
            {
                jogador->vida--;
                jogador->delay = GetTime();

            }

        }
    }

}


//Função principal para manipular bombas
void plantaBomba(Player *jogador, char mapa[][COLUNAS], Enemy inimigo[], Bomb bomba[])
{
    if (IsKeyPressed(KEY_B))
    {
        for (int k = 0; k < MAXBOMBA; k++)
        {
            if (bomba[k].estadoAtiva == 0)   //Percorre as bombas até encontrar uma inativa
            {
                bomba[k].posic.x = jogador->posic.x;
                bomba[k].posic.y = jogador->posic.y;
                bomba[k].estadoAtiva = 1;
                jogador->nBombas--;
                bomba[k].tempo = GetTime();  //Registra o tempo em que a bomba foi plantada
                mapa[bomba[k].posic.y / LADO][bomba[k].posic.x / LADO] = 'S';
                break;
            }
        }
    }


    for (int k = 0; k < MAXBOMBA; k++)
    {
        if (bomba[k].estadoAtiva == 1)  //Percorre as bombas até achar uma ativa
        {

            double tempAtual = GetTime();  //Pega o tempo atual
            if (tempAtual - bomba[k].tempo >= 3)   //Quando o tempoAtual passa em 3 segundos o tempo em que a bomba foi plantada, explode
            {
                int x = bomba[k].posic.x/LADO;
                int y = bomba[k].posic.y/LADO;

                int seguroVida, seguroPontuacao;

                seguroVida = (*jogador).vida;
                seguroPontuacao = (*jogador).pontuacao;
                propagaExplosao(mapa, inimigo, x, y, 1, 0, jogador); // Direita (x+1)      /*
                propagaExplosao(mapa, inimigo, x, y, -1, 0, jogador); // Esquerda (x-1)                 PORQUE JOGADOR NAO LEVA &????
                propagaExplosao(mapa, inimigo, x, y, 0, -1, jogador); // Cima (y-1)
                propagaExplosao(mapa, inimigo, x, y, 0, 1, jogador); // Baixo (y+1)                                                     */

                //Mecanismo pro jogador nao sofrer mais de uma perda de vida ou de pontuacao por explosao (pixel do meio pode dar 4 de dano)
                if (seguroVida - (*jogador).vida > 1)
                {
                    (*jogador).vida = seguroVida - 1;
                    (*jogador).pontuacao = seguroPontuacao;
                }

                mapa[y][x] = ' '; // Remove a bomba do mapa

                //Inicia as variaveis necessarias para a execucao da funcao desenhaExplosao
                bomba[k].estadoExplosao = 1;
                bomba[k].tempoExplosao = GetTime(); //Registra o tempo do comeco da explosao
                bomba[k].estadoAtiva = 0;
                jogador->nBombas++;
            }
        }
    }
}


//Propaga a explosao em uma direcao, fazendo as alteracoes no mapa
void propagaExplosao(char mapa[][COLUNAS], Enemy inimigo[], int x, int y, int direcx, int direcy, Player *jogador)
{
    for (int z = 0; z < MAXBOMBA; z++)   //Bomba com alcance de 100 pixels -> 3 pixels pra todas as direcoes (incluindo o do meio)
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
            jogador->pontuacao += 10;
            break;
        }
        else if (mapa[ny][nx] == 'K')
        {
            mapa[ny][nx] = 'C';   //Condicao especial para as chaves
            jogador->pontuacao += 10;
            break;
        }

        //Para a deteccao de inimigos, pra elimina-los mesmo com um pixel encostando na explosao, uso da funcao CheckCollisionRecs, do raylib

        Rectangle retanguloExplosao = {nx*LADO, ny*LADO, LADO, LADO}; //Retangulo da explosao: mesmos elementos do DrawRectangle()

        //Deteccao dos inimigos
        for (int i = 0; i < NINIMIGOS; i++)
        {
            Rectangle retanguloInimigo = {inimigo[i].posic.x, inimigo[i].posic.y, LADO, LADO}; //Retangulo pra cada inimigo

            if (CheckCollisionRecs(retanguloExplosao, retanguloInimigo) == 1) //Checa se há colisao entre dois retangulos. Funcao bool.
            {
                inimigo[i].posic.x = -20; //Joga o inimigo pra fora do mapa. -20 ele ja nao fica visivel.
                inimigo[i].posic.y = -20;
                jogador->pontuacao += 20;
            }
        }
        //Deteccao do prprio jogador
        Rectangle retanguloJogador = {jogador->posic.x, jogador->posic.y, LADO, LADO};
        if (CheckCollisionRecs(retanguloExplosao, retanguloJogador) == 1)
        {
            jogador->vida--;
            jogador->pontuacao -= 100;
        }

    }
}


//Funcao para desenhar a explosão
void desenhaExplosao(Bomb bomba[], char mapa[][COLUNAS])
{
    for (int k = 0; k < MAXBOMBA; k++)
    {
        if (bomba[k].estadoExplosao == 1)
        {
            double duracaoExplosao = GetTime() - bomba[k].tempoExplosao;
            int x = bomba[k].posic.x/LADO;
            int y = bomba[k].posic.y/LADO;

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

//Funcao para desenhar cada direcao da explosao
void desenhaPropagacao(char mapa[][COLUNAS], int x, int y, int direcx, int direcy)
{
    for (int z = 0; z < MAXBOMBA; z++)
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
            DrawRectangle(nx*LADO, ny*LADO, LADO, LADO, MAGENTA); //Espaco vazio continua a propagacao do desenho
        }
    }
}

void displayInferior(Player *jogador, Bomb bomba[])
{
    char displayVida[20];
    char displayBomba[20];
    char displayPontuacao[30];

    //Funcao sprintf() guarda o conteudo de um printf em uma string
    sprintf(displayVida, "Vidas: %d", jogador->vida);
    sprintf(displayBomba, "Bombas: %d", jogador->nBombas);
    sprintf(displayPontuacao, "Pontuacao: %d", jogador->pontuacao);

    DrawText(displayVida, 30, 525, TAMTEXTO, WHITE);
    DrawText(displayBomba, 375, 525, TAMTEXTO, WHITE);
    DrawText(displayPontuacao, 750, 525, TAMTEXTO, WHITE);
    DrawRectangle(0, 575, 1200, 30, BLACK);
    DrawRectangle(0, 200, 20, 400, BLACK);
    DrawRectangle(1180, 200, 20, 400, BLACK);
}

void chave(Player *jogador, char mapa[][COLUNAS])
{
    int x = jogador->posic.x/LADO;
    int y = jogador->posic.y/LADO;

    if (mapa[y][x] == 'C')
    {
        mapa[y][x] = ' ';
        jogador->nChaves++;
        printf("%d", jogador->nChaves);
    }

    //Carrega procimo mapa
    if (jogador->nChaves == 5)
    {

    }
}

void menu(int *pausa, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], char *nome_arq, Save *optimus)
{
    char opcao;

    desenhaMapa(mapa);
    DrawRectangle(400, 100, 400, 208, LIGHTGRAY);
    DrawText("Novo Jogo (N)", 402, 102, TAMTEXTO, WHITE);
    DrawText("Carregar Jogo (C)", 402, 144, TAMTEXTO, WHITE);
    DrawText("Salvar Jogo (S)", 402, 186, TAMTEXTO, WHITE);
    DrawText("Sair do Jogo (Q)", 402, 228, TAMTEXTO, WHITE);
    DrawText("Voltar (V)", 402, 270, TAMTEXTO, WHITE);


        if(IsKeyPressed(KEY_N))
            opcao = 'N';
        /*if(IsKeyPressed(KEY_C))
            opcao = 'C';*/
        if(IsKeyPressed(KEY_S))
            opcao = 'S';
        if(IsKeyPressed(KEY_Q))
            opcao = 'Q';
        if (IsKeyPressed(KEY_V))
            opcao = 'V';

        switch(opcao){
        case 'N':
            novoJogo(nome_arq, mapa, jogador, inimigo, bomba, pausa);// Reseta todas as informações para como estão no arquivo, ou seja, como eram originalmente.
            *pausa = 0;
            break;
        /*case 'C':
            loadGame();
            break;*/
        case 'S':
            saveGame(mapa, jogador, inimigo, bomba, nome_arq, optimus);
            *pausa = 0;
            break;
        case 'Q':
            CloseWindow();
            break;
        case 'V':
            *pausa = 0;
            break;
        }

}

void novoJogo(char *nome_arq, char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], int *pausa)
{
    leMapa(nome_arq, mapa, jogador, inimigo);
    for (int i = 0; i<MAXBOMBA; i++) //Loop pra desativar bombas ativas
    {
        bomba[i].estadoAtiva = 0;
        bomba[i].estadoExplosao = 0;
        bomba[i].tempo = 0;
        bomba[i].tempoExplosao = 0;
    }
    *pausa = 0;
}

void saveGame(char mapa[][COLUNAS], Player *jogador, Enemy inimigo[], Bomb bomba[], char *nome_arq, Save *optimus)
{

    char nomeArqB[50];
    int contador = 1;
    FILE *arq;

    sprintf(nomeArqB, "Save%d", contador);

    arq = fopen(nomeArqB, "wb");
    if(arq == NULL)
        printf("Erro ao gerar arquivo salvo");
    else{
        fwrite(&optimus, sizeof(Save), 1, arq);
        if(ferror(arq))
            printf("Erro ao gerar/escrever no arquivo binario");
        fclose(arq);
    }


}
