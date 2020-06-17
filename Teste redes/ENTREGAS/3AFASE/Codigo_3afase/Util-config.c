#include "Header.h"

/****************************************************/
/**LEITURA DO FICHEIRO DE CONFIGURAÇÃO DO SIMULADOR**/
/****************************************************/
void lerConfigSimulador(simulador_config * conf)
{
	/* É aberto o ficheiro de configuração */
	FILE * fp = fopen(FICHEIRO_SIMULADOR_CONFIG,READ_MODE);

	/* O simulador toma os valores 'default' */
	conf->taxa_populacao = DEFAULT_TAXA_POPULACAO;
	conf->t_min_viagem = DEFAULT_T_MIN_VIAGEM;
	conf->max_pessoas_total = DEFAULT_MAX_PESSOAS_TOTAL;
	conf->taxa_atendimento_compra = DEFAULT_TAXA_ATENDIMENTO_COMPRA;
	conf->taxa_atendimento_dev = DEFAULT_TAXA_ATENDIMENTO_DEV;
	conf->taxa_atendimento_carros = DEFAULT_TAXA_ATENDIMENTO_CARROS;
	conf->max_pessoas_dev = DEFAULT_MAX_PESSOAS_DEV;
	conf->taxa_desistencia = DEFAULT_PROB_DESISTENCIA;
	conf->lotacao_carro = DEFAULT_LOTACAO_CARRO;
	conf->num_carros = DEFAULT_NUM_CARROS;

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

		if(strequals(param,"t_min_viagem"))
		{
			conf->t_min_viagem = atoi(value);
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
		}else{
		
		if(strequals(param,"num_carros"))
		{
			conf->num_carros = atof(value);
		}
		
		}}}}}}}}}

		/* Limpeza de vetores */
		bzero(param,sizeof(param));
		bzero(value,sizeof(value));
		bzero(buffer,sizeof(buffer));
	}
}
