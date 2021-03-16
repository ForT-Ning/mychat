#include "../mychat.h"
#include "../myconst.h"


typedef struct
{
    pid_t   child_pid;
    int     child_pipefd;
    int     child_status;
    long    child_count;
}Child;

class childprocessPoll
{
private:
    static int m_count;
private:
    Child *cptr;
public:
    childprocessPoll(int nCount = -1);
    ~childprocessPoll();

//function here
public:
    Child GetAt(int index);
    int SetAt(int index,Child *clhild);
    static int GetCount() { return m_count;};
};

childprocessPoll::childprocessPoll(int n)
{
    if(n == -1)
        n = FORK_NUM;
    cptr =(Child*)calloc(n, sizeof(Child));
    if(cptr == NULL)
    {
        assert(false);
        printf("cann't creat %d child process", n);
    }
    m_count = n;
}

childprocessPoll::~childprocessPoll()
{
    free(cptr);
    cptr = NULL;
    cout<<"~childprocessPoll"<<endl;
}

Child childprocessPoll::GetAt(int nIndex)
{
    if(nIndex >= m_count)
        return cptr[nIndex - 1];
    return cptr[nIndex];
}

//return -1, can not set;
int childprocessPoll::SetAt(int nIndex,Child* clhild)
{
    if(nIndex >= m_count)
        return -1;
    cptr[nIndex] = *clhild;
    return 0;
}