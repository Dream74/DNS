#include <stdio.h>
#include <string.h>
#include "dns.h"

int doit(void) {
  char ac[80], tmp[100];
  if (gethostname(ac, sizeof (ac)) == SOCKET_ERROR) {
    sprintf(tmp, "Error %d when getting local host name.\r\n", WSAGetLastError());
    perror(tmp);
    return 1;
  }
  sprintf(tmp, "Host name is %s.\r\n", ac);
  printf(tmp);

  struct hostent *phe = gethostbyname(ac);
  if (phe == 0) {
    printf("Yow! Bad host lookup.\r\n");
    return 1;
  }

  ServerInfo.phe = phe;
  return 0;
}

void WSA_init(void) {
  /*** Initial WSA ***/
  int WSAState;
  WSADATA wsaData;

  WSAState = WSAStartup(MAKEWORD(2, 2), (LPWSADATA) & wsaData);
  if (WSAState) {
    char meserr[64];
    sprintf(meserr, "Initial WSA Error!, error code:%d\n", WSAState);
    perror(meserr);
    exit(EXIT_FAILURE);
  }
}

void X02printf(unsigned char * buf, int len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("%02x ", buf[i]);
  }
  printf("\r\n");
}

SOCKET ex_socket_init(int port) {
  /*** Create a Socket ***/
  SOCKET s;
  s= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s== SOCKET_ERROR) {
    perror("Error occurred in socket()");
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  /*** Inital host address and port ***/
  struct sockaddr_in local, cli_addr;

  local.sin_family = AF_INET;
  //  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_addr.s_addr = inet_addr(inet_ntoa(ServerInfo.networkAddr));
  local.sin_port = htons(port);

  /*** bind Socket ***/
  if (bind(s, (struct sockaddr*) &local, sizeof (local)) == SOCKET_ERROR) {
    perror("Error occurred in bind()");
    WSACleanup();
    closesocket(s);
    exit(EXIT_FAILURE);
  }

  return s;

}

void dns_ask(void){
  static SOCKADDR_IN toServer;
  memset((char *) &toServer, 0, sizeof (toServer));
  toServer.sin_family = AF_INET;
  toServer.sin_port = htons(SERVERPORT);
  toServer.sin_addr.S_un.S_addr = inet_addr(ASKHOST);
  DBG("ask Len:%d\r\n", AskInfo.len);
  ServerInfo.askquit = 0;
  for (;;) {
    if (ServerInfo.askquit) {
      break;
    }

    sendto(AskInfo.s, AskInfo.buf, AskInfo.len, 0, (LPSOCKADDR) & toServer, sizeof (toServer));
    Sleepms(1000) ;
   }
}

int dns_askServer(SOCKET s, unsigned char * questbuff,unsigned int questLen, unsigned char * buff) {
  static SOCKADDR_IN client_addr;
  memset((char *) &client_addr, 0, sizeof (client_addr));
  int clientLen = sizeof ( client_addr);

  DBG("askServer Len:%d\r\n", questLen);
  AskInfo.s = s ;
  AskInfo.buf = questbuff ;
  AskInfo.len = questLen ;
  CreateThread(NULL, 0, (void *) dns_ask, (void *)NULL , 0, NULL);

  unsigned int ret;

  ret = recvfrom(s,
          buff,
          MAX_SOCKET_CONTENT_SIZE - 1,
          0,
          (LPSOCKADDR) & client_addr,
          &clientLen);

  ServerInfo.askquit = 1 ;
  return ret;
}

void dns_start(void) {
  SOCKET dnsServer, dnsAsk;
  dnsServer = ex_socket_init(SERVERPORT);
  dnsAsk = ex_socket_init(ASKPORT);

  unsigned char buff[MAX_SOCKET_CONTENT_SIZE], sendbuff[MAX_SOCKET_CONTENT_SIZE];
  SOCKADDR_IN client_addr;
  memset((char *) &client_addr, 0, sizeof (client_addr));
  unsigned int clientLen = sizeof ( client_addr), ret;
  for (;;) {
    if (ServerInfo.quit) {
      break;
    }
    ret = recvfrom(dnsServer,
            (unsigned char *) &buff,
            MAX_SOCKET_CONTENT_SIZE - 1,
            0,
            (LPSOCKADDR) & client_addr,
            &clientLen);
    if (ret == SOCKET_ERROR) {
      printf("Error\r\n");
      ServerInfo.quit = 1;
      break;
    }
    DBG("#Connected from %s:%d.\r\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    DBG("Len :%d\r\n", ret);
    ret = dns_askServer(dnsAsk, (unsigned char*) &buff, ret, (unsigned char*) &sendbuff);
    sendto(dnsServer, (unsigned char *)&sendbuff, ret, 0, (LPSOCKADDR) & client_addr, clientLen);
  }
  /*** close socket ***/
  closesocket(dnsServer);
  closesocket(dnsAsk);
  WSACleanup();
}

void ex_init(void) {
  WSA_init();
  ServerInfo.quit = 0;
}

void ex_start(void) {
  CreateThread(NULL, 0, (void *) dns_start, (void *) NULL, 0, NULL);
}

void ex_uninit(void) {
  ServerInfo.quit = 1;
}

void Sleepms(int ms) {
  Sleep(ms);
}