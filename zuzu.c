#include <stdio.h>
#include <stdlib.h>

#define CORPO 'o'
#define COMIDA '*'
#define DINHEIRO '%'
#define MORTE 'X'
#define PAREDE '#'
#define TAMANHO 101

//Structs jogo//
typedef struct {
  char Corpo;
  int x, y, xC, yC;
} tCoordenadas;

typedef struct {
  int Tamanho;
  tCoordenadas Cobra[TAMANHO];
} tCobra;

typedef struct {
  char Mapa [100][100]; 
  int Coluna;
  int Linha;
  int MapaDeCalor[100][100];
} tMapa;

typedef struct {
  int Score, GameOver;
  int Comida, Comeu;
  int Mov, W, A, S, D, TR;
  tCobra Cobra;
  tMapa Mapa;
}  tJogo;

typedef struct {
    int Ranking[10000];
    tCoordenadas PosRanking[10000];
    int Tam;
} tRank;
//Structs jogo//

//Funcoes analise//
int EhCobra (char C);
int EhComida (char C);
int EhCorpo (char C);
int EhDinheiro (char C);
int EhParede (char C);
//Funcoes analise//

//Funcoes mapa//
char PegaDirecao (tMapa Mapa);
int ScoreComida (tMapa Mapa);
tMapa CobraMorreu (tMapa Mapa);
tMapa LeMapa (tMapa Mapa, FILE * file);
void ImprimeMapa (tMapa Mapa);
//Funcoes mapa//

//Funções inicializacao//
tCobra IniciaCobra(tMapa Mapa, tCobra Cobra);
tJogo IniciaJogo(tJogo Jogo, FILE * arqv);
void IniciaMapa(tMapa Mapa, FILE * inic);
//Funcoes inicializacao//

//Funcoes coordenadas//
tCoordenadas AtualCoord (int a, int b);
tCoordenadas CoordenadaCobra (tCoordenadas k);
tCoordenadas CoordenadaAnterior (tCoordenadas k);
tCoordenadas PosicaoCobra (tMapa Mapa, tCobra Cobra);
//Funcoes coordenadas//

//Funcoes movimentacao da cobra//
tJogo AtualCabeca(tJogo Jogo, char Direcao, char Movimento);
tJogo ImprimeCobra (tJogo Jogo);
tJogo LimpaCobra (tJogo Jogo);
tJogo MoveCobra (tJogo Jogo, char Direcao, char Movimento);
tJogo VerificaCabeca (tJogo Jogo);
tJogo VerificaCorpo (tJogo Jogo);
tJogo SegueCorpo (tJogo Jogo);
//Funcoes movimentacao da cobra//

//Funcoes direcao da cobra em cada movimento//
tJogo MovW(char Movimento, tJogo Jogo);
tJogo MovA(char Movimento, tJogo Jogo);
tJogo MovS(char Movimento, tJogo Jogo);
tJogo MovD(char Movimento, tJogo Jogo);
//Funcoes direcao da cobra em cada movimento//

//Funcoes Principais//
tJogo FazMovimento(tJogo Jogo,char Movimento);
tJogo FazJogo(tJogo Jogo);
//Funcoes Principais//

//Funcoes Estado do Jogo//
void ImprimeEstadoDoJogo(tJogo Jogo, char Mov);
void ImprimirGameover(tJogo Jogo);
void ImprimirVitoria(tJogo Jogo);
//Funcoes Estado do Jogo//

//Funcoes Estatisticas//
tJogo ZeroMov(tJogo Jogo);
void GeraStatus(tJogo Jogo, FILE * stts);
//Funcoes Estatisticas//

//Funcoes Ranking//
tRank OrdenaColuna (tRank R);
tRank OrdenaLinha (tRank R);
tRank OrdenaValor (tRank R);
tRank PassaValorMapaDeCalor (tMapa Mapa, tRank R);
void GeraRanking (tJogo Jogo, FILE * rank);
//Funcoes Ranking//

//Funcoes Mapa De Calor//
tMapa CriaMapaDeCalor (tMapa Mapa);
tMapa AtualMapaDeCalor (tMapa Mapa, tCobra Cobra);
void GeraMapaDeCalor (tMapa Mapa, FILE * htmap);
//Funcoes Mapa De Calor//

