#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/inotify.h>
#include "inotifyHandler.c"
#include "telnetHandler.c"
#include "arguments.h"
#include <libcli.h>
#define BT_BUF_SIZE 100

struct arguments inotify_args;

int main(int argc, char *argv[])
{
    char *ip_add = NULL;
    char *directory = NULL;
    char *args[2];
    pthread_t tid_telnet;
    pthread_t tid_inotify;
    char opt;

    while ((opt = getopt(argc, argv, "i:d:")) != -1)
    {
        switch (opt)
        {
        case 'd':
        {
            if (opendir(optarg) == NULL)
            {
                printf("Error: Directory does not exist.");
                return -1;
            }
            directory = optarg;
            break;
        }
        case 'i':
        {
            ip_add = optarg;
            break;
        }
        }
    }

    if (directory && ip_add)
    {

        inotify_args.directory = directory;
        inotify_args.ip_address = ip_add;
        if (pthread_create(&tid_inotify, NULL, inotifyHandler, (void *)&inotify_args)) //Thread Notify Handler
            return 1;
        if (pthread_create(&tid_telnet, NULL, telnetHandler, (void *)&inotify_args)) //Thread Telnet
            return 1;
    }
    pthread_join(tid_inotify, NULL);
    pthread_join(tid_telnet, NULL);
    return 0;
}