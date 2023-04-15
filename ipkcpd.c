/**
 * @file ipkcpd.c
 * @author Samuel Cus (xcussa00@fit.vutbr.cz)
 * @brief  IPK Project 2
 * @date 2023-04-15
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>


int arguments_check(int argn, const char *arga[], int *h, int *p, int *m)
{
    bool swp = false;
    bool swh = false;
    bool swm = false;
    if (argn != 7)
    {
        return 1;
    }
    for (int i = 1; i < 7; i++)
    {
        if (strcmp(arga[i], "-h") == 0)
        {
            *h = i + 1;
            if (swm == true && swp == true)
            {
                return 0;
            }
            swh = true;
            continue;
        }
        if (strcmp(arga[i], "-p") == 0)
        {
            *p = i + 1;
            if (swh == true && swm == true)
            {
                return 0;
            }
            else
            {
                swp = true;
                continue;
            }
        }
        if (strcmp(arga[i], "-m") == 0)
        {
            if (strcmp(arga[i + 1], "tcp") == 0 || strcmp(arga[i + 1], "udp") == 0)
            {
                *m = i + 1;
                swm = true;
                if (swh == true && swp == true)
                    return 0;
            }
            else
            {
                return 1;
            }
        }
    }
    if (swh != true || swp != true || swm != true)
    {
        return 1;
    }
    return 0;
}

// Type defines whether we use TCP(1) or UDP(0)
int create_socket(int type)
{
    int family = AF_INET;
    if(type)
    {
        int type = SOCK_STREAM;
        return socket(family, type, 0);
    }
    else
    {
        int type = SOCK_DGRAM;
        return socket(family, type, 0);
    }
}


int main(int argc, const char *argv[])
{

    return 0;
}