//Funcoes GeraArquivo//
void ExecArqvs (tJogo Jogo, char * arqv);
//Funcoes GeraArquivo//

int main (int argc, char * argv[]) {
  FILE * arqv;
  FILE * inic;
  tJogo Jogo;

  if (argc <= 1) {
    printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
    return 1;
  }

  char Caminho[1001], Fora[1002];

  sprintf (Caminho, "%s/mapa.txt", argv[1]);
  arqv = fopen (Caminho, "r");

  sprintf(Fora, "%s/saida/inicializacao.txt", argv[1]);
  inic = fopen (Fora, "w");

  if (!arqv) {
    printf("Nao foi possivel ler o arquivo %s\n", argv[1]);
    return 1;
  }

  Jogo = IniciaJogo(Jogo, arqv);
  IniciaMapa (Jogo.Mapa, inic);
  Jogo = FazJogo(Jogo);

  ExecArqvs (Jogo, argv[1]); 
  return 0;
}

void ExecArqvs(tJogo Jogo, char * arq) {
  FILE * inic;
  FILE * heatmap;
  FILE * statik;
  FILE * rank;
  char htmap[1003], stts [1004], ranking[1005];

  sprintf(htmap, "%s/saida/heatmap.txt", arq);
  heatmap = fopen(htmap, "w");

  sprintf(stts, "%s/saida/estatisticas.txt", arq);
  statik = fopen(stts, "w");

  sprintf(ranking, "%s/saida/ranking.txt", arq);
  rank = fopen(ranking, "w");

  GeraStatus(Jogo, statik);
  GeraRanking(Jogo, rank);
  GeraMapaDeCalor(Jogo.Mapa, heatmap);
}

tJogo FazJogo(tJogo Jogo) {
    char Movimento;
    int GameOver, Victory;

    while(scanf("%c", &Movimento) == 1){
        scanf("%*[^\n]");
        scanf("%*c");

        Jogo = FazMovimento(Jogo, Movimento);

        if(Jogo.GameOver == 1){
            Jogo.Mapa = CobraMorreu(Jogo.Mapa);
            ImprimeEstadoDoJogo(Jogo, Movimento);
            GameOver = 1;
            break;
        }
        if(Jogo.Comida == 0){
            ImprimeEstadoDoJogo(Jogo, Movimento);
            Victory = 1;
            break;
        }
        ImprimeEstadoDoJogo(Jogo, Movimento);
    }

    if(Victory != 1) {
      ImprimirGameover(Jogo);
    }
    return Jogo;
}

tJogo FazMovimento(tJogo Jogo, char Movimento) {
  char Direcao;
  Direcao = PegaDirecao(Jogo.Mapa);
  Jogo = MoveCobra(Jogo, Direcao, Movimento);
  Jogo.Mapa = AtualMapaDeCalor(Jogo.Mapa, Jogo.Cobra);
  return Jogo;
}

tJogo MoveCobra(tJogo Jogo, char Direcao, char Movimento) {
  int a = 0;
  tCoordenadas k;

  for (a = 0; a < Jogo.Cobra.Tamanho; a++) {
    Jogo.Cobra.Cobra[a] = CoordenadaAnterior(Jogo.Cobra.Cobra[a]);
  }
  Jogo = AtualCabeca (Jogo, Direcao, Movimento);
  Jogo = LimpaCobra (Jogo);
  Jogo = VerificaCabeca (Jogo);
  Jogo = SegueCorpo (Jogo);
  Jogo = ImprimeCobra (Jogo);
  Jogo = VerificaCorpo (Jogo);
  Jogo.Mov++;
  return Jogo;
}

tJogo AtualCabeca(tJogo Jogo, char Direcao, char Movimento) {
  switch (Movimento) {
    case 'c': {
      Jogo = MovW (Movimento, Jogo);
      break;
    }
    case 'e': {
      Jogo = MovA (Movimento, Jogo);
      break;
    }
    case 'b': {
      Jogo = MovS (Movimento, Jogo);
      break;
    }
    case 'd': {
      Jogo = MovD (Movimento, Jogo);
      break;
    }
  }
  
  return Jogo;
}

