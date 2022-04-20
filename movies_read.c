#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define M 10000
#define F 3

int numPart = 1;

typedef struct stFilm{
  char titleType[13], primaryTitle[422], originalTitle[422];
  unsigned char isAdult;
  unsigned short int startYear, endYear;
  char runtimeMinutes[6], genres[33];
}tpFilm;

void imprimirFilmes(char *arq, int qtde)
{
    FILE *arquivo = fopen(arq, "rb");
    tpFilm film;
    int estrutura = sizeof(tpFilm);

    for(int i = 1; i <= qtde; i++){
        assert(fread(&(film), estrutura, 1, arquivo));
        printf("\n");
        printf("\033[1;31mFilme Número: %i\033[0m\n", i);
        printf("\033[1;38m%s\033[0m\n", film.titleType);
        printf("\033[1;38m%s\033[0m\n", film.primaryTitle);
        printf("\033[1;38m%s\033[0m\n", film.originalTitle);
        printf("\033[1;38m%u\033[0m\n", film.isAdult);
        printf("\033[1;38m%u\033[0m\n", film.startYear);
        printf("\033[1;38m%u\033[0m\n", film.endYear);
        printf("\033[1;38m%s\033[0m\n", film.runtimeMinutes);
        printf("\033[1;38m%s\033[0m\n", film.genres);
    }

    fclose(arquivo);
}

void arquivoInfo(char *arq){
    FILE *arquivo = fopen(arq, "rb");
    tpFilm film;
    int estrutura = sizeof(tpFilm);
    unsigned long long int tamanho = 0;
    assert(arquivo != NULL);

    fseek(arquivo, 0L, SEEK_END);
    tamanho = ftell(arquivo);
    fseek(arquivo, 0L, SEEK_SET);
    
    printf("\n\033[1;33mInformações sobre o arquivo inicial:\033[0m\n");
    printf("\033[1;32mTamanho da estrutura dos filmes:\033[0m \033[1;31m%d\033[0m \033[1;34mbytes.\033[0m\n", estrutura);
    printf("\033[1;32mTamanho total do arquivo: \033[1;31m%lld\033[0m \033[1;34mbytes.\033[0m\n", tamanho);
    printf("\033[1;32mTotal de filmes registrados: \033[1;31m%lld \033[1;34mfilmes.\033[0m\n", (tamanho / estrutura));

    fclose(arquivo);
}

int menorValor(tpFilm *memoria){

    tpFilm menor = memoria[0];
    int indiceMenor = 0;

    for(int i = 1; i < M; i++)
    {
        if( strcmp(memoria[i].originalTitle, menor.originalTitle) < 0)
        {
            indiceMenor = i;
            menor = memoria[i];
        }
    }

    return indiceMenor;
}

void salvarMemoriaParticao(tpFilm *memoria, FILE *particao, int usado){
   tpFilm aux;
   for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if(i != usado && j != usado)
            {
                if (strcmp(memoria[i].originalTitle, memoria[j].originalTitle) < 0)
                {
                    aux = memoria[i];
                    memoria[i] = memoria[j];
                    memoria[j] = aux;
                }
            }
        }
    }

    for (int i = 0; i < M; i++)
    {
        if(i != usado)
        {
            assert(fwrite(&memoria[i], sizeof(tpFilm), 1, particao) > 0);
        }
    }
}

void acesso_Direto(char *arq, long int reg){
  FILE *arquivo = fopen(arq, "rb");
  int tam = sizeof(tpFilm);
  long long int pos = reg * tam; 
  tpFilm film;
  

  fseek(arquivo, pos, SEEK_SET);
  assert(fread(&(film), tam, 1, arquivo) > 0);

  printf("\n\033[1;31mRegistrador: %i\033[0m\n", reg);
  printf("\033[1;38m%s\033[0m\n", film.titleType);
  printf("\033[1;38m%s\033[0m\n", film.primaryTitle);
  printf("\033[1;38m%s\033[0m\n", film.originalTitle);
  printf("\033[1;38m%u\033[0m\n", film.isAdult);
  printf("\033[1;38m%u\033[0m\n", film.startYear);
  printf("\033[1;38m%u\033[0m\n", film.endYear);
  printf("\033[1;38m%s\033[0m\n", film.runtimeMinutes);
  printf("\033[1;38m%s\033[0m\n\n", film.genres);

  fclose(arquivo);
}

