#include "Header.h"
/**************************************************/
/**LEITURA DO FICHEIRO DE CONFIGURAÇÃO DO MONITOR**/
/**************************************************/
void lerConfigMonitor(monitor_config * conf)
{
	/* É aberto o ficheiro de configuração */
	FILE * fp = fopen(FICHEIRO_MONITOR_CONFIG,"r+");

	/* O monitor toma os valores 'default' */
	conf->t_viagem = DEFAULT_T_VIAGEM;
	conf->max_pessoas = DEFAULT_MAX_PESSOAS_TOTAL;

	/* Vetores auxiliares para o 'parse' */
	char buffer[BUFFER_SIZE];
	char param[BUFFER_SIZE];
	char value[BUFFER_SIZE];

	int i;

	/* Percorre todo o ficheiro */
	while(fgets(buffer,BUFFER_SIZE,fp))
	{
		/* Limpeza de vetores */
		bzero(param,sizeof(param));
		bzero(value,sizeof(value));	
	
		/* Percorre até o '=' */
		for(i = 0 ; buffer[i] != '=' ; i++);

		/* Copia o nome do parâmetro */
		strncpy(param,buffer,i);	
		
		/* Copia o valor do parâmetro */
		strncpy(value,buffer+i+1,strlen(buffer)-i-1);

		/* Consoante o parâmetro encontrado no ficheiro */
		/* É atribuído o valor encontrado no mesmo */
		if(strequals(param,"t_viagem"))
		{
			conf->t_viagem = atoi(value); 
		}else{
		
		if(strequals(param,"max_pessoas"))
		{
			conf->max_pessoas = atoi(value);
		}else{
		
			//...
		}
		}

		/* Limpeza de vetores */
		bzero(param,sizeof(param));
		bzero(value,sizeof(value));
		bzero(buffer,sizeof(buffer));
	}
}

/****************************************************/
/**LEITURA DO FICHEIRO DE CONFIGURAÇÃO DO SIMULADOR**/
/****************************************************/
void lerConfigSimulador(simulador_config * conf)
{
	/* É aberto o ficheiro de configuração */
	FILE * fp = fopen(FICHEIRO_SIMULADOR_CONFIG,"r+");

	/* O simulador toma os valores 'default' */
	conf->taxa_populacao = DEFAULT_TAXA_POPULACAO;
	conf->t_simulacao = DEFAULT_T_SIMULACAO;
	conf->t_viagem = DEFAULT_T_VIAGEM;
	conf->max_pessoas_total = DEFAULT_MAX_PESSOAS_TOTAL;
	conf->taxa_atendimento_compra = DEFAULT_TAXA_ATENDIMENTO_COMPRA;
	conf->taxa_atendimento_dev = DEFAULT_TAXA_ATENDIMENTO_DEV;
	conf->taxa_atendimento_carros = DEFAULT_TAXA_ATENDIMENTO_CARROS;
	conf->max_pessoas_dev = DEFAULT_MAX_PESSOAS_DEV;
	conf->taxa_desistencia = DEFAULT_PROB_DESISTENCIA;
	conf->lotacao_carro = DEFAULT_LOTACAO_CARRO;

	/* Vetores auxiliares para o 'parse' */
	char buffer[BUFFER_SIZE];
	char param[BUFFER_SIZE];
	char value[BUFFER_SIZE];

	int i;

	while(fgets(buffer,BUFFER_SIZE,fp))
	{
		/* Limpeza de vetores */	
		bzero(param,sizeof(param));
		bzero(value,sizeof(value));
	
		/* Percorre até o '=' */
		for(i = 0 ; buffer[i] != '=' ; i++);	

		/* Copia o nome do parâmetro */
		strncpy(param,buffer,i);	
		
		/* Copia o valor do parâmetro */
		strncpy(value,buffer+i+1,strlen(buffer)-i-1);

		/* Consoante o parâmetro encontrado no ficheiro */
		/* É atribuído o valor encontrado no mesmo */
		if(strequals(param,"taxa_populacao"))
		{
			conf->taxa_populacao = atof(value); 
		}else{
		
		if(strequals(param,"t_simulacao"))
		{
			conf->t_simulacao = atoi(value);		
			// (...)
		}else{
		
		if(strequals(param,"t_viagem"))
		{
			conf->t_viagem = atoi(value); 
		}else{
		
		if(strequals(param,"max_pessoas_total"))
		{
			conf->max_pessoas_total = atoi(value);
		}else{
		
		if(strequals(param,"taxa_atendimento_compra"))
		{
			conf->taxa_atendimento_compra = atof(value);
		}else{
		
		if(strequals(param,"taxa_atendimento_dev"))
		{
			conf->taxa_atendimento_dev = atof(value);
		}else{
		
		if(strequals(param,"max_pessoas_dev"))
		{
			conf->max_pessoas_dev = atof(value);
		}else{
		
		if(strequals(param,"taxa_desistencia"))
		{
			conf->taxa_desistencia = atof(value);
		}else{
		
		if(strequals(param,"lotacao_carro"))
		{
			conf->lotacao_carro = atoi(value);
		}else{
		
		if(strequals(param,"taxa_atendimento_carros"))
		{
			conf->taxa_atendimento_carros = atof(value);
		} }}}}}}}}}

		/* Limpeza de vetores */
		bzero(param,sizeof(param));
		bzero(value,sizeof(value));
		bzero(buffer,sizeof(buffer));
	}
}