tJogo LimpaCobra (tJogo Jogo) {
  int a, b;

  for (a = 0; a < Jogo.Mapa.Linha; a++) {
    for (b = 0; b < Jogo.Mapa.Coluna; b++) {
      if(EhCobra(Jogo.Mapa.Mapa[a][b])) {
        Jogo.Mapa.Mapa[a][b] = ' ';
      } else {
        if(EhCorpo(Jogo.Mapa.Mapa[a][b])) {
          Jogo.Mapa.Mapa[a][b] = ' ';
        }
      }
    }
  } 
  
  return Jogo;
}

tJogo VerificaCabeca(tJogo Jogo) {
  tCoordenadas k;
  int x, y;

  k = CoordenadaCobra (Jogo.Cobra.Cobra[0]);
  x = k.x;
  y = k.y;

  if (EhParede(Jogo.Mapa.Mapa[x][y])) {
    Jogo.GameOver = 1;
    Jogo.TR++;
  } else { 
      if(EhComida(Jogo.Mapa.Mapa[x][y])) {
      Jogo.Comida -= 1;
      Jogo.Score++;
      Jogo.Comeu = 1;
      Jogo.Cobra.Tamanho++;
       } else {
          if(EhDinheiro(Jogo.Mapa.Mapa[x][y])) {
          Jogo.Score += 10;
          } else {
          Jogo.TR++;
             }
        } 
    }
    Jogo.Mapa.Mapa[x][y] = Jogo.Cobra.Cobra[0].Corpo;
    return Jogo;
}

tJogo SegueCorpo(tJogo Jogo) {
  int a;

  if (Jogo.Cobra.Tamanho > 1) {
    for (a = 1; a < Jogo.Cobra.Tamanho; a++) {
      Jogo.Cobra.Cobra[a].x = Jogo.Cobra.Cobra[a-1].xC;
      Jogo.Cobra.Cobra[a].y = Jogo.Cobra.Cobra[a-1].yC;
    }
  } else {
      return Jogo;
    }
  
  return Jogo;
}
tJogo ImprimeCobra (tJogo Jogo) {
  int a;

  for (a = 0; a < Jogo.Cobra.Tamanho; a++) {
    if (a == 0) {
      Jogo.Mapa.Mapa[Jogo.Cobra.Cobra[0].x][Jogo.Cobra.Cobra[0].y] = Jogo.Cobra.Cobra[0].Corpo;
    } else {
        if (Jogo.Comeu == 1) {
            Jogo.Mapa.Mapa[Jogo.Cobra.Cobra[a].x][Jogo.Cobra.Cobra[a].y] = 'o';
        } else {
            Jogo.Mapa.Mapa[Jogo.Cobra.Cobra[a].x][Jogo.Cobra.Cobra[a].y] = 'o';
          }
      }
  }
  Jogo.Comeu = 0;

  return Jogo;
}

tJogo VerificaCorpo(tJogo Jogo) {
  tCoordenadas k;
  int x, y;

  k = CoordenadaCobra(Jogo.Cobra.Cobra[0]);
  x = k.x;
  y = k.y;

  if(EhCorpo(Jogo.Mapa.Mapa[x][y])) {
    Jogo.GameOver = 1;
  }
  
  return Jogo;
}

tJogo MovD(char Movimento, tJogo Jogo) {
  switch (Movimento) {
    case 'c': {
      Jogo.Cobra.Cobra[0].y++;
      if(Jogo.Cobra.Cobra[0].y > Jogo.Mapa.Coluna-1) {
          Jogo.Cobra.Cobra[0].y = 0;
      }
      Jogo.D++;
      break;
    }
    case 'a': {
      Jogo.Cobra.Cobra[0].x--;
      if(Jogo.Cobra.Cobra[0].x < 0) {
          Jogo.Cobra.Cobra[0].x = Jogo.Mapa.Linha-1;
      }
      Jogo.Cobra.Cobra[0].Corpo = '^';

      Jogo.W++;
      break;
    }
    case 'h': {
      Jogo.Cobra.Cobra[0].x++;
      if(Jogo.Cobra.Cobra[0].x > Jogo.Mapa.Linha-1){
          Jogo.Cobra.Cobra[0].x = 0;
      }
      Jogo.Cobra.Cobra[0].Corpo = 'v';

      Jogo.S++;
      break;
    }
  }

  return Jogo;
}