void classificacao(){
    
    // Atribuitos iniciais
    FILE *arquivo = fopen("films.dat", "rb");
    FILE *reservatorio = fopen("reservatorio.dat", "wb+");
    
    static tpFilm memoria[M];
    tpFilm menor;
    int indiceMenor;
    int indiceReservatorio = 0;

    FILE *particao;
    int indice = 1;
    char nome[100];
    sprintf(nome, "part-%.5d.dat", indice);
    particao = fopen(nome,"wb");

    assert(arquivo != NULL);
    assert(reservatorio != NULL);
    assert(particao != NULL);

    assert(fread(&(memoria), sizeof(tpFilm), M, arquivo) > 0);
    indiceMenor = menorValor(memoria);
    menor = memoria[indiceMenor];
    assert(fwrite(&memoria[indiceMenor], sizeof(tpFilm), 1, particao) > 0);

    // Loop responsável pelo gerenciamento da memória, reservatório, e criação de novas partições até terminar o arquivo.
    while(1){

        if(fread(&(memoria[indiceMenor]), sizeof(tpFilm), 1, arquivo) > 0)
        {
            if(strcmp(memoria[indiceMenor].originalTitle, menor.originalTitle) < 0)
            {
                assert(fwrite(&memoria[indiceMenor], sizeof(tpFilm), 1, reservatorio) > 0);
                indiceReservatorio++;
                if(indiceReservatorio == M){
                    salvarMemoriaParticao(memoria, particao, indiceMenor);
                    fclose(particao);

                    sprintf(nome, "part-%.5d.dat", ++indice);
                    particao = fopen(nome,"wb");
                    numPart++;

                    fflush(reservatorio);
                    fseek(reservatorio, 0L, SEEK_SET);
                    assert(fread(&(memoria), sizeof(tpFilm), M, reservatorio) > 0);
                    fseek(reservatorio, 0L, SEEK_SET);

                    indiceMenor = menorValor(memoria);
                    menor = memoria[indiceMenor];
                    assert(fwrite(&memoria[indiceMenor], sizeof(tpFilm), 1, particao) > 0);

                    indiceReservatorio = 0;
                }
            } 
            else 
            {
                indiceMenor = menorValor(memoria);
                menor = memoria[indiceMenor];
                assert(fwrite(&memoria[indiceMenor], sizeof(tpFilm), 1, particao) > 0);
            }
        } else {

            salvarMemoriaParticao(memoria, particao, indiceMenor);
            if(indiceReservatorio > 0){
                fclose(particao);
                sprintf(nome, "part-%.5d.dat", ++indice);
                particao = fopen(nome,"wb");
                numPart++;
                fseek(reservatorio, 0L, SEEK_SET);
                assert(fread(&(memoria), sizeof(tpFilm), indiceReservatorio, reservatorio) > 0);
                tpFilm aux;
                for (int i = 0; i < indiceReservatorio; i++)
                {
                    for (int j = 0; j < indiceReservatorio; j++)
                    {
                        if (strcmp(memoria[i].originalTitle, memoria[j].originalTitle) < 0)
                        {
                            aux = memoria[i];
                            memoria[i] = memoria[j];
                            memoria[j] = aux;
                        }
                    }
                }
                for (int i = 0; i < indiceReservatorio; i++){
                    assert(fwrite(&memoria[i], sizeof(tpFilm), 1, particao) > 0);
                }
            }
            fclose(particao);
            fclose(reservatorio);
            fclose(arquivo);
            break;
        }
    }
}

