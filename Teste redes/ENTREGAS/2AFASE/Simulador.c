#include "Header.h"
/***************************************************************************************************************************/
/**VARIÁVEIS GLOBAIS******************************************************************************************************/
/***************************************************************************************************************************/
/**COMUNICAÇÃO**************************/
socket_t s;
mutex_t m_comunicacao;
/***************************************/
/**MODELO - GUICHÉ DE COMPRA************/
tarefa_t guicheCompra;
mutex_t mGCP;
semaforo_t sGCP;
int nrPessoasEspGCP=0;
mutex_t mGCNP;
semaforo_t sGCNP;
int nrPessoasEspGCNP=0;
/***************************************/
/**MODELO - GUICHÉ DE DEVOLUÇÃO*********/
tarefa_t guicheDevolucao;
mutex_t mDev;
semaforo_t sClienteDev,sDev;
int nrPessoasEspDev = 0;
/***************************************/
/**MODELO - CARROS DA MONTANHA-RUSSA****/
tarefa_t filaCarros;
mutex_t mCarros;
semaforo_t sFilaCarros,sCarro1,sCarro2;
int nrPessoasEspCarros = 0;
int tipo_carro = 1;
int viagemEmCurso = 0;
int nrPessoasEspera = 0;
time_t start,end,inicioViagem,tempoAtualViagem;
/***************************************/
/***************************************************************************************************************************/
/**MAIN******************************************************************************************************/
/***************************************************************************************************************************/
int main()
{
	/* Medidas de precaução (devido ao multithreading) */
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
	
	srand(time(NULL));																		//É criada a instância de aleatoriedade				

	printf("@@@SIMULADOR@@@\n");													//Mensagem de arranque
	printf("A configuração está a ser lida..\n");					//Mensagem de arranque
	lerConfigSimulador(&sconf);														//É lido o ficheiro de configuração do Simulador
	
	Fechar(&m_nrUtilizadoresRestantes);									
	nrUtilizadoresRestantes = sconf.max_pessoas_total;		//É guardado o número-limite de utilizadores na variável
	Abrir(&m_nrUtilizadoresRestantes);

	printf("A configuração foi lida com sucesso.\n");			//Mensagem após a leitura da sua configuração
	
	s = criarSocketCliente();															//O Simulador(cliente) conecta-se ao Monitor(servidor)
	printf("Conectou-se com sucesso ao Monitor.\n");			//Mensagem após a conexão estar estabelecida													

	char log[BUFFER_SIZE];																//Array onde vão ser guardadas mensagens a enviar para o log

	/***********************/
	/* Início da simulação */
	/***********************/
	sprintf(log,"---INÍCIO---\n");											
	printf("%s",log);
	escreverNoLog(log);
	printf("********************************************************************************\n");
	
	/* Inicialização dos mutex's */
	inicializarMutex(&m_comunicacao);													//Mutex para a comunicação Simulador-Monitor
	inicializarMutex(&mGCP);																	//Mutex para a fila do guiché de compra (com prioridade)
	inicializarMutex(&mGCNP);																	//Mutex para a fila do guiché de compra (sem prioridade)
	inicializarMutex(&mDev);																	//Mutex para a fila do guiché de devolução
	inicializarMutex(&mCarros);																//Mutex para a fila para os carros da montanha-russa
	
	/* Inicialização dos semáforos */
	inicializarSemaforo(&sGCNP,0);														//Semáforo para a fila do guiché de compra (sem prioridade)
	inicializarSemaforo(&sGCP,0);															//Semáforo para a fila do guiché de compra (com prioridade)
	inicializarSemaforo(&sClienteDev,0);											//Semáforo para os clientes que se dirigem ao guiché de devolução
	inicializarSemaforo(&sDev,0);															//Semáforo para a fila do guiché de devolução
	inicializarSemaforo(&sFilaCarros,0);											//Semáforo para a fila para os carros da montanha-russa
	inicializarSemaforo(&sCarro1,sconf.lotacao_carro);				//Semáforo para o carro 1 da montanha-russa
	inicializarSemaforo(&sCarro2,sconf.lotacao_carro);				//Semáforo para o carro 2 da montanha-russa

	time_t start, end;
	time(&start); 																						//É registado o tempo de início da simulação

	pthread_create(&guicheCompra,NULL,guicheCompraFunc,NULL);	//É criada a tarefa para a fila do guiché de comra
	pthread_create(&guicheDevolucao,NULL,filaDev,NULL);				//É criada a tarefa para a fila do guiché de devolução
	pthread_create(&filaCarros,NULL,filaEsperaCarros,NULL);		//E criada a tarefa para a fila de espera para os carros da montanha-russa

	/* Enquanto houver utilizadores por tratar */
	Fechar(&m_nrUtilizadoresRestantes);
	while(nrUtilizadoresRestantes > 0)												
	{
		Abrir(&m_nrUtilizadoresRestantes);
		
		/* Delay necessário para a estabilidade da comunicação */
		delay();																								
		
		/* Se "calhar" para criar um cliente */
		if(podeCriarCliente())																	
		{
			/* Um cliente é criado */
			criarCliente();
		}
		
		/* Se não houver utilizadores suficientes para uma próxima viagem */
		Fechar(&mCarros);
		if(!viagemEmCurso && nrUtilizadoresRestantes < (sconf.lotacao_carro*2))
		{
			/* Esses utilizadoes são "expulsos" */
			printf("%i cliente(s) teve/tiveram que ser expulso(s), devido à falta de pessoas para a diversão\n",nrUtilizadoresRestantes);
			Abrir(&mCarros);
			break;
		}
		else Abrir(&mCarros);

		/* É atualizado o tempo de simulação */
		time(&end);
		tempoSimul=difftime(end, start);
	}
	Abrir(&m_nrUtilizadoresRestantes);

	/********************/
	/* Fim da simulação */
	/********************/
	/* Cálculos para obter horas, minutos e segundos */
	hora = tempoSimul / 3600;      	//Horas de simulação
	minutosAux = tempoSimul / 60;		//Cálculo intermédio para os minutos de simulação
	minutos = minutosAux % 60;			//Minutos de simulação
	segundos = tempoSimul % 60;			//Segundos de simulação

	/* Mensagem de fim da simulação */
	printf("********************************************************************************\n");
	sprintf(log,"Acabou o tempo de simulação: %d hora(s) %d minuto(s) %d segundo(s)\n",hora,minutos,segundos);
	printf("%s",log);
	escreverNoLog(log);
	printf("********************************************************************************\n");
	sprintf(log,"---FIM---\n");
	printf("%s",log);
	escreverNoLog(log);
	
	/* Avisa o Monitor que a simulação chegou ao fim */
	Fechar(&m_comunicacao);
	escreverIntNoMonitor(FIM_SIMULACAO);
	escreverIntNoMonitor(nrUtilizadores);
	Abrir(&m_comunicacao);

	/* Fica em "pausa" */
	getchar();
	return 0;
}
/***************************************************************************************************************************/
/***************************************************************************************************************************/

