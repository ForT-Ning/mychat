#include "../mychat.h"
#include "../myconst.h"
#include "childprocess.h"
#include "transferDescriptor.h"

void child_make(int, int, Child *);
void child_main(int , int, int);
void  sig_int(int);

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in seraddr, cliaddr;
    fd_set rset, masterset;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(SERVERPORT);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (SA *)&seraddr, sizeof(seraddr));
    listen(listenfd, LISTENQ);
    len = sizeof(seraddr);

    childprocessPoll childPolls;
    for(int ii = 0; ii < FORK_NUM; ii++)
    {
        Child child;
        child_make(listenfd, len, &child);
        FD_SET(child.child_pipefd, &masterset);
        childPolls.SetAt(ii, &child);
    }

    //signal(SIGINT, sig_int);


    return 0;
}

void child_make(int listenfd, int addrlen, Child *child)
{
    int sockfd[2];
    pid_t pid;

    socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

    if( (pid = fork()) > 0 )
    {
        child->child_pid = pid;
        child->child_pipefd = sockfd[0];
        child->child_status = 0;//unused
        return; //parent return
    }

    //dup2(sockfd[1], STDERR_FILENO);
    close(sockfd[0]);
    close(sockfd[1]);
    close(listenfd);
    child_main(listenfd, addrlen, sockfd[1]);
}

void child_main(int listenfd, int addrlen, int fd)
{
    char c;
    int connfd;
    ssize_t n;

    cout<<"child starting: "<<getpid()<<endl;

    for(;;)
    {
        if( (n = read_fd(fd, &c, 1, &connfd)) == 0)
        { 
            cout<<"read_fd returned 0"<<endl;
            exit(-1);
        }
        if(connfd < 0)
        {
            cout<<"no descriptor from read_fd"<<endl;
            exit(-1);
        }
        char buff[MAXLINE];
        while( (n = read(connfd, buff, MAXLINE)) > 0)
        {
            //cout<<"read from: "<<inet_ntoa(cliaddr.sin_addr)<<endl;
            cout<<"Child process :"<<getpid()<<endl;
            cout<<"Message: "<<buff<<endl;
            write(connfd, buff, n);
        }
        close(connfd);
        write(fd,"", 1);
    }
}