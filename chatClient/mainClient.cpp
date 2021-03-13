#include "../mychat.h"
#include "serverInfo.h"

int main(int argc ,char **argv)
{
    struct sockaddr_in seraddr;
    int sockfd;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &seraddr.sin_addr);

    connect(sockfd, (SA *)&seraddr, sizeof(seraddr));

    char sendline[MAXLINE], recvline[MAXLINE];

    while ( fgets(sendline, MAXLINE, stdin) != NULL)
    {
        cout<<"send: "<<sendline<<endl;
        write(sockfd, sendline, strlen(sendline));

        if(( n=read(sockfd, recvline, MAXLINE)) < 0)
        {
            cout<<"ssss"<<endl;
            exit(0);
        }

        fputs(recvline, stdout);

    }

    return 0;

}