/***************************************************************************************************************************/
/**CLIENTE*****************************************************************************************************************/
/***************************************************************************************************************************/
int podeCriarCliente()
{
	/* É aleatoriamente determinado se o cliente é criado ou não */
	int permissao = randWithProb(sconf.taxa_populacao);
	
	/* Em caso afirmativo e em que exceda o número máximo de clientes */
	if(permissao && nrUtilizadores < sconf.max_pessoas_total)
	{
		/* É indicado que o cliente pode ser criado */
		return CLIENTE_E_CRIADO;
	}
	
	/* Caso contrário */
	else
	{
		/* É indicado que o cliente não pode ser criado */
		return CLIENTE_NAO_E_CRIADO;
	}
}

/*	CRIAÇÃO DO CLIENTE	*/
void criarCliente()
{
		Fechar(&m_nrUtilizadores);
		nrUtilizadores++;
		Abrir(&m_nrUtilizadores);

		/* É criada a tarefa para o cliente */
		pthread_t cliente;
		pthread_create(&cliente,NULL,cliente_act,(void *)(intptr_t)(randWithProb(0.5)));
}

/* FUNCIONAMENTO GERAL DO CLIENTE */
void * cliente_act(void *prio)
{
	/* Tempos usados para cálculos estatísticos */
	time_t tempoChegadaG,TempoSaidaG,tempoInicioDev,tempoSaidaDev;
	
	/* Array que vai conter mensagens para o log */
	char log[BUFFER_SIZE];

	/* Inicialização dos atributos do cliente */
	utilizador c;
	Fechar(&m_nrUtilizadores);
	c.id=nrUtilizadores;
	Abrir(&m_nrUtilizadores);
	c.prioritarios=(int *)prio;
	c.tempoEspGuiche=0;
	c.tempoEspCarros=0;
	c.tempoEspDev=0;
	c.emViagem=0;
	c.estado=0;

	/* Mensagem de chegada do cliente */
	sprintf(log,"Chegou o cliente com o id %d e prioridade %d\n",c.id,(int)(intptr_t)c.prioritarios);//Faz cópia da string para log
	printf("%s",log);
	escreverNoLog(log);
	
	/* É comunicado ao Monitor que o cliente chegou ao guiché de compra */
	Fechar(&m_comunicacao);
	escreverIntNoMonitor(CHEGAR_GUICHE_COMPRA);
	escreverIntNoMonitor(c.id);
	escreverIntNoMonitor((int)(intptr_t)(c.prioritarios));
	Abrir(&m_comunicacao);

	/* Se for um cliente prioritário */
	if(c.prioritarios)
	{
		/* É registado o tempo de chegada ao guiché de compra */
		time(&tempoChegadaG);
		
		/* Entra na fila prioritária do guiché de compra */
		entraClienteGuicheCompraPrio(c.id);
		
		/* É registado o tempo de saida do guiché de compra */
		time(&TempoSaidaG);
		c.tempoEspGuiche=difftime(TempoSaidaG, tempoChegadaG);

		/* É comunicado ao Monitor a compra do bilhete pelo cliente */
		Fechar(&m_comunicacao);
		escreverIntNoMonitor(COMPRA_BILHETE);
		escreverIntNoMonitor(c.id);
		escreverIntNoMonitor(c.tempoEspGuiche);
		Abrir(&m_comunicacao);

		/* É registado o tempo (para o caso de entrar para a fila de devolução */
		time(&tempoInicioDev);
		
		/* Se o cliente desistir */
		if(veSeDesiste())
		{
			/* É registado o tempo em que sai da fila de devolução */
			time(&tempoSaidaDev);
			c.tempoEspDev=difftime(tempoSaidaDev,tempoInicioDev);
			
			Fechar(&m_nrDesistencias);
			nrDesistencias++;
			Abrir(&m_nrDesistencias);
			
			printf("O cliente %d vai desistir\n",c.id);
			
			/* É comunicado ao Monitor que houve uma desistência */
			Fechar(&m_comunicacao);
			escreverIntNoMonitor(DESISTENCIA);
			escreverIntNoMonitor(c.id);
			escreverIntNoMonitor(nrDesistencias);
			escreverIntNoMonitor(c.tempoEspDev);
			Abrir(&m_comunicacao);
			
			Fechar(&m_nrUtilizadoresRestantes);
			nrUtilizadoresRestantes--;
			Abrir(&m_nrUtilizadoresRestantes);
		}
		
		/* Se o cliente não desistir */
		else
		{
			/* O cliente entra na fila para os carros da montanha-russa */
			entraClienteCarros(c.id);
		}
	}
	
	/* Se for um cliente não prioritário */
	else
	{
		/* É registado o tempo de chegada ao guiché de compra */	
		time(&tempoChegadaG);
		
		/* Entra na fila prioritária do guiché de compra */		
		entraClienteGuicheCompraNaoPrio(c.id);
		
		/* É registado o tempo de saida do guiché de compra */		
		time(&TempoSaidaG);
		c.tempoEspGuiche=difftime(TempoSaidaG, tempoChegadaG);

		/* É comunicado ao Monitor a compra do bilhete pelo cliente */
		Fechar(&m_comunicacao);
		escreverIntNoMonitor(COMPRA_BILHETE);
		escreverIntNoMonitor(c.id);
		escreverIntNoMonitor(c.tempoEspGuiche);
		Abrir(&m_comunicacao);

		/* É registado o tempo (para o caso de entrar para a fila de devolução */
		time(&tempoInicioDev);

		/* Se o cliente desistir */
		if(veSeDesiste())
		{	
			/* É registado o tempo em que sai da fila de devolução */		
			time(&tempoSaidaDev);
			c.tempoEspDev=difftime(tempoSaidaDev,tempoInicioDev);
			
			Fechar(&m_nrDesistencias);
			nrDesistencias++;
			Abrir(&m_nrDesistencias);
			
			printf("O cliente %d vai desistir\n",c.id);
			
			/* É comunicado ao Monitor que houve uma desistência */			
			Fechar(&m_comunicacao);
			escreverIntNoMonitor(DESISTENCIA);
			escreverIntNoMonitor(c.id);
			escreverIntNoMonitor(nrDesistencias);
			escreverIntNoMonitor(c.tempoEspDev);
			Abrir(&m_comunicacao);
			
			Fechar(&m_nrUtilizadoresRestantes);
			nrUtilizadoresRestantes--;
			Abrir(&m_nrUtilizadoresRestantes);
		}
		
		/* Se o cliente não desistir */		
		else
		{
			/* O cliente entra na fila para os carros da montanha-russa */		
			entraClienteCarros(c.id);
		}
	}
}
int veSeDesiste()
{
	/* Se "calhar" para o cliente desistir */
	if(randWithProb(sconf.taxa_desistencia))
	{
		/* O cliente vai (tentar) desistir */
		/* (há uma possibilidade de afinal não desistir) */
		return entraClienteDev();
	}
	
	/* Caso contrário */
	else
	{
		/* É indicado que o cliente não desistiu */
		return CLIENTE_NAO_DESISTE;
	}
}
/***************************************************************************************************************************/
/***************************************************************************************************************************/

