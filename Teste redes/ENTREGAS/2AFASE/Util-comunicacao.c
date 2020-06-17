#include "Header.h"
/*****************************************************************************************************************/
/**COMUNICACAO (geral)*****************************************************************************************/
/*****************************************************************************************************************/
socket_t criarSocket()
{
	socket_t s = socket(AF_UNIX,SOCK_STREAM,0);	//é criado um socket descriptor
	verificarErro(s);							//o programa só continua caso não ocorra erro no passo anterior
	return s;
}
sockaddr_un criarLigacaoSocket()
{
	sockaddr_un socket_addr;					//
	socket_addr.sun_family=AF_UNIX;			//família do tipo AF_UNIX
	strcpy(socket_addr.sun_path,UNIXSTR_PATH);	//é indicado o endereço do espaço partilhado

	return socket_addr;
}
/*****************************************************************************************************************/
/**SIMULADOR (cliente)*******************************************************************************************/
/*****************************************************************************************************************/
socket_t criarSocketCliente()
{
	socket_t s = criarSocket();										//é criada a socket
	sockaddr_un s_addr = criarLigacaoSocket();						//estrutura do endereço do cliente (Simulador)
	
	int connectstate = connect(s,(sockaddr*) &s_addr,sizeof(sockaddr_un));	// é feita a tentativa de conexão
	verificarErro(connectstate);										// o programa só prossegue caso não ocorram erros

	return s;
}
/*****************************************************************************************************************/
/**MONITOR******************************************************************************************************/
/*****************************************************************************************************************/
client_t esperarPorCliente()
{
	socket_t s = criarSocket();									// é criada a socket
	sockaddr_un s_addr = criarLigacaoSocket();					// estrutura de endereço do Monitor
	unlink(UNIXSTR_PATH);										// é apagado o ficheiro partilhado

	int bindstate = bind(s,(sockaddr*) &s_addr,sizeof(sockaddr_un));	// é atribuído o endereço à socket
	verificarErro(bindstate);										// o programa só prossegue caso não ocorram erros
	
	int listenstate = listen(s,1);									// espera pela conexão com 1 cliente (Simulador)
	verificarErro(listenstate);									// o programa só prossegue caso não ocorram erros

	sockaddr_un client_addr;									// estrutura de endereço do cliente (Simulador)
	int clientlen = sizeof(client_addr);
	client_t client = accept(s,(sockaddr*) &client_addr,&clientlen);		// quando ocorra, aceita a conexão com o cliente (Simulador)
	verificarErro(client);										// o programa só prossegue caso não ocorram erros

	return client;
}