tJogo MovA(char Movimento, tJogo Jogo) {
  switch(Movimento) {
    case 'c':{
      Jogo.Cobra.Cobra[0].y--;
      if(Jogo.Cobra.Cobra[0].y < 0){
        Jogo.Cobra.Cobra[0].y = Jogo.Mapa.Coluna-1;
      }
      Jogo.A++;
      break;
    }
    case 'a': {
      Jogo.Cobra.Cobra[0].x++;
      if(Jogo.Cobra.Cobra[0].x > Jogo.Mapa.Linha-1) {
          Jogo.Cobra.Cobra[0].x = 0;
      }
      Jogo.Cobra.Cobra[0].Corpo = 'v';

      Jogo.S++;
      break;
    }
    case 'h': {
      Jogo.Cobra.Cobra[0].x--;
      if(Jogo.Cobra.Cobra[0].x < 0){
        Jogo.Cobra.Cobra[0].x = Jogo.Mapa.Linha-1;
      }
      Jogo.Cobra.Cobra[0].Corpo = '^';

      Jogo.W++;
      break;
    }
  }
  return Jogo;
}

tJogo MovW(char Movimento, tJogo Jogo) {
  switch(Movimento) {
    case 'c': {
      Jogo.Cobra.Cobra[0].x--;
      if(Jogo.Cobra.Cobra[0].x < 0){
        Jogo.Cobra.Cobra[0].x = Jogo.Mapa.Linha-1;
      }
      Jogo.W++;
      break;
    }
    case 'a': {
      Jogo.Cobra.Cobra[0].y--;
      if(Jogo.Cobra.Cobra[0].y < 0){
          Jogo.Cobra.Cobra[0].y = Jogo.Mapa.Coluna-1;
      }
      Jogo.Cobra.Cobra[0].Corpo = '<';

      Jogo.A++;
      break;
    }
    case 'h':{
      Jogo.Cobra.Cobra[0].y++;
      if(Jogo.Cobra.Cobra[0].y > Jogo.Mapa.Coluna - 1) {
        Jogo.Cobra.Cobra[0].y = 0;
      }
      Jogo.Cobra.Cobra[0].Corpo = '>';

      Jogo.D++;
      break;
    }
  }
  return Jogo;
  
}

tJogo MovS(char Movimento, tJogo Jogo) {
  switch(Movimento) {
    case 'c':{
      Jogo.Cobra.Cobra[0].x++;
      if(Jogo.Cobra.Cobra[0].x > Jogo.Mapa.Linha - 1) {
        Jogo.Cobra.Cobra[0].x = 0;
      }
      Jogo.S++;
      break;
    }
    case 'a': {
      Jogo.Cobra.Cobra[0].y++;
      if(Jogo.Cobra.Cobra[0].y < 0) {
        Jogo.Cobra.Cobra[0].y = Jogo.Mapa.Coluna - 1;
      }
      Jogo.Cobra.Cobra[0].Corpo = '>';

      Jogo.D++;
      break;
    } 
    case 'h':{
      Jogo.Cobra.Cobra[0].y--;
      if(Jogo.Cobra.Cobra[0].y > Jogo.Mapa.Coluna - 1) {
        Jogo.Cobra.Cobra[0].y = 0;
      }
      Jogo.Cobra.Cobra[0].Corpo = '<';
      Jogo.A++;
      break;
    }
  }
  return Jogo;
}

tJogo IniciaJogo(tJogo Jogo, FILE * arqv) {
  Jogo.Mapa = LeMapa (Jogo.Mapa, arqv);
  Jogo.Mapa = CriaMapaDeCalor (Jogo.Mapa);
  Jogo = ZeroMov (Jogo);
  Jogo.Score = 0;
  Jogo.GameOver = 0;
  Jogo.Comida = ScoreComida (Jogo.Mapa);
  Jogo.Cobra.Cobra[0] = PosicaoCobra(Jogo.Mapa, Jogo.Cobra);
  Jogo.Cobra = IniciaCobra(Jogo.Mapa, Jogo.Cobra);
  Jogo.Mapa = AtualMapaDeCalor(Jogo.Mapa, Jogo.Cobra);
  return Jogo;
}