/***************************************************************************************************************************/
/**GUICHÉ DE COMPRA******************************************************************************************************/
/***************************************************************************************************************************/

/*<< CLIENTE >>*/
void entraClienteGuicheCompraPrio(int id)
{
	/* É incrementado o número de pessoas à espera */
	Fechar(&mGCP);
	nrPessoasEspGCP++;
	printf("Número de clientes prioritários à espera no guiché de compra: %d\n",nrPessoasEspGCP);
	Abrir(&mGCP);
	
	/* Espera na fila */
	Esperar(&sGCP);
}
void entraClienteGuicheCompraNaoPrio(int id)
{
	/* É incrementado o número de pessoas à espera */
	Fechar(&mGCNP);
	nrPessoasEspGCNP++;
	printf("Número de clientes não prioritários à espera no guiché de compra: %d\n",nrPessoasEspGCNP);
	Abrir(&mGCNP);
	
	/* Espera na fila */	
	Esperar(&sGCNP);
}

/*<< FILA >>*/
void * guicheCompraFunc()
{
	while(1)
	{
		/* Delay necessário para a estabilidade da comunicação */
		delay();
		
		/* Simulação da eficácia de atendimento na fila */
		if(randWithProb(sconf.taxa_atendimento_compra))
		{
			/* Dirige-se primeiro aos prioritários */
			/* (atendendo-os primeiro) */
			Fechar(&mGCP);
			if(nrPessoasEspGCP>0)
			{
				nrPessoasEspGCP--;
				Abrir(&mGCP);
				
				//servico();
				
				Assinalar(&sGCP);
				printf("Foi assinalado o GCP\n");
			}
			
			/* Caso não hajam clientes prioritários */
			/* É que atende os clientes não prioritários */
			else 
			{
				Abrir(&mGCP);
				Fechar(&mGCNP);
				if(nrPessoasEspGCNP>0)
				{
					nrPessoasEspGCNP--;
					Abrir(&mGCNP);
					
					//servico();
					
					Assinalar(&sGCNP);
					printf("Foi assinalado o GCNP\n");
				}
				else Abrir(&mGCNP);
			}
		}
	}
}
/***************************************************************************************************************************/
/***************************************************************************************************************************/

