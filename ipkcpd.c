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
#include <netinet/in.h>

void arguments_check(int argn, const char *arga[], int *h, int *p, int *m)
{
    bool swp = false;
    bool swh = false;
    bool swm = false;
    if (argn != 7)
    {
        perror('Usage: ipkcpc -h <host> -p <port> -m <mode>\n');
        exit(EXIT_FAILURE);
    }
    for (int i = 1; i < 7; i++)
    {
        if (strcmp(arga[i], "-h") == 0)
        {
            *h = i + 1;
            if (swm == true && swp == true)
            {
                return;
            }
            swh = true;
            continue;
        }
        if (strcmp(arga[i], "-p") == 0)
        {
            *p = i + 1;
            if (swh == true && swm == true)
            {
                return;
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
                    return;
            }
            else
            {
                exit(EXIT_FAILURE);
            }
        }
    }
    if (swh != true || swp != true || swm != true)
    {
        perror('Usage: ipkcpc -h <host> -p <port> -m <mode>\n');
        exit(EXIT_FAILURE);
    }
    return;
}

// Type defines whether we use TCP(1) or UDP(0)
int create_socket(int type)
{
    int family = AF_INET;
    if (type)
    {
        int type = SOCK_STREAM;
        int welcome_socket = socket(family, type, 0);
        return socket(family, type, 0);
    }
    else
    {
        int type = SOCK_DGRAM;
        return socket(family, type, 0);
    }
}

void check_portnumber(int port_n)
{
    if (port_n > 65535 || port_n < 0)
    {
        perror('Error: port number out of range');
        exit(EXIT_FAILURE);
    }
}

void fnc_bind(int prtnum, int socket_n)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(prtnum);
    struct sockaddr *address = (struct sockaddr *)&server_addr;
    int address_size = sizeof(server_addr);
    if (bind(socket_n, &address, address_size) < 0)
    {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }
}
int main(int argc, const char *argv[])
{
    const char *server_hostname;
    int h = 0;
    int p = 0;
    int m = 0;
    int portnumber, socket_f;
    int mode = !strcmp(argv[m], "tcp");
    arguments_check(argc, argv, &h, &p, &m);
    portnumber = atoi(argv[p]);
    server_hostname = argv[h];
    check_portnumber(portnumber);
    socket_f = create_socket(mode);
    if (socket_f <= 0)
    {
        perror("Error: socket");
        return (EXIT_FAILURE);
    }
    

    return 0;
}