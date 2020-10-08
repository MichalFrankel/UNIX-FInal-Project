#ifndef ARGUMENTS_H
#define ARGUMENTS_H
#define BT_BUF_SIZE 100
int nptrs;
void *back_buffer[BT_BUF_SIZE];
struct arguments
{
    char *directory;
    char *ip_address;
};

#endif