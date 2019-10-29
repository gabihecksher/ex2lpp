#include<stdio.h>
#include<mpi.h>

int main(int argc, char** argv){
	int my_rank; //processo atual
	int p; //numero de processos
	float a=0.0, b=1.0; //intervalo a calcular
	int n = 1024; //numero de trapezoides
	float h; //base do trapezoide
	float local_a, local_b; // intervalo local
	int local_n; //numero de trapezoides local
	float integral; //integral do intervalo
	float total; //integral total
	int source; //remetente da integral
	int dest=0; //destino das integrais ( no 0 )
	int tag=200; //tipo de mensagem(unico)
	MPI_Status status;
	
	float f(float x) {
		float fx; // valor de retorno
		// esta é a função a integrar
		// exemplo: função quadrática
		fx = x * x;
		return fx;
	}
	//float calcula(float local_a, float local_b, int local_n, float h);
	float calcula(float local_a, float local_b, int local_n, float h) {
		float integral;
		float x, i;
		f(x); // função a integrar
		integral = ( f(local_a) + f(local_b) ) / 2.0;
		x = local_a;
		for(i=1; i<=local_n; i++) {
			x += h;
			integral += f(x);
		}
		integral *= h;
		return integral;
	}


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	h = (b-a) / n; //calcula base
    int resto = n/p; //resto do num de trapezoides divido por num de processos
    local_n = n/p; //calcula numero de trapezoides local baseado no numero de processos
    if(resto != 0) {
        if(my_rank < resto){ //os primeiros processos vao ter mais um trapezoide
            local_n += 1;
            local_a = a + my_rank * local_n * h;
            local_b = local_a + local_n * h;
        } else {
            local_a = a + my_rank * local_n * h + resto*h; //os outros tem o local_a mais pra frente
            local_b = local_a + local_n * h;
        }
    } else{ //se o resto é zero, local_n tem o msm valor para todos os processos
        local_a = a + my_rank * local_n * h;
        local_b = local_a + local_n * h;
    }
    integral = calcula(local_a, local_b, local_n, h);

	MPI_Reduce(&integral, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    if(my_rank == 0) printf("resultado: %f\n", total);
	MPI_Finalize();

}
//depois implementar em openMP
