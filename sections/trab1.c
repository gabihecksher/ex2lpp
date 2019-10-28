// A,B,C,D,E,F,G,H são matrizes 10x10 inicializadas com 1
//thread 1: (somar) R1 = A + B 
//thread 2: (multiplicar)R2 = C x D 
//thread 3: (somar) R3 = E + F 
//thread 4: (multiplicar) R4 = G x H 
//resultado final (somar) RF =  R1 + R2 + R3 + R4 -> colocar omp atomico aqui, a thread que for terminando ja vai adicionando no final
//unir paralelismo funcional com paralelismo de dados

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>


int main(){
	int i, j, k;
	int N = 10;
	int a[N][N], b[N][N], c[N][N], d[N][N], e[N][N], f[N][N], g[N][N], h[N][N];
	int rf[N][N];
	int resultado[N][N], rf1[N][N], rf2[N][N];

	for(i=0; i < N; i++){
		for(j=0; j < N; j++){
			resultado[i][j] = 0;
			rf1[i][j]=0;
			rf2[i][j]=0;
		}
	}	
	#pragma omp parallel shared(a,b,c,d,e,f,g,h,N) private(i,j) num_threads(4)
	{
		#pragma omp for
			for(i=0; i < N; i++){
				for(j=0; j < N; j++){
					a[i][j] = 1;
					b[i][j] = 1;
					c[i][j] = 1;
					d[i][j] = 1;
					e[i][j] = 1;
					f[i][j] = 1;
					g[i][j] = 1;
					h[i][j] = 1;
					rf[i][j] = 0;
				}
			}
	}

	#pragma omp parallel default(none) shared(N,a,b,c,d,e,f,g,h,rf1,rf2) private(i,j,k) num_threads(4)
	{
		int rf[N][N], r1[N][N], r2[N][N], r3[N][N], r4[N][N]; 
		
		
		#pragma omp sections nowait
		{
			// section que calcula r1, r3 e guarda a soma delas em rf1
			#pragma omp section
			{
				#pragma omp parallel shared(N,a,b,e,f,r1,r3,rf1) private(i,j) num_threads(4)
				{
					#pragma omp for
						for(i=0; i<N; i++){
							for(j=0; j<N; j++){
								r1[i][j] = a[i][j] + b[i][j];
								r3[i][j] = e[i][j] + f[i][j];
							}	
						}
					#pragma omp for
						for(i=0; i<N; i++){
							for(j=0; j<N; j++){
								rf1[i][j] = r1[i][j] + r3[i][j];
							}	
						}
				}
				
				
			}

			// section que calcula r2, r4 e guarda a soma delas em rf2
			#pragma omp section
			{
				int somaprod2, somaprod4,linha,coluna,i;	
					
				#pragma omp parallel shared(c,d,g,h,N,r2,r4,somaprod2,somaprod4,rf2) private(linha,coluna,i,j) num_threads(4)
				{
					#pragma omp for
						for(linha=0; linha<N; linha++){
						    for(coluna=0; coluna<N; coluna++){
						    	somaprod2=0;
						    	somaprod4=0;
						    	for(i=0; i<N; i++){
						      		somaprod2+=c[linha][i]*d[i][coluna];
						      		somaprod4+=g[linha][i]*h[i][coluna];
						     	} 
						    	r2[linha][coluna]=somaprod2;
						    	r4[linha][coluna]=somaprod4;
						    }
						}
					#pragma omp for
						for(i=0; i<N; i++){
							for(j=0; j<N; j++){
								rf2[i][j] = r2[i][j] + r4[i][j];
							}	
						}
				}
				
			}

		}//end of sections      

		//usar critical em vez de atomic quando for varias intruçoes
		
	}//end of parallel region
	printf("matriz rf1:\n");
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			printf("%d ", rf1[i][j]);
		}
		printf("\n");
	}
	printf("matriz rf2:\n");
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			printf("%d ", rf2[i][j]);
		}
		printf("\n");
	}
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			resultado[i][j] += rf1[i][j];
			resultado[i][j] += rf2[i][j];
		}
	}
	printf("matriz resultado final:\n");
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			printf("%d ", resultado[i][j]);
		}
		printf("\n");
	}
		
}