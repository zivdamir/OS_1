#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
    // TODO: Add your implementation
    cout << "smash: got ctrl-C" << endl;
    SmallShell &instance = SmallShell::getInstance();
    pid_t fg_pid = instance.getForegroundPid();
    if (fg_pid==NO){}
}

void ctrlCHandler(int sig_num) {
  // TODO: Add your implementation
    /*if(there is a process in the foreground)
    {
        SmallShell& small_shell_inst = SmallShell::getInstance();
        kill(small_shell_inst.foreground_pid, SIGKILL);
    }*/
    printf("no ctrlCHandler implemented\n");
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

