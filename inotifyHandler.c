

/*This is the sample program to notify us for the file creation and file deletion takes place in “/tmp” directory*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <time.h>
#include "arguments.h"
#include "udpClient.c"
#include <execinfo.h>
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
char date[1024];

void  __attribute__ ((no_instrument_function))  __cyg_profile_func_enter (void *this_fn,
                                         void *call_site)
{
  	nptrs = backtrace(back_buffer, BT_BUF_SIZE);
}

void  __attribute__ ((no_instrument_function))  __cyg_profile_func_exit (void *this_fn,
                                         void *call_site)
{
    nptrs = backtrace(back_buffer, BT_BUF_SIZE);
}

void formatDateToString()
{   
   time_t rawtime;
   struct tm *info;
   time( &rawtime );

   info = localtime( &rawtime );

   strftime(date,80,"%d %B %Y: %H:%S", info);

}
int write_to_HTML()
{
    char *line = NULL;
    //char* buffer=line;
    FILE *fr;
    size_t len = 0;
    ssize_t read;

    FILE *fp = fopen("events.txt", "r");
    if (fp == NULL)
        return -1;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        printf("DEBUG:%s", line); //DEBUG
        fr = fopen("/var/www/html/index.html", "a+");
        if (fr == NULL)
        {
            printf("Could not write to index.html webrowser\n");
            return -1;
        }
        else
        {
            fputs(line, fr);
            fputs("<br>", fr); // New line
            
        }
    }
    fclose(fp);
    fclose(fr);
    return 0;
}

void* inotifyHandler(void* args)
{
    struct arguments* inotify_args=(struct arguments*)args;
    int length, i = 0;
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];

    FILE *fp = fopen("events.txt", "w");
    /*creating the INOTIFY instance*/
    fd = inotify_init();

    /*checking for error*/
    if (fd < 0)
    {
        perror("inotify_init");
    }
        
        FILE* fr = fopen("/var/www/html/index.html", "w+");//Clean html file from previous content
        if (fr == NULL)
        {
            printf("Could not write to index.html webrowser\n");
            return NULL; 
        }
        fclose(fr);
    
    udpClientConnect(inotify_args); //connect to server


    /*read to determine the event change happens on the chosen directory. Actually this read blocks until the change event occurs*/

    while (1)
    {
        i = 0;

        /*adding the chosen directory into watch list. Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
        wd = inotify_add_watch(fd,inotify_args->directory, IN_ACCESS | IN_MODIFY);
        length = read(fd, buffer, EVENT_BUF_LEN);

        /*checking for error*/
        if (length < 0)
        {
            perror("read");
        }

        /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
        while (i < length)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len)
            {
                
                formatDateToString();
                if (event->mask & IN_ACCESS)
                {
                    printf("File %s have been read.\n", event->name);
                    fprintf(fp, "Event was created at %s |%s| Type:READ\n",date, event->name);
                    fflush(fp);
                    write_to_HTML();
                    sendMessageToRemoteServer(event->name,"NO_WRITE",date);
                }
                if (event->mask & IN_MODIFY)
                {
                    printf("File %s have been modified.\n", event->name);
                    fprintf(fp, "Event was created at %s |%s| Type:WRITE\n",date,event->name);
                    fflush(fp);
                    write_to_HTML();
                    sendMessageToRemoteServer(event->name,"WRITE",date);
                }
            }
            i += EVENT_SIZE + event->len;
        }

        /*removing the chosen directory from the watch list.*/
        inotify_rm_watch(fd, wd);

        /*closing the INOTIFY instance*/
    }
    closeSocket(); 
    close(fd);
    fclose(fp);
}