#include "Header.h"
/***************************************************************************************************************************/
/**VARIÁVEIS GLOBAIS******************************************************************************************************/
/***************************************************************************************************************************/
client_t simulador;
int nrViagensSimul=0;
int nrClientesViajaram=0;
int numeroCarros;
int numeroClientePorCarrinho;

/*String com as mensagens*/
char tempoEsperaAtualCompra[BUFFER_SIZE];
char numeroClienteD[BUFFER_SIZE];
char tempoEsperaDevAtual[BUFFER_SIZE];
char numeroViagem[BUFFER_SIZE];
char tempoEsperaCarrinho[BUFFER_SIZE];
char configuracao[4][BUFFER_SIZE];

/***************************************************************************************************************************/
/**MAIN********************************************************************************************************************/
/***************************************************************************************************************************/
int main()
{

	/* Mensagem de arranque */
	printf("@@@MONITOR@@@\n");

	/* É feita a conexão com o Simulador */
	printf("À espera do simulador..\n");
	simulador = esperarPorCliente();
	printf("O simulador foi conectado com sucesso.\n");

	/***********************/
	/* Início da simulação */
	/***********************/
	printf("---INÍCIO---\n");
	printf("********************************************************************************\n");

	/* 'Booleano' usado para determinar se a simulação deve terminar */
	int terminar=0;
	/*Mensagens default*/

	strcpy(numeroViagem,"Número de viagens: 0 \n");
	strcpy(tempoEsperaAtualCompra,"Tempo médio de espera do bilhete: 0 \n");
	strcpy(numeroClienteD,"Número de desistências: 0\n");
	strcpy(tempoEsperaDevAtual,"Tempo médio de espera para desistir: 0.00\n");
	strcpy(tempoEsperaCarrinho,"Tempo médio de espera para entrar num carrinho: 0.00\n");
	/* No decorrer da simulação */
	while(!terminar)
	{
		/* São convertidas mensagens enviadas pelo simulador */
		converteMens(&terminar,lerDoSimulador());
	}
	escreverEstatisticas();

	return 0;
}