tMapa LeMapa(tMapa Mapa, FILE * file) {
  int a, b;
  char Calleri;

  fscanf(file,"%d %d",&Mapa.Linha, &Mapa.Coluna);
  fscanf(file,"%*[^\n]");
  fscanf(file, "%*c");

  for(a = 0; a < Mapa.Linha; a++){
    for(b = 0; b < Mapa.Coluna; b++) {
      fscanf(file, "%c", &Calleri);
      Mapa.Mapa[a][b] = Calleri;
    }
    fscanf(file, "%*c");
  }
  fclose(file);
  return Mapa;
}

tMapa CriaMapaDeCalor(tMapa Mapa) {
  int a, b;
  for (a = 0; a < Mapa.Linha; a++){
    for (b = 0; b < Mapa.Coluna; b++) {
      Mapa.MapaDeCalor[a][b] = 0;
    }
  }
  return Mapa;
}
tJogo ZeroMov(tJogo Jogo){
  Jogo.Mov = 0;
  Jogo.TR = 0;
  Jogo.W = 0;
  Jogo.A = 0;
  Jogo.S = 0;
  Jogo.D = 0;

  return Jogo;
}

int ScoreComida (tMapa Mapa) {
  int a, b, Score = 0;

  for (a = 0; a < Mapa.Linha; a++) {
    for (b = 0; b < Mapa.Coluna; b++) {
      if (EhComida(Mapa.Mapa[a][b])) {
        Score++;
      }
    }
  }
  return Score;
}

void GeraMapaDeCalor(tMapa Mapa, FILE * htmap) {
  int a, b;

  for(a = 0; a < Mapa.Linha; a++) {
    for(b = 0; b < Mapa.Coluna; b++) {
      if(b != Mapa.Coluna - 1) {
        fprintf(htmap,"%d ", Mapa.MapaDeCalor[a][b]);
      } else {
        fprintf(htmap,"%d", Mapa.MapaDeCalor[a][b]);
      }
    }
    fprintf(htmap, "\n");
  }

  fclose(htmap);
}

void GeraStatus(tJogo Jogo, FILE * stts) {
  fprintf(stts, 
    "Numero de movimentos: %d\n"
    "Numero de movimentos sem pontuar: %d\n"
    "Numero de movimentos para baixo: %d\n"
    "Numero de movimentos para cima: %d\n"
    "Numero de movimentos para esquerda: %d\n"
    "Numero de movimentos para direita: %d\n", 
    Jogo.Mov, Jogo.TR, Jogo.S, Jogo.W, Jogo.A, Jogo.D);
  fclose(stts);
}

void GeraRanking(tJogo Jogo, FILE * rank){
  tRank R;
  int a;

  R = PassaValorMapaDeCalor(Jogo.Mapa, R);
  R = OrdenaValor(R);

  for (a = 0; a < R.Tam; a++){
    fprintf(rank, "(%d, %d) - %d\n", R.PosRanking[a].x,R.PosRanking[a].y, R.Ranking[a]);
  }
  fclose(rank);
}

tRank PassaValorMapaDeCalor(tMapa Mapa, tRank R) {
  int a, b, c = 0;
  for (a = 0; a < Mapa.Linha; a++){
    for (b = 0; b < Mapa.Coluna; b++) {
      if(Mapa.MapaDeCalor[a][b] > 0) {
        R.Ranking[c] = Mapa.MapaDeCalor [a][b];
        R.PosRanking[c].x = a;
        R.PosRanking[c].y = b;
        c++;
      }
    }
  }
  R.Tam = c;
  return R;
}

tRank OrdenaValor(tRank R) {
  int a, b, u, px, py;

  for(a = 0; a < R.Tam; a++){
    for(b = a+1; b < R.Tam; b++){
      if(R.Ranking[a] < R.Ranking[b]){
        u = R.Ranking[b];
        R.Ranking[b] = R.Ranking[a];
        R.Ranking[a] = u;

        px = R.PosRanking[a].x;
        R.PosRanking[a].y = R.PosRanking[b].y;
        R.PosRanking[b].y = py;
      }
    }
  }
  
  return R;
}
void IniciaMapa(tMapa Mapa, FILE * inic){
  int a, b, px, py;
  
  for (a = 0; a < Mapa.Linha; a++) {
    for (b = 0; a < Mapa.Coluna; b++) {
      fprintf(inic, "%c", Mapa.Mapa[a][b]);
      if(EhCobra(Mapa.Mapa[a][b])){
        px = a;
        py = b;
      }
    }
    fprintf(inic, "\n");
  }

  fprintf(inic,"A cobra comecara o jogo na linha %d e coluna %d\n", px+1, py+1);
  fclose(inic);
}