void intercalacao(){

    int number = 0;
    char nome[100];
    FILE *arq1;
    FILE *arq2;
    FILE *arq3;
    tpFilm part1;
    tpFilm part2;
    numPart = 161;

    while(number < numPart - 1)
    {
        sprintf(nome, "part-%.5d.dat", ++number);
        arq1 = fopen(nome, "rb");
        sprintf(nome, "part-%.5d.dat", ++number);
        arq2 = fopen(nome, "rb");
        sprintf(nome, "part-%.5d.dat", ++numPart);
        arq3 = fopen(nome, "wb");

        int reg1 = fread(&(part1), sizeof(tpFilm), 1, arq1);
        int reg2 = fread(&(part2), sizeof(tpFilm), 1, arq2);

        do{
            if(strcmp(part1.originalTitle, part2.originalTitle) < 0){
                fwrite(&part1, sizeof(tpFilm), 1, arq3);
                reg1 = fread(&(part1), sizeof(tpFilm), 1, arq1);
            } else {
                fwrite(&part2, sizeof(tpFilm), 1, arq3);
                reg2 = fread(&(part2), sizeof(tpFilm), 1, arq2);
            }
        } while ((reg1 > 0) && (reg2 > 0));

        if(reg2 == 0)
        {
            do{
                fwrite(&part1, sizeof(tpFilm), 1, arq3);
            } while(fread(&(part1), sizeof(tpFilm), 1, arq1) > 0);

        } else if(reg1 == 0)
        {
            do{
                fwrite(&part2, sizeof(tpFilm), 1, arq3);
            } while(fread(&(part2), sizeof(tpFilm), 1, arq2) > 0);
        }

        fclose(arq1);
        fclose(arq2);
        fclose(arq3);
    }
}

void busca(char *arq, char *filme){

  FILE *arquivo = fopen(arq, "rb");
  tpFilm film;
  int inicio = 0;
  int meio = 0;
  int fim = 0;
  int achou = 0;
  int posicao = 0;

  if(arquivo == NULL)
  {
    printf("Error ao abrir o arquivo");
    exit(EXIT_FAILURE);
  }

  fseek(arquivo, 0L, SEEK_END);
  fim = (ftell(arquivo) / sizeof(tpFilm));
  fseek(arquivo, 0L, SEEK_SET);

  while(inicio <= fim){
    meio = ((inicio + fim) / 2);

    fseek(arquivo, meio * sizeof(tpFilm), SEEK_SET);
    fread(&(film), sizeof(tpFilm), 1, arquivo );

    if(strcmp(film.originalTitle, filme) == 0){
      achou = 1;
      posicao = meio;
      break;
    } else if(strcmp(film.originalTitle, filme) < 0){
      inicio = meio + 1;
    } else{
      fim = meio - 1;
    }
  }

  if(achou == 1){
    printf("\n\033[1;36mBusca realizada com sucesso\033[0m");
    printf("\n\033[1;31mFilme Número: %i\033[0m\n", meio);
    printf("\033[1;38m%s\033[0m\n", film.titleType);
    printf("\033[1;38m%s\033[0m\n", film.primaryTitle);
    printf("\033[1;38m%s\033[0m\n", film.originalTitle);
    printf("\033[1;38m%u\033[0m\n", film.isAdult);
    printf("\033[1;38m%u\033[0m\n", film.startYear);
    printf("\033[1;38m%u\033[0m\n", film.endYear);
    printf("\033[1;38m%s\033[0m\n", film.runtimeMinutes);
    printf("\033[1;38m%s\033[0m\n\n", film.genres);

  } else{
    printf("Não foi possível encontrar o livro\n");
  }

  fclose(arquivo);
}

int main (int ac, char **av){
    if(strcmp(av[2], "info") == 0){
        arquivoInfo(av[1]);
    } else if(strcmp(av[2], "imprimir") == 0){
        int qtde;
        printf("\n\033[1;33mDeseja imprimir quantos filmes ?\033[0m\n");
        scanf("%i", &qtde);
        imprimirFilmes(av[1], qtde);
    } else if(strcmp(av[2], "classificação") == 0){
        classificacao();
    } else if(strcmp(av[2], "intercalação") == 0){
        intercalacao();
    } else if(strcmp(av[2], "busca") == 0){
        busca(av[1], av[3]);
    } else if(strcmp(av[2], "direto") == 0){
        int reg;
        printf("\n\033[1;33mDeseja buscar qual registrador ?\033[0m\n");
        scanf("%i", &reg);
        acesso_Direto(av[1], reg);
    }
    return EXIT_SUCCESS;
}