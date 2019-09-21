#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#define SIZE 10

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}

void main(int argc, char** argv){
    int **A = alloc_2d_int(SIZE,SIZE);
    int **B = alloc_2d_int(SIZE,SIZE);
    int **C = alloc_2d_int(SIZE,SIZE);
    int i, j, meu_rank, np = 0, tag = 0;

    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&np);


    int linhas_por_processo = SIZE/np; // numero de linhas que cada processo é responsável
    int resto = SIZE % np;
    if(meu_rank < resto){
        linhas_por_processo += 1;  
    } 
    int prim_linha = (meu_rank * linhas_por_processo);
    if(meu_rank >= resto){
        prim_linha += resto;
    } 
    
    int **resultado = alloc_2d_int(linhas_por_processo,SIZE);

    if(meu_rank < SIZE){
        printf("PROCESSO %d\n", meu_rank);
        printf("Linhas: %d ate %d\n\n\n", prim_linha, prim_linha+linhas_por_processo-1);
        

        // inicializando matrizes
        for(i=0;i<SIZE;i++){
            for(j=0;j<SIZE;j++){
                A[i][j] = 1;
                B[i][j] = 1;
                C[i][j] = 0;
            }
        }

        // inicializando matriz resultado
        for(i=0;i<linhas_por_processo;i++){
            for(j=0;j<SIZE;j++){
                resultado[i][j] = 0;
            }
        }

        int coluna = 0;
        for(int l = 0; l < linhas_por_processo; l++){
           for(i = 0; i < SIZE; i++){
               for(j = 0; j < SIZE; j++){
                    resultado[l][coluna] += A[i][j] * B[j][i];
                }
                coluna++;
            }
            coluna = 0;
        }

        int destino = 0;
        MPI_Send(&(resultado[0][0]),
            linhas_por_processo*SIZE,
            MPI_INT,
            destino,
            tag,
            MPI_COMM_WORLD);

        if(meu_rank == 0){
            int origem;
            int processos = np;
            if(np > SIZE) processos = SIZE;
            for(origem = 0; origem < processos; origem++){
                int lp = SIZE/np; 
                int tamanho;
                if(origem<resto){
                    lp += 1;
                    tamanho = lp * SIZE;

                    MPI_Recv(&C[origem*lp][0],
                        tamanho,
                        MPI_INT,
                        origem,
                        tag,
                        MPI_COMM_WORLD,
                        &status);
                }
                else{
                    tamanho = lp * SIZE;
                    MPI_Recv(&C[(origem*lp)+resto][0],
                        tamanho,
                        MPI_INT,
                        origem,
                        tag,
                        MPI_COMM_WORLD,
                        &status);
                }
                
            }
            
        }

    }

    MPI_Finalize();
    if(meu_rank == 0){
        for(i=0;i<SIZE;i++){
            for(j=0;j<SIZE;j++){
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }
    
}