/***************************************************************************************************************************/
/**GUICHÉ DE DEVOLUÇÃO**************************************************************************************************/
/***************************************************************************************************************************/

/*<< CLIENTE >>*/
int entraClienteDev()
{
	/* Caso a fila para a devolução não esteja grande */
	/* O cliente entra nesta fila */
	Fechar(&mDev);
	if(nrPessoasEspDev < sconf.max_pessoas_dev)
	{
		nrPessoasEspDev++;
		Abrir(&mDev);
		
		/* Assinala ao guiché que um cliente se apresenta na fila */
		Assinalar(&sClienteDev);
		
		/* Espera que seja atendido */
		Esperar(&sDev);
		
		return CLIENTE_DESISTE;
	}
	
	/* Caso contrário */
	/* (caso se encontrem muitas pessoas na fila de devolução) */
	/* O cliente "desiste de desistir" e vai para a diversão */
	else
	{
		Abrir(&mDev);
		return CLIENTE_NAO_DESISTE;
	}
}

/*<< FILA >>*/
void * filaDev()
{
	while(1)
	{
		/* Delay necessário para a estabilidade da comunicação */
		delay();
		
		/* Simulação da eficácia de atendimento da fila */
		if(randWithProb(sconf.taxa_atendimento_dev))
		{
			/* Espera que apareçam clientes na fila */
			Esperar(&sClienteDev);

			/* Recebe um cliente */
			Fechar(&mDev);
			nrPessoasEspDev--;
			Abrir(&mDev);
			
			//servico();

			/* Assinala o fim de atendimento do cliente */
			Assinalar(&sDev);
		}
	}
}
/***************************************************************************************************************************/
/***************************************************************************************************************************/

