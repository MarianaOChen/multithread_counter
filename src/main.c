/* Contador de numero primos
 * Esta tarefa consiste em receber um texto na entrada e contar quantos números primos existem nessa entrada, utilizando multithreads
 
** Autora: Mariana Chen 
** Ultima modificacao: 22Maio2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MAX_THREADS 4
#define MAX_ENTRADAS 100
#define ATIVO 1
#define INATIVO 0

typedef struct {
	int N;
	int ID;	
	int entrada;
} thread_args;

int ehprimo(int numero);
void* worker(void *arg);

pthread_mutex_t trava;
int n_workers = 0;
pthread_t workers[MAX_THREADS]; 
int disponivel[MAX_THREADS]; //disponibilidade de uma thread
int resultados[MAX_ENTRADAS]; //guardar resultado de ser ou nao primo


int main() {

	/*Leitura da entrada*/
	int numero;
	numero = '\0';
	int entradas[MAX_ENTRADAS];
	int contador = 0;
	while(scanf("%d", &numero) != EOF){
		entradas[contador] = numero;
		//printf("Recebi: %d\n", entradas[contador]);		
		contador++; //conta quantos numeros serao tratados 
   	}

	/*Inicializar vetor de disponibilidade da thread*/
	for(int i=0;i<MAX_THREADS;i++){
		disponivel[i] = INATIVO;
	}

	/*Threads*/
	int aux=0;	
	thread_args *send_args;
	while(aux<contador){
		for(int i=0;i<MAX_THREADS;i++){
			if(disponivel[i]== INATIVO){//thread disponivel
			/*criar thread*/
			//printf("Criando thread\n");
			send_args = (thread_args*)malloc(sizeof(thread_args));
			send_args ->N = entradas[aux];
			send_args ->ID = i;
			send_args -> entrada = aux;
			pthread_mutex_lock(&trava);/*Inicio de condicao de corrida -- acesso ao vetor workers*/
			disponivel[i] = ATIVO; //thread em funcionamento
			pthread_create(& (workers[i]), NULL, worker, (void*) send_args);
			pthread_mutex_unlock(&trava);/*Fim de condicao de corrida -- acesso ao vetor workers*/
		
			aux++; //pode verificar o proximo numero
			break; //sair do for para nao atribuir o mesmo numero para threads diferentes
			}		
		}

	}

	/*Verificar se todas as threads terminaram de processar*/
	for(int i=0;i<MAX_THREADS;i++){
		if(disponivel[i] == ATIVO){
			pthread_join(workers[i],NULL);
		}
	}

	/*Computar resultados e imprimir na tela*/
	int contagem;
	contagem = 0;
	for(int i=0;i<contador;i++){
		contagem = contagem + resultados[i];
	}
	printf("%d\n",contagem);
	

	return 0;
}

int ehprimo(int numero){
/*Função que testa se o numero eh primo - um numero primo somente eh divisivel por 1 e ele mesmo
Como fazer o teste: Dado um numero, fazer divisoes sucessivas (a partir de 2)verificando o modulo, ate a metade do numero. 
Exemplo: 15. 15/2 = 7 %1 , 15/3 = 5 %0 (portanto nao eh primo)... 7: 7/2 = 3 %1, 7/3 = 2 %1 (portanto eh primo)*/

	int n;
	int modulo;
	int verificado = 1;
	int aux;

	n = numero;

	if(n <= 1){
	verificado = 0	;
	}
	else if (n == 2){
		verificado = 1;
	}
	else{
		for(aux = 2; aux <= n/2;aux++){
			//printf("%d mod %d\n", n,aux);
			modulo = numero%aux;
			if(modulo == 0){ //nao primo
				verificado = 0;
				break;
			}
		}
	}
	return verificado;
}

void* worker(void *arg) {
	thread_args *info = (thread_args *)arg; 
	int result = ehprimo(info->N);
	//printf("%d => %d\n", info->N,result);
	resultados[info->entrada] = result;
	//printf("Worker %d verificou numero %d, resultado %d\n",info->ID, info->N,resultados[info->entrada]);
	pthread_mutex_lock(&trava); /*Inicio de condicao de corrida -- acesso ao vetor disponivel*/
	disponivel[info->ID] = INATIVO;
	free(info);
	pthread_mutex_unlock(&trava); /*Fim de condicao de corrida -- acesso ao vetor disponivel*/
	return NULL;
}
