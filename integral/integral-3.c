#include<stdio.h>
#include<omp.h>

int main(int argc, char** argv){
	float a=0.0, b=1.0; //intervalo a calcular
	int n = 1024; //numero de trapezoides
	float h; //base do trapezoide
	float integral; //resultado
	
	float f(float x) {
		float fx; // valor de retorno
		// esta é a função a integrar
		// exemplo: função quadrática
		fx = x * x;
		return fx;
	}
	float calcula(float local_a, float local_b, int local_n, float h) {
		float integral;
		float x, i;
		f(x); // função a integrar
		integral = ( f(a) + f(b) ) /2.0;
		x = a;
		#pragma omp parallel shared(h, n) private(i, x) reduction(+:integral)
		{
		#pragma omp for
		for(int i=1; i<=n; i++) {
			x += h;
			integral += f(x);
		}
		integral *= h;
		printf("resultado: %f\n", integral);
		}
		return integral;
	}

	h = (b-a) / n;

	integral = calcula(a, b, n, h);

	printf("resultado: %f\n", integral);
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
