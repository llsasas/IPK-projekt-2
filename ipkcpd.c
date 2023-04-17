/**
 * @file ipkcpd.c
 * @author Samuel Cus (xcussa00@fit.vutbr.cz)
 * @brief  IPK Project 2
 * @date 2023-04-15
 */

#include <arpa/inet.h>
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

#define MAX_STACK_SIZE 100

int stack[MAX_STACK_SIZE];
int top = -1;

/**
 * @brief Function used for handling ctrlc
 * @param num int value
 * @return void
 */
void handlectrlc(int num)
{
    exit(num);
}

/**
 * @brief Function used for checking whether it is valid operator
 * @param ch operator itself
 * @return 1 if it is correct operator or 0 if it is invalid
 */
int is_operator(char ch)
{
    switch (ch)
    {
    case '+':
        return 1;
    case '-':
        return 1;
    case '*':
        return 1;
    case '/':
        return 1;
    }
    return 0;
}

/**
 * @brief Function used for pushing items on the stack 
 * @param operation what operation will be performed
 * @param operand1 first operand
 * @param operand2 second operand
 * @return void
 */
int perform_operation(char operation, int operand1, int operand2)
{
    switch (operation)
    {
    case '+':
        return operand1 + operand2;
    case '-':
        return operand1 - operand2;
    case '*':
        return operand1 * operand2;
    case '/':
        return operand1 / operand2;
    default:
        return -1;
    }
}
/**
 * @brief Function used for pushing items on the stack 
 * @param item item that will be pushed
 * @return void
 */
void push(int item)
{
    if (top >= MAX_STACK_SIZE - 1)
    {
        printf("Error: Stack overflow\n");
        exit(0);
    }
    else
    {
        stack[++top] = item;
    }
}

/**
 * @brief Function used for popping items from the stack top
 * @return void
 */
int pop()
{
    if (top < 0)
    {
        printf("Error: Stack underflow\n");
        exit(0);
    }
    else
    {
        return stack[top--];
    }
}

/**
 * @brief Function used for handling expression evaluation for both UDP and TCP
 * @param expr expression that will be evaluated
 * @return -1 if error occurs or the result of the exprssion
 */
int evaluate_prefix_expression(char *expr)
{
    int i, operand1, operand2, result, oper;
    char c;
    int cnt = 0;
    bool sw = false;
    for (i = strlen(expr) - 1; i >= 0; i--)
    {
        c = expr[i];
        if (isdigit(c))
        {
            push(c - '0');
        }
        else if (is_operator(c))
        {
            push(c);
        }
        else if (c == ')')
        {
            cnt++;
            push(c);
        }
        else if (c == '(')
        {
            while (true)
            {
                if (!sw)
                {
                    oper = pop();
                    sw = true;
                }
                operand2 = pop();
                operand1 = pop();
                result = perform_operation(oper, operand1, operand2); // odstranit operátor z vrcholu zásobníku
                if (stack[top] == ')')
                {
                    pop();
                    cnt--;
                }
                push(result); // výsledek uložit zpět na zásobník
                if (cnt == 0)
                {
                    break;
                }
                c = stack[top - 1] + '0';

                while (isdigit(c))
                {
                    operand2 = pop();
                    operand1 = pop();
                    result = perform_operation(oper, operand1, operand2);
                    push(result);
                    c = stack[top - 1] + '0';
                }
                break;
            }
            sw = false;
        }
        else if (c == ' ')
        {
            continue;
        }
        else
        {
            perror("Error: Invalid character\n");
            return -1;
        }
    }
    result = pop();
    if (stack[top] == ')')
    {
        pop();
        cnt--;
    }
    if (top >= 0)
    {
        perror("Error: Invalid expression\n");
        return -1;
    }
    return result;
}

char *extract_substring(char *str)
{
    char *start = strchr(str, '('); // find the first '(' character
    char *end = strrchr(str, ')');  // find the last ')' character
    char *result = NULL;

    if (start != NULL && end != NULL)
    {
        result = (char *)malloc(end - start + 2); // allocate memory for the result string
        strncpy(result, start, end - start + 1);  // copy the substring to the result array
        result[end - start + 1] = '\0';           // add null terminator to the result array
    }

    return result;
}

/**
 * @brief Function that checks if arguments are being passed the correct way
 * @param argn number of arguments that were passed to the 'main' function
 * @param arga array of arguments that were passed to the 'main' function
 * @param h pointer used for storing the index of hostname in the argv
 * @param p pointer used for storing the index of port in the argv
 * @param m pointer used for storing the index of mode in the argv
 * @return 0 if everything is okay, 1 if error occurrs during the programme
 */
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
                perror("Usage: ipkcpc -h <host> -p <port> -m <mode>\n");
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

