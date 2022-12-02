#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"
#include "assert.h"

int main(int argc, char* argv[]) {
    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }
    /*struct sigaction SA = {alarmHandler,NULL,SA_RESTART,NULL};
    if(sigaction(SIGALRM, &SA, NULL)==-1) {
        perror("smash error: failed to set alarm handler");
    }*/

    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        std::cout << smash.getPromptName()<<"> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
		if(_trim(cmd_line)=="")
		{
			continue;}
        smash.executeCommand(cmd_line.c_str());
    }
    return 0;
}