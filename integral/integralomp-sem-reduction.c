#include<stdio.h>
#include<omp.h>

int main(int argc, char** argv){
	float a=0.0, b=1; //intervalo a calcular
	int n = 1024; //numero de trapezoides
	float h; //base do trapezoide
	float resultado_integral; //resultado
	int num_threads = 4;

	float f(float x) {
		float fx; // valor de retorno
		// esta é a função a integrar
		// exemplo: função quadrática
		fx = x * x;
		return fx;
	}
	float calcula(float local_a, float local_b, int local_n, float h) {
        int i;
	    float vet_integral[num_threads];
        #pragma omp parallel shared(vet_integral, num_threads) private(i) num_threads(num_threads)
        {
            #pragma omp for
            for(i=0; i<num_threads; i++){
                vet_integral[i] = 0.0; //inicializando vetor com 0
            } 
        }
	    float x;
	    f(x); // função a integrar
	    x = local_a;
	    #pragma omp parallel shared(h, local_n,vet_integral) private(i,x) num_threads(num_threads)
	    {
	    	int iam = omp_get_thread_num();
	    	x += h*(n/num_threads)*iam;
	    	#pragma omp for
		    for(i=1; i<local_n; i++) {
		        x += h;
		        vet_integral[iam] += f(x);
		    }
		    vet_integral[iam] += (f(local_a) + f(local_b))/2.0;
		    vet_integral[iam] *= h;
	    }//fim parallel
        
        float soma_integral = 0.0;
        for(int j=0; j<num_threads; j++){
            soma_integral += vet_integral[j]; 
        } //somando pra ter o valor final da integral

	    return soma_integral;
	} //fim funcao calcula

	h = (b-a) / n;

	resultado_integral = calcula(a, b, n, h);

	printf("\n\nresultado: %f\n", resultado_integral);
}

//terminar o trab das sections e
//terminar esse do omp
//quem fizer ganha meio ponto
//quem perder perde meio ponto
//dicas:
//com reduction funciona, mas eh ruim pq chama um sessao critica para cada iteração
//entao:
//criar um vetor de integrais, cada thread vai incrementar na sua posicao
//a variavel eh shared mas n tem condicao de corrida pois cada um mexe na sua posicao
//cria um for onde vamos somar todos os valores desse vetor para ter o resultado final da integral