/**
 * @brief Function used for handling socker creation for both UDP and TCP
 * @param type defines whether we use TCP(1) or UDP(0)
 * @return socket number used further in code
 */
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

/**
 * @brief Function used for checking if port is within given range
 * @param port_n represents port number
 * @return 0 void
 */
void check_portnumber(int port_n)
{
    if (port_n > 65535 || port_n < 0)
    {
        perror("Error: port number out of range");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Function used for bind for both TCP and UDP communication
 * @param socket_n represents socket number
 * @param prtnum represents port number
 * @return 0 void
 */
void fnc_bind(int prtnum, int socket_n)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(prtnum);
    if (bind(socket_n, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Function used for handling listen during TCP
 * @param socketn represents socket number
 * @return 0 void
 */
void listenfnc(int socketn)
{
    int max_waiting_connections = 1;
    if (listen(socketn, max_waiting_connections) < 0)
    {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Function used for handling TCP communication
 * @param socketn represents socket number
 * @param prtnum represents port number
 * @return 0 void
 */
void tcp_communication(int socketn, int prtnum)
{
    struct sockaddr_in comm_addr;
    socklen_t comm_addr_size = sizeof(comm_addr);
    bool error = false;
    char buffer[BUFSIZE];
    char response[BUFSIZE];
    int buffer_len = 0;
    int flags = 0;

    while (true)
    {
        int comm_socket = accept(socketn, (struct sockaddr *)&comm_addr, &comm_addr_size);
        if (comm_socket < 0)
        {
            perror("286");
            error = true;
            break;
        }
        while (true)
        {
            int bytes_rx = recv(comm_socket, buffer, BUFSIZE, flags);
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
                        int result = evaluate_prefix_expression(extract_substring(message));
                        perror("314 ");
                        printf("%d \n", result);
                        if (result == -1)
                        {
                            perror("Error occurred: evaluating expression");
                            error = true;
                            strcpy(response, "BYE\n");
                            break;
                        }
                        sprintf(response, "RESULT %d\n", result);
                    }
                    else
                    {
                        perror("Invalid message sent");
                        strcpy(response, "BYE\n");
                        error = true;
                    }
                    // Send response
                    int bytes_tx = send(comm_socket, response, strlen(response), flags);
                    if (bytes_tx <= 0)
                    {
                        perror("Error occurred: send");
                        error = true;
                        break;
                    }

                    break;
                }
                if (error)
                {
                    break;
                }
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
            break;
        }
    }
    if (error)
    {
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Function used for handling UDP communication
 * @param socketn represents socket number
 * @return 0 void
 */
void udp_communication(int socketn)
{
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    struct sockaddr *addr = (struct sockaddr *)&client_addr;
    bool error = false;
    char buff[BUFSIZE];
    char send[BUFSIZE];
    char out[BUFSIZE];
    while (1)
    {
        int bytes_rx = recvfrom(socketn, buff, BUFSIZE, 0, addr, &addr_size);
        if (bytes_rx < 0)
        {
            perror("ERROR: recvfrom");
            error = true;
            break;
        }
        if ((buff[0]) != 0)
        {
            perror("Error occurred: response from client");
            error = true;
            break;
        }
        int result = evaluate_prefix_expression(buff + 3);
        sprintf(out, "%d\n", result);
        out[0] = '4';
        out[1] = '\0';
        send[0] = 1;
        send[1] = 0;
        send[2] = 1;
        strcat(send + 3, out);
        int bytes_tx = sendto(socketn, send, strlen(send), 0, addr, addr_size);
        if (bytes_tx < 0)
        {
            perror("ERROR: sendto");
            error = true;
            break;
        }
    }
    if (error)
    {
        exit(EXIT_FAILURE);
    }
    exit(0)
}

/**
 * @brief Main function
 * @param argc number of arguments
 * @param argv array of arguments
 * @return 0 if everything is okay, 1 if error occurrs during the programme
 */
int main(int argc, const char *argv[])
{
    const char *server_hostname;
    int h = 0;
    int p = 0;
    int m = 0;
    int portnumber, socket_f;
    arguments_check(argc, argv, &h, &p, &m);
    int mode = !strcmp(argv[m], "tcp");
    portnumber = atoi(argv[p]);
    server_hostname = argv[h];
    check_portnumber(portnumber);
    socket_f = create_socket(mode);
    if (socket_f <= 0)
    {
        perror("Error: socket");
        return (EXIT_FAILURE);
    }
    if (true)
    {
        int enable = 1;
        setsockopt(socket_f, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
        fnc_bind(portnumber, socket_f);
        listenfnc(socket_f);
        tcp_communication(socket_f, portnumber);
    }

    return 0;
}