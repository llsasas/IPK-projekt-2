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
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

#define BUFSIZE 512

typedef struct {
    char data[100];
    int top;
} Stack;

void push(Stack* s, char ch) {
    s->top++;
    s->data[s->top] = ch;
}

char pop(Stack* s) {
    char ch = s->data[s->top];
    s->top--;
    return ch;
}

int is_operator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}


void arguments_check(int argn, const char *arga[], int *h, int *p, int *m)
{
    bool swp = false;
    bool swh = false;
    bool swm = false;
    if (argn != 7)
    {
        perror("Usage: ipkcpc -h <host> -p <port> -m <mode>\n");
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
        perror("Usage: ipkcpc -h <host> -p <port> -m <mode>\n");
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
        perror("Error: port number out of range");
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
    if (bind(socket_n, (struct sockaddr *)&address, address_size) < 0)
    {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }
}

void listenfnc(int socketn)
{
    int max_waiting_connections = 1;
    if (listen(socketn, max_waiting_connections) < 0)
    {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }
}


void tcp_communication(int socketn)
{
    struct sockaddr *comm_addr;
    socklen_t comm_addr_size;
    bool error = false;
    char buffer[BUFSIZE];
    char response[BUFSIZE];
    int buffer_len = 0;
    while (true)
    {
        int comm_socket = accept(socketn, comm_addr, &comm_addr_size);
        if (comm_socket > 0)
        {
            int flags = 0;
            int bytes_rx = recv(comm_socket, buffer + buffer_len, BUFSIZE - buffer_len, flags);
            if (bytes_rx <= 0)
            {
                perror("Error occurred: recv");
                error = true;
                break;
            }
            buffer_len += bytes_rx;
            if (strchr(buffer, '\n') != NULL)
            {
                // Complete message received
                char *message = strtok(buffer, "\n");
                while (message != NULL)
                {
                    // Process message
                    if (strcmp(message, "HELLO") == 0)
                    {
                        strcpy(response, "HELLO\n");
                    }
                    else if (strcmp(message, "BYE") == 0)
                    {
                        strcpy(response, "BYE\n");
                    }
                    else if (!strncmp(message, "SOLVE ", 6))
                    {
                     
                    }
                    // Send response
                    int bytes_tx = send(comm_socket, response, strlen(response), flags);
                    if (bytes_tx <= 0)
                    {
                        perror("Error occurred: send");
                        error = true;
                        break;
                    }
                    // Get next message
                    message = strtok(NULL, "\n");
                }
                // Move remaining data to the beginning of buffer
                buffer_len = strlen(message);
                memmove(buffer, message, buffer_len);
            }
        }
        shutdown(comm_socket, SHUT_RDWR);
        if (close(comm_socket) < 0)
        {
            perror("Error: close socket");
            exit(EXIT_FAILURE);
        }
        if (error)
        {
            exit(EXIT_FAILURE);
        }
    }
}

void udp_communication(int socketn)
{
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    struct sockaddr *addr = (struct sockaddr *)&client_addr;
    char buffer[BUFSIZE];
    while (1)
    {
        int bytes_rx = recvfrom(socketn, buffer, BUFSIZE, 0, addr, &addr_size);
        if (bytes_rx < 0)
        {
            perror("ERROR: recvfrom");
        }
        int bytes_tx = sendto(socketn, buffer, strlen(buffer), 0, addr, addr_size);
        if (bytes_tx < 0)
        {
            perror("ERROR: sendto");
        }
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
    if (mode)
    {
        int enable = 1;
        setsockopt(socket_f, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    }

    return 0;
}