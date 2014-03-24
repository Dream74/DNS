/* 
 * File:   dns.h
 * Author: Dream
 *
 * Created on 2013年5月30日, 下午 12:28
 */

#ifndef DNS_H
#define	DNS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <winsock2.h>

#define SERVERPORT 53
#define ASKPORT 22253

#define SERVER_NAME "Dream DNS SERVER"
#define ASKHOST "8.8.8.8"
 
// 同時支援多少連線
#define SERVER_BACKLOG 5

/* the max thread number */
#define SOCKET_MAX_THREADS 10

#define MAX_SOCKET_CONTENT_SIZE (1024 )

typedef struct _Dns DNS; 
typedef struct _Type DNS_TYPE_A ;

void ex_init(void);
void ex_uninit(void);
void ex_start(void);
void Sleepms(int);
int doit(void);

struct {
  unsigned int quit ;
  unsigned int askquit ;
  struct in_addr networkAddr ; 
  struct hostent * phe ;
} ServerInfo;

struct 
{
  SOCKET s ;
  unsigned char * buf ;
  unsigned int len ;
} AskInfo;

struct _Dns {
  unsigned char transactionID[2];
  unsigned char flags[2] ;
  unsigned char questionNum[2] ;
  unsigned char answerNum[2] ;
  unsigned char authorityNum[2] ;
  unsigned char additionalNum[2] ;
};

struct _TypeA{
  unsigned char name[256] ;
  unsigned char type[2] ;
  unsigned char clas[2] ;
};
#ifdef	__cplusplus
}
#endif

#endif	/* DNS_H */