void converteMens(int *terminar,char mensagem[])
{
	int id, tempEspG, numUtil, tmpDev, prio;
	int comecouViagem,idCliente;
	int tempo;
	double tempMedActualGuiche, tempMedActualDesis;

	/* Id do evento (indicado na mensagem) */
	int evento = mensagem[0] - 48;

	/* Array onde ficarão as mensagens que serão enviadas para o log */
	char log[BUFFER_SIZE];

	switch (evento)
	{
		/* Caso seja para terminar a simulação */
		case FIM_SIMULACAO:

			nrViagensSimul=lerIntDoSimulador();
			tempoMedEspG/=nrClientesActuais;							//Calcula o tempo médio de espera no guiché de compra
			tempoMedEspDev/=nrDesistencias;			//Calcula o tempo médio de espera no guiché de devolução

			*terminar=1;									//Indica o fim da simulação

			break;

		/* Caso seja uma compra de um bilhete */
		case COMPRA_BILHETE:

			id=lerIntDoSimulador();				//Busca o id do cliente em questão

			tempEspG=lerIntDoSimulador();	//Busca o tempo de espera do cliente (no guiché de compra)
			tempoMedEspG+=tempEspG;				//Incrementa ao tempo total de espera na fila (para depois calcular o tempo médio)
			nrClientesActuais=lerIntDoSimulador();

			/* É enviado para o log uma mensagem sobre o evento */
			sprintf(log,"O cliente %d comprou o bilhete (esperou na fila %d segundos)\n",id,tempEspG);
			escreverNoLog(log);

			tempMedActualGuiche=tempoMedEspG/nrClientesActuais;
			sprintf(tempoEsperaAtualCompra,"Tempo médio de espera do bilhete: %.2fs\n",tempMedActualGuiche);
			break;

		/* Caso se trate de uma desistência */
		case DESISTENCIA:

			id=lerIntDoSimulador();							//Busca o id do cliente em questão

			nrDesistencias=lerIntDoSimulador(); //Busca o número de desistência

			tmpDev=lerIntDoSimulador();					//Busca o tempo de espera do cliente (no guiché de devolução)
			tempoMedEspDev+=tmpDev;							//Incrementa ao tempo total de espera na fila (para depois calcular o tempo médio)

			/* É enviado para o log uma mensagem sobre o evento */
			sprintf(log,"O cliente %d mudou de ideias e foi para a guiché de devolução (esperou %d segundos na fila de devolução)\n",id,tmpDev);
			escreverNoLog(log);
			
			sprintf(numeroClienteD,"Número de desistências: %d\n",nrDesistencias);

			tempMedActualDesis=tempoMedEspDev/nrDesistencias;
			sprintf(tempoEsperaDevAtual,"Tempo médio de espera para desistir: %.2fs\n",tempMedActualDesis);
			break;

		/* Caso se trate de um cliente a chegar ao guiché de compra */
		case CHEGAR_GUICHE_COMPRA:
			id=lerIntDoSimulador();				//Busca o id do cliente em questão
			prio=lerIntDoSimulador();			//Busca a prioridade do cliente em questão

			/* É enviado para o log uma mensagem sobre o evento */
			sprintf(log,"O cliente %d chegou ao guiché de compra com prioridade %d\n",id,prio);
			escreverNoLog(log);

			break;

		case INICIO_VIAGEM:
			nrViagemActual++;
			sprintf(numeroViagem,"Número de viagens: %d\n",nrViagemActual);

			break;

		case FIM_VIAGEM:
			sprintf(log,"ACABOU A VIAGEM NÚMERO: %d\n",nrViagemActual);
			escreverNoLog(log);

			break;

		case ENTRADA_NO_CARRO:
			id=lerIntDoSimulador();
			tempo=lerIntDoSimulador();
			nrClientesViajaram=lerIntDoSimulador();
			tempoMedEspCarrosActual+=tempo;

			sprintf(tempoEsperaCarrinho,"Tempo médio de espera para entrar num carrinho: %.2fs\n",(double)((double)tempoMedEspCarrosActual/(double)nrClientesViajaram));

			sprintf(log,"O cliente %d demorou %d segundos na fila dos carrinhos\n",id,tempo);
			escreverNoLog(log);
			break;

		case INICIO_SIMULACAO:

			sprintf(configuracao[0],"Número de carros: %i\n",numeroCarros=lerIntDoSimulador());

			sprintf(configuracao[1],"Número de lugares dos carros: %i\n",numeroClientePorCarrinho=lerIntDoSimulador());

			sprintf(configuracao[2],"Tamanho da montanha-russa: %i\n",numeroClientePorCarrinho*numeroCarros);

			sprintf(configuracao[3],"Número total de pessoas: %i\n",lerIntDoSimulador());

			break;
	}
	
	printf("------------------------------------------------------------------------------\n");

	char linhaDisplay[]="|               ";
	char linhaDisplayF[]="                                  |";
	for (size_t i = 0; i < 3; i++) {
		printf("|\n");
	}
	printf("%s%s",linhaDisplay,"[ MONTANHA-RUSSA DOS MASOQUISTAS ]\n");
	printf("%s%s",linhaDisplay,configuracao[0]);
	printf("%s%s",linhaDisplay,configuracao[1]);
	printf("%s%s",linhaDisplay,configuracao[2]);
	printf("%s%s",linhaDisplay,configuracao[3]);
	printf("%s%s",linhaDisplay,numeroViagem);
	printf("%s%s",linhaDisplay,tempoEsperaDevAtual);
	printf("%s%s",linhaDisplay,numeroClienteD);
	printf("%s%s",linhaDisplay,tempoEsperaAtualCompra);
	printf("%s%s",linhaDisplay,tempoEsperaCarrinho);
	for (size_t i = 0; i < 8; i++) {
		printf("|\n");
	}
	printf("------------------------------------------------------------------------------\n");
}
/***************************************************************************************************************************/
/**FUNÇÕES AUXILIARES*****************************************************************************************************/
/***************************************************************************************************************************/
char* lerDoSimulador()
{
	char * mensagem = (char *) malloc(sizeof(char)*BUFFER_SIZE);
	read(simulador,mensagem,BUFFER_SIZE);
	return mensagem;
}
int lerIntDoSimulador()
{
	return atoi(lerDoSimulador());
}
/**************************************************************************************************************/
/**ESCREVER ESTATÍSTICAS*****************************************************************************/
/**************************************************************************************************************/
void escreverEstatisticas()
{
	FILE *file = fopen(FICHEIRO_ESTATISTICAS,APPEND_MODE);
	fprintf(file,"%s","[ MONTANHA-RUSSA DOS MASOQUISTAS ]\n");
	fprintf(file,"%s",configuracao[0]);
	fprintf(file,"%s",configuracao[1]);
	fprintf(file,"%s",configuracao[2]);
	fprintf(file,"%s",configuracao[3]);
	fprintf(file,"%s",numeroViagem);
	fprintf(file,"%s",tempoEsperaDevAtual);
	fprintf(file,"%s",numeroClienteD);
	fprintf(file,"%s",tempoEsperaAtualCompra);
	fprintf(file,"%s",tempoEsperaCarrinho);
	fclose(file);
}
