#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "util.h"

int main()
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in clnt_addr;
    bzero(&clnt_addr, sizeof(clnt_addr));

    clnt_addr.sin_family = AF_INET;
    clnt_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    clnt_addr.sin_port = htons(8888);

    connect(sockfd, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr)) ;
    
    while(1) {
        char buf[MAX_BUFFER];
        printf("Input message: ");
        scanf("%s", buf);
        int write_bytes = write(sockfd, buf, sizeof(buf));
        if (write_bytes == -1) {
            errif(1, "write error!!");
        }

        bzero(&buf, sizeof(buf));
        int read_bytes = read(sockfd, buf, sizeof(buf));

        if (read_bytes > 0)
        {
            printf("message from server: %s\n", buf);
        }
        else if (read_bytes == 0)
        { 
            printf("server socket left!!\n");
            break;
        }
        else if (read_bytes == -1)
        { 
            errif(1, "socket read error");
        }
    }

    close(sockfd);

    return 0;
}