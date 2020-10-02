

/*This is the sample program to notify us for the file creation and file deletion takes place in “/tmp” directory*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <time.h>
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

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

int main()
{
    int length, i = 0;
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
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
            return -1;
        }
        fclose(fr);

    /*read to determine the event change happens on “/tmp” directory. Actually this read blocks until the change event occurs*/

    while (1)
    {
        i = 0;

        /*adding the “/tmp” directory into watch list. Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
        wd = inotify_add_watch(fd, "/home/ubuntu/Desktop/Test", IN_ACCESS | IN_MODIFY);

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
                if (event->mask & IN_ACCESS)
                {
                    printf("File %s have been read.\n", event->name);
                    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    fprintf(fp, "Event was created at %d-%02d-%02d %02d:%02d:%02d |%s| Type:READ\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, event->name);
                    fflush(fp);
                    write_to_HTML();
                }
                if (event->mask & IN_MODIFY)
                {
                    printf("File %s have been modified.\n", event->name);
                    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    fprintf(fp, "Event was created at %d-%02d-%02d %02d:%02d:%02d |%s| Type: WRITE\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, event->name);
                    fflush(fp);
                    write_to_HTML();
                }
            }
            i += EVENT_SIZE + event->len;
        }

        /*removing the “/tmp” directory from the watch list.*/
        inotify_rm_watch(fd, wd);

        /*closing the INOTIFY instance*/
    }
    close(fd);
    fclose(fp);
}