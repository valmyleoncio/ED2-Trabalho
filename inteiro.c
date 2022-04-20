#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define M 6
#define F 3

int numPart = 1;

typedef struct {
    int numero;
} Inteiro;

void registrarInteiros(){
    Inteiro num;
    FILE *arquivo = fopen("inteiro", "ab");
    
    for(int i = 0; i < 54; i++){
        printf("Digite um inteiro:\n");
        scanf("%d", &num.numero);
        fwrite(&num, sizeof(Inteiro), 1, arquivo);
    }

    fclose(arquivo);
}

void imprimirInteiros(char *arq, char *print){
    Inteiro num;
    int cont = 0;
    FILE *arquivo = fopen(arq, "rb");
    if(arquivo == NULL){
        printf("Error ao abrir o arquivo");
        return;
    }
    printf("\n\033[1;36mArquivo %s:\033[0m\n", print);
    while(fread(&(num), sizeof(Inteiro), 1, arquivo) > 0){
        printf("\033[1;37m%d  ", num.numero);
        if(cont == 10){
            cont = 0;
            printf("\n");
        }
        cont++;
    }

    printf("\n");
    fclose(arquivo);
}

void imprimirParticoes(int number){
    char nome[100];
    sprintf(nome, "partExemplo-%.5d.dat", number);
    FILE *arquivo = fopen(nome, "rb");
    Inteiro num;
    printf("\n\033[1;33mPartição %d:\033[0m", number);
    while(fread(&num, sizeof(Inteiro), 1, arquivo) > 0){
        printf("\033[1;37m  %d", num.numero);
    }
    fclose(arquivo);
}

int menorValor(Inteiro *memoria){

    Inteiro menor = memoria[0];
    int indiceMenor = 0;

    for(int i = 1; i < M; i++)
    {
        if(memoria[i].numero < menor.numero)
        {
            indiceMenor = i;
            menor = memoria[i];
        }
    }

    return indiceMenor;
}

void salvarMemoriaParticao(Inteiro *memoria, FILE *particao, int usado){
   Inteiro aux;
   for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if(i != usado && j != usado)
            {
                if (memoria[i].numero < memoria[j].numero)
                {
                    aux = memoria[i];
                    memoria[i] = memoria[j];
                    memoria[j] = aux;
                }
            }
        }
    }

    for (int i = 0; i < M; i++){
        if(i != usado)
        {
            assert(fwrite(&memoria[i], sizeof(Inteiro), 1, particao) > 0);
        }
    }
}

