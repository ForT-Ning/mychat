#include "../mychat.h"

#define LISTENQ 80
#define SERVERPORT 12356

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    socklen_t len;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    char buff[MAXLINE];

    struct sockaddr_in seraddr, cliaddr;
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(SERVERPORT);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (SA *)&seraddr, sizeof(seraddr));

    listen(listenfd, LISTENQ);

    for(;;)
    {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *)&cliaddr, &len);

        read(connfd, buff, MAXLINE);

        cout<<cliaddr.sin_addr.s_addr<<": "<<buff<<endl;

        write(connfd, buff, strlen(buff));

    }
    

    return 0;
}