/***************************************************************************************************************************/
/**FILA DOS CARROS**********************************************************************************************************/
/***************************************************************************************************************************/

/*<< CLIENTE >>*/
void entraClienteCarros(int id)
{
	/* Se houverem clientes suficientes para a próxima diversão */
	Fechar(&m_nrUtilizadores);
	if(nrUtilizadoresRestantes >= (sconf.lotacao_carro*2))
	{
		Abrir(&m_nrUtilizadores);
		
		/* Entra num dos carros */
		/* (a entrada é feita alternadamente) */
		Fechar(&mCarros);
		if(tipo_carro == 1)
		{	
			/* O carro a entrar foi escolhido */
			tipo_carro = 2;	
			nrPessoasEspera++;
			Abrir(&mCarros);
		
			/* Entra na fila de espera para o carro */
			printf("O cliente %d entrou na fila de espera para o carro 1\n",id);
			Esperar(&sCarro1);
			
			/* O cliente vai ser o próximo a entrar nesse carro */
			printf("O cliente %d vai entrar no carro 1\n",id);
			
		}
		else if(tipo_carro == 2)
		{
			/* O carro a entrar foi escolhido */		
			tipo_carro = 1;	
			nrPessoasEspera++;
			Abrir(&mCarros);
			
			/* Entra na fila de espera para o carro */
			printf("O cliente %d entrou na fila de espera para o carro 2\n",id);
			Esperar(&sCarro2);
			
			/* O cliente vai ser o próximo a entrar nesse carro */			
			printf("O cliente %d vai entrar no carro 2\n",id);
		}

		/* Espera pelo momento para entrar */
		/* (no início de uma viagem) */
		Esperar(&sFilaCarros);
		
		/* É guardado o tempo em que iniciou a viagem */
		time(&inicioViagem);
		printf("Começou a viagem para o cliente %d\n",id);
		
		/* A viagem é iniciada */
		Fechar(&mCarros);
		viagemEmCurso=1;
		Abrir(&mCarros);
  }
  
  /* Caso contrário */
  /* (caso não hajam pessoas suficientes para a próxima viagem) */
	else
	{
		Abrir(&m_nrUtilizadores);
		
		/* O cliente entra (forçadamente) na fila de devolução */
		entraClienteDev();
		printf("Desistencia forcada (por falta de pessoas para a diversão)\n");
	}
}

