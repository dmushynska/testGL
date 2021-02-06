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
    explicit ChildWork(int _ms, int _fd, int count) : mS(_ms), fd(_fd), counter(count) {
    }
    ~ChildWork() = default;
    void increaseCounter() {
        while (1) {
            counter++;
            write(fd, &counter, sizeof(counter));
            std::this_thread::sleep_for(std::chrono::milliseconds(mS));
        }
    }
    void printCounter() {
        while (1) {
            std::cout << counter << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    void Process() {
        std::thread tread(&ChildWork::printCounter, this);
        increaseCounter();
        tread.join();
    }
};

class ChildFork {
    pid_t pid;
    int mS;
    int fd[2];
    int counter = 0;

public:
    void Child() {
        if (pid == 0) {
            ChildWork child(mS, fd[1], counter);
            child.Process();
        }
    }
    void newProcess() {
        while (1) {
            pipe(fd);
            pid = fork();
            if (pid == 0) {
                close(fd[0]);
                Child();
            } else {
                close(fd[1]);
                while (!(wait(0) == -1 && errno == ECHILD))
                    ;
                int a;
                while (read(fd[0], &a, sizeof(a)) == 4) {
                    counter = a;
                }
            }
        }
    }
    explicit ChildFork(int _ms) : mS(_ms) {
        newProcess();
    }

    ~ChildFork() {
        kill(pid, SIGTERM);
    };
};

int main(int argc, char **argv) {
    if (argc > 1) {
        const int mS = std::stoi(argv[1]);
        ChildFork child(mS);
    } else {
        std::cout << "[./childfork] [1-1000]";
    }
}