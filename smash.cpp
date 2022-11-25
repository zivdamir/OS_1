#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"
#include "assert.h"

#define NO_PID_NUMBER 0
#define NO_ID_NUMBER -1

int main(int argc, char* argv[]) {
    //testing for joblist
    // JobsList job_list=JobsList();
    // printf("%d\n ",job_list.getMaxJobId());
    //ShowPidCommand cmd=ShowPidCommand("command1");
    // job_list.addJob(&cmd,true);
    //  assert(job_list.getMaxJobId()==1);
    //  ShowPidCommand cmd2=ShowPidCommand("command2");
    // job_list.addJob(&cmd2,false);
    //  assert(job_list.getMaxJobId()==2);

    // job_list.printJobsList();

    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    //TODO: setup sig alarm handler

    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        std::cout << smash.getPromptName()<<"> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        std::cout << cmd_line<< std::endl; // TODO DELETE
        std::cout << cmd_line.c_str()<< std::endl; // TODO DELETE
        smash.executeCommand(cmd_line.c_str());
    }
    return 0;
}