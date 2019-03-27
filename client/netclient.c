/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define BUF_SIZE 9999

#define PORT "3490" // Порт, к которому подключается клиент

#define MAXDATASIZE 100 // максимальное число байт, принимаемых за один раз

// получение структуры sockaddr, IPv4 или IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char bufin[BUF_SIZE];
    char bufout[BUF_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname \n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s \n", gai_strerror(rv));
        return 1;
    }

    // Проходим через все результаты и соединяемся к первому возможному
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket \n");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect \n");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect \n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s \n", s);

    freeaddrinfo(servinfo); // эта структура больше не нужна

    read(sockfd, bufin, BUF_SIZE-1);
    printf("%s\n",bufin);

    while(1)
    {
	printf(">");
	memset(bufout, 0, BUF_SIZE);
	fgets(bufout, BUF_SIZE-1, stdin);
	write(sockfd, bufout, strlen(bufout));
	memset(bufin, 0, BUF_SIZE);
	read(sockfd, bufin, BUF_SIZE-1);
	printf("%s\n",bufin);
    }

    //if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        //perror("recv \n");
        //exit(1);
    //}

    //buf[numbytes] = ' ';

    //printf("client: received '%s' \n",buf);

    close(sockfd);

    return 0;
}
