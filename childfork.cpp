#include "childfork.h"
ChildWork::ChildWork(int _ms, int _fd, int count) : mS(_ms), fd(_fd), counter(count) {}

void ChildWork::increaseCounter() {
    while (1) {
        counter++;
        write(fd, &counter, sizeof(counter));
        std::this_thread::sleep_for(std::chrono::milliseconds(mS));
    }
}

void ChildWork::printCounter() {
    while (1) {
        std::cout << counter << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ChildWork::Process() {
    std::thread tread(&ChildWork::printCounter, this);
    increaseCounter();
    tread.join();
}

void ChildFork::Child() {
    if (pid == 0) {
        ChildWork child(mS, fd[1], counter);
        child.Process();
    }
}

void ChildFork::newProcess() {
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

ChildFork::ChildFork(int _ms) : mS(_ms) {
    newProcess();
}

ChildFork::~ChildFork() {
    kill(pid, SIGTERM);
};