/*<< FILA >>*/
void * filaEsperaCarros()
{
	/* Array que vai conter mensagens para o log */
	char log[BUFFER_SIZE];
	
	while(1)
	{
		/* Delay usado para a estabilidade da comunicação */
		delay();
		
		/* Simulação da eficácia de atendimento na fila */
		if(randWithProb(sconf.taxa_atendimento_carros))
		{
			/* Se uma viagem estiver a decorrer */
			if(viagemEmCurso)
			{
				/* O tempo de viagem é atualizado */
				time(&tempoAtualViagem);
				tempoViagem=difftime(tempoAtualViagem,inicioViagem);
			}
		
			/* Se a viagem chegou ao fim */
			if(tempoViagem >= sconf.t_viagem)
			{
				/* Mensagem de fim de viagem */
				sprintf(log,"**Fim da viagem (a viagem durou %d segundos)**\n",tempoViagem);
				printf("%s",log);
				escreverNoLog(log);
				
				tempoViagem=0;	
				viagemEmCurso=0;
				fimDaViagem();
			}
			
			Fechar(&mCarros);
	
			/* Se uma viagem não estiver a decorrer */
			/* E houver pessoas suficientes para tal, */
			/* É começada uma viagem */
			if(!viagemEmCurso && nrPessoasEspera >= (sconf.lotacao_carro*2))	//O 4 é o número de pessoas no total 
			{
				/* Mensagem de início de viagem */
				printf("**Início da viagem**\n");						
			
				/* Os clientes já se podem sentar nos carros */
				int j;
				for(j=0;j < (sconf.lotacao_carro*2);j++)
				{
					Assinalar(&sFilaCarros);
					nrPessoasEspera--;
				}
			}
		
			Abrir(&mCarros);
		}
	}
}
void fimDaViagem()
{
	Fechar(&m_nrUtilizadores);
	nrUtilizadoresRestantes -= (sconf.lotacao_carro*2);
	Abrir(&m_nrUtilizadores);

	/* Percorre todos os carros (e os seus lugares) */
	int i;
	for(i=1;i <= (sconf.lotacao_carro*2);i++)
	{
		Fechar(&m_nrUtilizadores);
		
		/* Caso hajam clientes suficientes para a próxima viagem */
		if(nrUtilizadoresRestantes >= (sconf.lotacao_carro*2))
		{
			/* Os lugares dos carros são esvaziados alternadamente */
			
			if((i%2)==0)
			{
				Abrir(&m_nrUtilizadores);
				
				/* É assinalado o carro 1 */
				Assinalar(&sCarro1);
				printf("Assinalou carro 1\n");
			}
			else if((i%2)!=0)
			{
				Abrir(&m_nrUtilizadores);
				
				/* É assinalado o carro 2 */
				Assinalar(&sCarro2);
				printf("Assinalou carro 2\n");
			}
		}
		
		/* Caso contrário */
		else {	Abrir(&m_nrUtilizadores); }
	}
}

/***************************************************************************************************************************/
/**FUNÇÕES AUXILIARES*****************************************************************************************************/
/***************************************************************************************************************************/
void escreverNoMonitor(char message[])
{
	write(s,message,BUFFER_SIZE);
}
void escreverIntNoMonitor(int x)
{
	char message[BUFFER_SIZE];
	sprintf(message,"%i",x);
	escreverNoMonitor(message);
}