void classificacao(){
    
    // Atribuitos iniciais
    FILE *arquivo = fopen("inteiro", "rb");
    FILE *reservatorio = fopen("reservatorio.dat", "wb+");
    
    Inteiro memoria[M];
    Inteiro menor;
    int indiceMenor;
    int indiceReservatorio = 0;

    FILE *particao;
    int indice = 1;
    char nome[1000];
    sprintf(nome, "partExemplo-%.5d.dat", indice);
    particao = fopen(nome,"wb");

    assert(arquivo != NULL);
    assert(reservatorio != NULL);
    assert(particao != NULL);

    assert(fread(&(memoria), sizeof(Inteiro), M, arquivo) > 0);
    indiceMenor = menorValor(memoria);
    menor = memoria[indiceMenor];
    assert(fwrite(&memoria[indiceMenor], sizeof(Inteiro), 1, particao) > 0);

    // Loop responsável pelo gerenciamento da memória, reservatório, e criação de novas partições até terminar o arquivo.
    while(1){

        if(fread(&(memoria[indiceMenor]), sizeof(Inteiro), 1, arquivo) > 0)
        {
            if(memoria[indiceMenor].numero < menor.numero)
            {
                assert(fwrite(&memoria[indiceMenor], sizeof(Inteiro), 1, reservatorio) > 0);
                indiceReservatorio++;
                if(indiceReservatorio == M){
                    salvarMemoriaParticao(memoria, particao, indiceMenor);
                    fclose(particao);

                    sprintf(nome, "partExemplo-%.5d.dat", ++indice);
                    particao = fopen(nome,"wb");
                    numPart++;

                    fflush(reservatorio);
                    fseek(reservatorio, 0L, SEEK_SET);
                    assert(fread(&(memoria), sizeof(Inteiro), M, reservatorio) > 0);
                    fseek(reservatorio, 0L, SEEK_SET);

                    indiceMenor = menorValor(memoria);
                    menor = memoria[indiceMenor];
                    assert(fwrite(&memoria[indiceMenor], sizeof(Inteiro), 1, particao) > 0);

                    indiceReservatorio = 0;
                }
            } 
            else 
            {
                indiceMenor = menorValor(memoria);
                menor = memoria[indiceMenor];
                assert(fwrite(&memoria[indiceMenor], sizeof(Inteiro), 1, particao) > 0);
            }
        } else {

            salvarMemoriaParticao(memoria, particao, indiceMenor);
            if(indiceReservatorio > 0){
                fclose(particao);
                sprintf(nome, "partExemplo-%.5d.dat", ++indice);
                particao = fopen(nome,"wb");
                numPart++;
                fseek(reservatorio, 0L, SEEK_SET);
                assert(fread(&(memoria), sizeof(Inteiro), indiceReservatorio, reservatorio) > 0);
                Inteiro aux;
                for (int i = 0; i < indiceReservatorio; i++)
                {
                    for (int j = 0; j < indiceReservatorio; j++)
                    {
                        if (memoria[i].numero < memoria[j].numero)
                        {
                            aux = memoria[i];
                            memoria[i] = memoria[j];
                            memoria[j] = aux;
                        }
                    }
                }
                for (int i = 0; i < indiceReservatorio; i++){
                    assert(fwrite(&memoria[i], sizeof(Inteiro), 1, particao) > 0);
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
    char nome[1000];
    FILE *arq1;
    FILE *arq2;
    FILE *arq3;
    Inteiro part1;
    Inteiro part2;

    while(number < numPart - 1)
    {
        sprintf(nome, "partExemplo-%.5d.dat", ++number);
        arq1 = fopen(nome, "rb");
        sprintf(nome, "partExemplo-%.5d.dat", ++number);
        arq2 = fopen(nome, "rb");
        sprintf(nome, "partExemplo-%.5d.dat", ++numPart);
        arq3 = fopen(nome, "wb");

        int reg1 = fread(&(part1), sizeof(Inteiro), 1, arq1);
        int reg2 = fread(&(part2), sizeof(Inteiro), 1, arq2);

        do{
            if(part1.numero < part2.numero){
                fwrite(&part1, sizeof(Inteiro), 1, arq3);
                reg1 = fread(&(part1), sizeof(Inteiro), 1, arq1);
            } else {
                fwrite(&part2, sizeof(Inteiro), 1, arq3);
                reg2 = fread(&(part2), sizeof(Inteiro), 1, arq2);
            }
        } while ((reg1 > 0) && (reg2 > 0));

        if(reg2 == 0)
        {
            do{
                fwrite(&part1, sizeof(Inteiro), 1, arq3);
            } while(fread(&(part1), sizeof(Inteiro), 1, arq1) > 0);

        } else if(reg1 == 0)
        {
            do{
                fwrite(&part2, sizeof(Inteiro), 1, arq3);
            } while(fread(&(part2), sizeof(Inteiro), 1, arq2) > 0);
        }

        fclose(arq1);
        fclose(arq2);
        fclose(arq3);
    }
}

int main(){
    imprimirInteiros("inteiro", "original");
    classificacao();
    intercalacao();
    imprimirParticoes(1);
    imprimirParticoes(2);
    imprimirParticoes(3);
    imprimirParticoes(4);
    imprimirParticoes(5);
    imprimirParticoes(6);
    printf("\n");
    imprimirInteiros("partExemplo-00011.dat", "final");
    return 0;
}