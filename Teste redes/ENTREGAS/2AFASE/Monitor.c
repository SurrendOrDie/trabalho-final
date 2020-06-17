#include "Header.h"
/***************************************************************************************************************************/
/**VARIÁVEIS GLOBAIS******************************************************************************************************/
/***************************************************************************************************************************/
client_t simulador;
mutex_t mMonitor;
/***************************************************************************************************************************/
/**MAIN********************************************************************************************************************/
/***************************************************************************************************************************/
int main()
{
	/* Mensagem de arranque */
	printf("@@@MONITOR@@@\n");
	printf("A configuração está a ser lida..\n");
	
	/* É lida a configuração do Monitor */
	lerConfigMonitor(&mconf);
	printf("A configuração foi lida com sucesso.\n");
	
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
	
	/* No decorrer da simulação */
	while(!terminar)
	{
		/* São convertidas mensagens enviadas pelo simulador */
		converteMens(&terminar,lerDoSimulador());
	}

	/********************/
	/* Fim da simulação */
	/********************/
	
	/* Array onde ficarão as mensagens que serão enviadas para o log */
	char log[BUFFER_SIZE];
	
	/* Mensagem de fim de simulação */
	/* E são mostradas as estatísticas calculadas com base na simulação */
	printf("********************************************************************************\n");
	
	sprintf(log,"Tempo médio de espera na fila do guiché: %.2f segundos\n",tempoMedEspG);
	printf("%s\n",log);
	escreverNoLog(log);
	
	sprintf(log,"Tempo médio de espera na fila das devoluções: %.2f segundos\n",tempoMedEspDev);
	printf("%s\n",log);
	escreverNoLog(log);
	
	sprintf(log,"Número de desistências: %d\n",nrDesistencias);
	printf("%s\n",log);
	escreverNoLog(log);
	
	printf("********************************************************************************\n");
	printf("---FIM---\n");
	
	/* Fica em "pausa" */
	getchar();
	return 0;
}

void converteMens(int *terminar,char mensagem[])
{
	int id,tempEspG,numUtil,tmpDev,prio;
	
	/* Id do evento (indicado na mensagem) */
	int evento = mensagem[0] - 48;
	
	/* Array onde ficarão as mensagens que serão enviadas para o log */
	char log[BUFFER_SIZE];
	
	switch (evento)
	{
		/* Caso seja para terminar a simulação */
		case FIM_SIMULACAO:
		
			numUtil=lerIntDoSimulador();	//Busca o nº de utilizadores
			
			tempoMedEspG/=numUtil;				//Calcula o tempo médio de espera no guiché de compra
			tempoMedEspDev/=numUtil;			//Calcula o tempo médio de espera no guiché de devolução
			
			*terminar=1;									//Indica o fim da simulação
			
			break;
			
		/* Caso seja uma compra de um bilhete */
		case COMPRA_BILHETE:
		
			id=lerIntDoSimulador();				//Busca o id do cliente em questão
			
			tempEspG=lerIntDoSimulador();	//Busca o tempo de espera do cliente (no guiché de compra)
			tempoMedEspG+=tempEspG;				//Incrementa ao tempo total de espera na fila (para depois calcular o tempo médio)
			
			/* É enviado para o log uma mensagem sobre o evento */
			sprintf(log,"O cliente %d comprou o bilhete (esperou na fila %d segundos)\n",id,tempEspG);
			printf("%s",log);
			escreverNoLog(log);
			
			break;

		/* Caso se trate de uma desistência */
		case DESISTENCIA:
		
			id=lerIntDoSimulador();							//Busca o id do cliente em questão
			
			nrDesistencias=lerIntDoSimulador(); //Busca o número de desistência
			
			tmpDev=lerIntDoSimulador();					//Busca o tempo de espera do cliente (no guiché de devolução)
			tempoMedEspDev+=tmpDev;							//Incrementa ao tempo total de espera na fila (para depois calcular o tempo médio)
			
			/* É enviado para o log uma mensagem sobre o evento */			
			sprintf(log,"O cliente %d mudou de ideias e foi para a guiché de devolução (esperou %d segundos na fila de devolução)\n",id,tmpDev);
			printf("%s",log);
			escreverNoLog(log);
			
			break;
	
		/* Caso se trate de um cliente a chegar ao guiché de compra */
		case CHEGAR_GUICHE_COMPRA:
			id=lerIntDoSimulador();				//Busca o id do cliente em questão
			prio=lerIntDoSimulador();			//Busca a prioridade do cliente em questão
			
			/* É enviado para o log uma mensagem sobre o evento */		
			sprintf(log,"O cliente %d chegou ao guiché de compra com prioridade %d\n",id,prio);
			printf("%s",log);
			escreverNoLog(log);
			
			break;
	}
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
