#include "bomberman.h"

int main()
{
    Player jogador;
    Enemy inimigo[NINIMIGOS];
    Bomb bomba[MAXBOMBA];
    char mapa[LINHAS][COLUNAS];
    char nomeArq[] = "Mapa1.txt";
    int pausa = 0;
    Save optimus;

    srand(time(NULL));
    
    InitWindow(LARGURA, ALTURA, "Bomberman");//Inicializa janela, com certo tamanho e t�tulo
    SetTargetFPS(60);//Ajusta a execu��o do jogo para 60 frames por segundo

    leMapa(nomeArq, mapa, &jogador, inimigo); //Le o arquivo do mapa e cria a matriz de caracteres com base no arquivo texto do mapa
    //Tambem atribui os dados iniciais para os inimigos e jogador.

    //Atribui estado 0 as bombas (inativas)
    for (int k=0; k<3; k++)
    {
        bomba[k].estadoAtiva = 0;
    }

    while (!WindowShouldClose()) // La�o principal do jogo
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
        chave(&jogador, mapa, inimigo, bomba, &pausa);
        morte(nomeArq, mapa, &jogador, inimigo, bomba, &pausa);
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

