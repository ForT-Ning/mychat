#include "../mychat.h"
#include "../myconst.h"
#include "childprocess.h"
#include "transferDescriptor.h"

void child_make(int, int, Child *);
void child_main(int , int, int);
void  sig_int(int);
int childprocessPoll::m_count = 0;
childprocessPoll childPolls;
int nCount = childPolls.GetCount();

int main(int argc, char *argv[])
{
    int listenfd, connfd, maxfd;
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

    FD_ZERO(&masterset);
    FD_SET(listenfd, &masterset);
    maxfd = listenfd;
    for(int ii = 0; ii < nCount; ii++)
    {
        Child child;
        child_make(listenfd, len, &child);
        FD_SET(child.child_pipefd, &masterset);
        childPolls.SetAt(ii, &child);
        maxfd = max(maxfd, child.child_pipefd);
    }

    signal(SIGINT, sig_int);
    
    int vavail = nCount;
    for(;;)
    {
        rset = masterset;
        if(vavail <= 0)
        {
            FD_CLR(listenfd, &rset);
        }

        int nsel = select(maxfd + 1, &rset, NULL, NULL, NULL);
        int ii, n, rc;
        if(FD_ISSET(listenfd, &rset))
        {
            socklen_t clilen = len;
            connfd = accept(listenfd, (SA *)&cliaddr, &clilen);
            cout<<"accept from"<<inet_ntoa(cliaddr.sin_addr)<<endl;
            for( ii = 0; ii < nCount; ++ii)
            {
                if(childPolls.GetAt(ii).child_status == 0)
                    break;
            }
            if(ii == nCount)
            {
                cout<<"only "<<nCount<<" child processes"<<endl;
                exit(0);
            }
            auto chil = childPolls.GetAt(ii);
            chil.child_status = 1;
            chil.child_count++;
            vavail--;

            n = write_fd(chil.child_pipefd, (void*)"", 1, connfd);
            close(connfd);
            if(--nsel == 0)
                continue;
        }

        for(ii = 0; ii < nCount; ++ii)
        {
            auto clid = childPolls.GetAt(ii);
            if(FD_ISSET(clid.child_pipefd, &rset))
            {
                if( (n = read(clid.child_pipefd, &rc, 1)) == 0)
                {
                    cout<<"some error"<<endl;
                    exit(0);
                }
                clid.child_status = 0;
                vavail++;
                if(--nsel == 0)
                    break;
            }
        }
    }
    close(listenfd);
    return 0;
}

void sig_int(int signo)
{
     for(int ii = 0; ii < nCount; ii++)
    {
        kill(childPolls.GetAt(ii).child_pid, SIGTERM);
        cout<<"kill Child: "<<childPolls.GetAt(ii).child_pid<<endl;
    }

    while(wait(NULL) > 0)
        ;
    if(errno != ECHILD)
    {
        cout<<"wait error"<<endl;
        exit(0);
    }
    exit(0);
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
    //close(sockfd[1]);
    close(listenfd);
    child_main(listenfd, addrlen, sockfd[1]);
}

void child_main(int listenfd, int addrlen, int fd)
{
    char c;
    int connfd;
    ssize_t n;

    cout<<"child starting: "<<getpid()<<endl;
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    for(;;)
    {
        select(fd + 1, &rset, NULL, NULL, NULL);
        if(FD_ISSET(fd, &rset))
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
}