tCobra IniciaCobra(tMapa Mapa, tCobra Cobra){
  Cobra.Tamanho = 1;
  Cobra.Cobra[0] = PosicaoCobra(Mapa, Cobra);
  Cobra.Cobra[0].Corpo = '>';
  return Cobra;
}

tCoordenadas PosicaoCobra(tMapa Mapa, tCobra Cobra) {
  int a, b;

  for (a = 0; a < Mapa.Linha; a++){
    for (b = 0; b < Mapa.Coluna; b++) {
      if (EhCobra(Mapa.Mapa[a][b])) {
        Cobra.Cobra[0] = AtualCoord(a, b);
      }
    }
  }

  return Cobra.Cobra[0];
}

tCoordenadas CoordenadaAnterior(tCoordenadas k){
  k.xC = k.x;
  k.yC = k.y;
  return k;
}

tCoordenadas AtualCoord(int a, int b){
  tCoordenadas k;
  k.x = a;
  k.y = b;
  return k;
}

void ImprimirVitoria(tJogo Jogo){
  printf("Voce venceu!\n");
  printf("Pontuacao final: %d\n", Jogo.Score);
}

void ImprimirGameover(tJogo Jogo){
  printf("Game over!\n");
  printf("Pontuacao final: %d\n", Jogo.Score);
}

void ImprimeEstadoDoJogo(tJogo Jogo, char Mov) {
  printf("\nEstado do jogo apos o movimento '%c':\n", Mov);
  ImprimeMapa(Jogo.Mapa);
  printf("Pontuacao: %d\n", Jogo.Score);
}

void ImprimeMapa(tMapa Mapa){
  int a, b;
  
  for (a = 0; a < Mapa.Linha; a++) {
    for (b = 0; b < Mapa.Coluna; b++){
      printf("%c", Mapa.Mapa[a][b]);
    }
    printf("\n");
  }
}

char PegaDirecao(tMapa Mapa){
  int a, b;
  
  for (a = 0; a < Mapa.Linha; a++) {
    for (b = 0; b < Mapa.Coluna; b++) {
      if(EhCobra(Mapa.Mapa[a][b])) {
        switch(Mapa.Mapa[a][b]){
          case '>': {
            return 'd';
          }
          case '<': {
            return 'e';
          }
          case '^': {
            return 'c';
          case 'v': {
            return  'b';
            }
          }
        }
      }
    }
  }

  return 'd';
}

tMapa AtualMapaDeCalor(tMapa Mapa, tCobra Cobra){
  tCoordenadas k;

  k = CoordenadaCobra(Cobra.Cobra[0]);

  Mapa.MapaDeCalor[k.x][k.y] += 1;

  return Mapa;
}

tCoordenadas CoordenadaCobra(tCoordenadas k) {
  return k;
}

tMapa CobraMorreu(tMapa Mapa){
  int a, b;

  for (a = 0; a < Mapa.Linha; a++) {
    for (b = 0; b < Mapa.Coluna; b++) {
      if(EhCobra(Mapa.Mapa[a][b])){
        Mapa.Mapa[a][b] = MORTE;
      } else {
        if(EhCorpo(Mapa.Mapa[a][b])) {
          Mapa.Mapa[a][b] = MORTE;
        }
      }
    }
  }
  return Mapa;
}

int EhCobra(char C){
  switch(C){
    case '>':{
      return 1;
    }
    case '<':{
      return 1;
    }
    case '^':{
      return 1;
    }
    case 'v':{
      return 1;
    }
    default:
      return 0;
  }
  return 0;
}

int EhParede(char C){
    if(C == PAREDE){
        return 1;
    }
    else {
        return 0;
    }
}

int EhDinheiro(char C){
    if(C == DINHEIRO){
        return 1;
    }
    else {
        return 0;
    }
}

int EhComida(char C){
    if(C == COMIDA){
        return 1;
    }
    else {
        return 0;
    }
}

int EhCorpo(char C){
    if(C == CORPO){
        return 1;
    }
    else {
        return 0;
    }
}