#include "../mychat.h"

#define LISTENQ 80 //allow max connect
#define SERVERPORT 12356//server port
#define FORK_NUM 10//process pool

pid_t child_make(int , int , int );
void sig_int(int signo);
static pid_t *pids;

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

    pids = (pid_t *)calloc(FORK_NUM, sizeof(pid_t));
    for(int ii = 0; ii < FORK_NUM; ii++)
    {
       pids[ii] = child_make(ii, listenfd, sizeof(seraddr));
    }
    signal(SIGINT, sig_int);
    for(;;)
    {
        pause();

    }
    

    return 0;
}

void sig_int(int signo)
{
    for(int ii = 0; ii < FORK_NUM; ii++)
    {
        kill(pids[ii], SIGTERM);
        cout<<"kill Child: "<<pids[ii]<<endl;
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

pid_t child_make(int i, int listenfd, int addrlen)
{   
    pid_t pid;
    void child_main(int, int, int);
    if( (pid = fork()) > 0)
        return (pid);
    
    child_main(i, listenfd, addrlen);
    printf("The child %ld stop", (long)getpid());
    exit(0);
}

void child_main(int i, int listenfd, int addrlen)
{
    int connfd;
    socklen_t len;
    struct sockaddr_in cliaddr;
    printf("child %ld starting\n", (long)getpid());
    int n;
    char buff[MAXLINE];
    for(;;)
    {
        len = addrlen;
        connfd = accept(listenfd, (SA *)&cliaddr, &len);
        printf("chlid %ld accept\n", (long)getpid());
        while( (n = read(connfd,buff, MAXLINE)) > 0)
        {
             cout<<inet_ntoa(cliaddr.sin_addr)<<": "<<buff<<endl;
             write(connfd, buff , strlen(buff));
        }
        close(connfd);
    }
}