#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "arguments.h"
#include <libcli.h>

int display_backtrace(struct cli_def *cli, char *name, char **argv, int argc)
{
  char** strings; 
  cli_print(cli, "Backtrace from InotifyHandler:\n");
  strings = backtrace_symbols(back_buffer, nptrs);
  if (strings == NULL)
  {
    perror("backtrace_symbols");
    exit(EXIT_FAILURE);
  }

  for (int j = 0; j < nptrs; j++)
  {
    cli_print(cli, strings[j]);
  }

  free(strings);
  return 0;
}

void *telnetHandler(void *args)
{
  struct cli_def *cli;
  int sockfd;
  int acceptfd;
  struct sockaddr_in saddr, raddr;
  unsigned int on = 1;
  unsigned int rlen = sizeof(struct sockaddr_in);
  cli = cli_init();
  cli_register_command(cli, NULL, "backtrace", display_backtrace,
                       PRIVILEGE_UNPRIVILEGED, MODE_ANY, "retrieves the backtrace");
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    exit(-1);
  }
  saddr.sin_port = htons(12345);
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  memset(&raddr, 0, sizeof(raddr));
  raddr.sin_family = AF_INET;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0)
  {
    perror("setsockopt");
    exit(-1);
  }
  if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
  {
    perror("bind");
    exit(-1);
  }
  if (listen(sockfd, 12) != 0)
  {
    perror("listen");
    exit(-1);
  }
  while ((acceptfd = accept(sockfd, (struct sockaddr *)&raddr, &rlen)) > 0)
  {
    while (cli_loop(cli, acceptfd) == 0)
      ;
  }
}
