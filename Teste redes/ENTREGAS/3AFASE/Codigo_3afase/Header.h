/**TRABALHO REALIZADO POR***************************/
/* Cláudio Sardinha | Joaquim Perez | Luís Freitas
   (2030215         | 2029015       | 2029715)     */
/***************************************************
/**BIBLIOTECAS*************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>
/***************************************************************************************************************************/
/**CONSTANTES PARA SOCKETS**********************************************************************************************/
#define UNIXSTR_PATH "/tmp/s.unixstr"
#define UNIXDG_PATH  "/tmp/s.unixdgx"
#define UNIXDG_TMP   "/tmp/dgXXXXXXX"
/***************************************************************************************************************************/
/**RENOMEAÇÕES***********************************************************************************************************/
typedef int socket_t;
typedef int client_t;
typedef struct sockaddr_un sockaddr_un;
typedef struct sockaddr sockaddr;
typedef pthread_mutex_t mutex_t;
typedef pthread_t tarefa_t;
typedef sem_t semaforo_t;
/***************************************************************************************************************************/
/**CONSTANTES GERAIS*****************************************************************************************************/
#define FOREVER 1
#define true 1
#define false 0
#define BUFFER_SIZE 255
#define ERROR -1
#define APPEND_MODE "a"
#define READ_MODE "r+"
#define FICHEIRO_LOG "log.log"
#define FICHEIRO_ESTATISTICAS "estatisticas.est"
#define FICHEIRO_SIMULADOR_CONFIG "Simulador.conf"
/***************************************************************************************************************************/
/**ESTRUTURA DO CLIENTE**************************************************************************************************/
typedef struct utilizador
{
	int id;		//Para distinguir uma pessoa de todas as outras
	int *prioritarios;	//não prioritarios (0) e prioritário(1).
	int estado;	//Fila de bilhete (0), fila do carro(1), a andar no carro (2) ou desistiu (3).
	int emViagem;
	int tempoEspGuiche;
	int tempoEspCarros;
	int tempoEspDev;
} utilizador;
/****************************************************************************************************************************/
/**CODIFICAÇÃO DOS EVENTOS***********************************************************************************************/
#define FIM_SIMULACAO 0
#define COMPRA_BILHETE 1
#define DESISTENCIA 2
#define CHEGAR_GUICHE_COMPRA 3
#define INICIO_VIAGEM 4
#define FIM_VIAGEM 5
#define ENTRADA_NO_CARRO 6
#define INICIO_SIMULACAO 7
// (...)
/**VARIÁVEIS GLOBAIS PARA ESTATÍSTICAS************************************************************************************/
static int nrUtilizadores = 0;
mutex_t m_nrUtilizadores;
static int nrDesistencias = 0;
mutex_t m_nrDesistencias;
static int nrViagens = 0;
mutex_t m_nrViagens;
static double tempoMedEspG = 0;
static double tempoMedEspDev = 0;
mutex_t m_tempoMedEspG;
static int tempoViagem = 0;
mutex_t m_tempoViagem;
static int tempoEntSaidaCarros = 0;
mutex_t m_tempoEntSaidaCarros;
static int hora,minutos,segundos,minutosAux;
static int tempoSimul = 0;
static int nrUtilizadoresRestantes;
mutex_t m_nrUtilizadoresRestantes;
static int nrUtilActuais=0;//Mudei aqui
static int nrViagemActual=0;//Mudei aqui
static int nrClientesQueViajaram=0;
static int tempoMedEspCarrosActual=0;
static int nrClientesActuais=0;
/***************************************************************************************************************************/
/**CONFIGURAÇÃO (Simulador)***********************************************************************************************/
typedef struct simulador_config
{
	float taxa_populacao;
	int t_min_viagem;
	int max_pessoas_total;
	float taxa_atendimento_compra;
	float taxa_atendimento_dev;
	float taxa_atendimento_carros;
	int max_pessoas_dev;
	float taxa_desistencia;
	int lotacao_carro;
	int num_carros;
} simulador_config;
static simulador_config sconf;
// Valores 'default' dos parâmetros de configuração do Simulador
#define DEFAULT_TAXA_POPULACAO 0.5
#define DEFAULT_T_MIN_VIAGEM 5
#define DEFAULT_MAX_PESSOAS_TOTAL 10
#define DEFAULT_TAXA_ATENDIMENTO_COMPRA 0.8
#define DEFAULT_TAXA_ATENDIMENTO_DEV 0.8
#define DEFAULT_TAXA_ATENDIMENTO_CARROS 0.8
#define DEFAULT_MAX_PESSOAS_DEV 10
#define DEFAULT_PROB_DESISTENCIA 0.1
#define DEFAULT_LOTACAO_CARRO 5
#define DEFAULT_NUM_CARROS 2
/***************************************************************************************************************************/
/**SIMULADOR - MODELOS DE SINCRONIZAÇÃO********************************************************************************/
/**MODELO - GUICHÉ DE COMPRA************/
void entraClienteGuicheCompraPrio(int id);
void entraClienteGuicheCompraNaoPrio(int id);
void * guicheCompraFunc();
/*******************************************/
/**MODELO - GUICHÉ DE DEVOLUÇÃO********/
int entraClienteDev();
void * filaDev();
#define CLIENTE_DESISTE 1
#define CLIENTE_NAO_DESISTE 0
/*******************************************/
/**MODELO - CARROS ************/
void entraClienteCarros(int id);
void * filaEsperaCarros();
void fimDaViagem();
void criaSCarros(semaforo_t * sCarro);
/*******************************************/
/***************************************************************************************************************************/
/**MONITOR****************************************************************************************************************/
void converteMens(int *terminar,char mensagem[]);
int lerIntDoSimulador();
char* lerDoSimulador();
void escreverEstatisticas();
/***************************************************************************************************************************/
/**SIMULADOR**************************************************************************************************************/
/**CLIENTE*********************************/
int podeCriarCliente();
void criarCliente();
void * cliente_act(void *prio);
#define CLIENTE_E_CRIADO 1
#define CLIENTE_NAO_E_CRIADO 0
int veSeDesiste();
/*******************************************/
void escreverNoMonitor(char message[]);
void escreverIntNoMonitor(int x);
/***************************************************************************************************************************/
/**UTIL-SINCRONIZAÇÃO*****************************************************************************************************/
void inicializarMutex(mutex_t * mutex);
void Fechar(mutex_t * mutex);
void Abrir(mutex_t * mutex);
void destruirMutex(mutex_t * mutex);
void inicializarSemaforo(semaforo_t * sem, int v);
void Esperar(semaforo_t * sem);
void Assinalar(semaforo_t * sem);
void destruirSemaforo(semaforo_t * sem);
/**UTIL-COMUNICAÇÃO******************************************************************************************************/
socket_t criarSocket();
sockaddr_un criarLigacaoSocket();
socket_t criarSocketCliente();
client_t esperarPorCliente();
/***************************************************************************************************************************/
/**UTIL-CONFIGURAÇÃO*****************************************************************************************************/
void lerConfigSimulador(simulador_config * conf);
/***************************************************************************************************************************/
/**UTIL-GERAL*********************************************************************************************************************/
int randWithProb(double prob);
void escreverNoLog(char message[]);
int strequals(char * a, char * b);
void verificarErro(int state);
void delay();
/***************************************************************************************************************************/
