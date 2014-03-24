/* 
 * File:   main.c
 * Author: Dream
 *
 * Created on 2013年5月30日, 上午 12:33
 */
#include <stdio.h>
#include <stdlib.h>

#include "dns.h"
#include "misc.h"

int main(int argc, char** argv) {
  ex_init();
  int macIP = doit();
  if (macIP) {
    printf("Get Network Card Fail");
    return (EXIT_FAILURE);
  } else {
    int select, i;
    do {
      i = 0;
      while (ServerInfo.phe->h_addr_list[i] != 0) {
        struct in_addr addr;
        memcpy(&addr, ServerInfo.phe->h_addr_list[i], sizeof (struct in_addr));
        printf("Address %d: %s\r\n", i, inet_ntoa(addr));
        ++i;
      }
      printf("Please Select Bind Network IP:[0-%d]\r\n", i - 1);

#ifdef _DEBUG
      select = 0 ;
#else 
      scanf("%d", &select);
#endif

    } while (select >= i);

    memcpy(&ServerInfo.networkAddr, ServerInfo.phe->h_addr_list[select], sizeof (struct in_addr));
    printf("You Select Bind Network:%s\r\n", inet_ntoa(ServerInfo.networkAddr));
  }

  ex_start();

  char buf[16];
  printf("Hello " SERVER_NAME "!!\r\n" );
  while (scanf("%16s", buf) && strncmp(buf, "quit", 4)) {
    Sleepms(500);
  }
  ex_uninit();
  system("pause");
  return (EXIT_SUCCESS);
}

