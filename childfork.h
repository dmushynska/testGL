#include <signal.h>
#include <sys/shm.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <thread>

class ChildWork {
    int mS;
    int fd;
    int counter = 0;

public:
    explicit ChildWork(int _ms, int _fd, int count);
    ~ChildWork() = default;
    void increaseCounter();
    void printCounter();
    void Process();
};

class ChildFork {
    pid_t pid;
    int mS;
    int fd[2];
    int counter = 0;

public:
    void Child();
    void newProcess();
    explicit ChildFork(int _ms);

    ~ChildFork();
};
