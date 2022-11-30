#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	std::cout<<"smash: got ctrl-Z"<<std::endl;
	SmallShell& shell=SmallShell::getInstance();
	char* cmd_line = shell.getFgCommand();
    pid_t fg_pid = shell.getForegroundPid();
    cout <<"foreground pid during ctrl-Z: " << fg_pid << endl;
    if (!(fg_pid==NO_PID_NUMBER))
	{
		shell.getJobsList()->addJob(cmd_line,fg_pid,true);
		kill(fg_pid,SIGSTOP);
		//shell.setFgCommand(nullptr);
		std::cout<< "smash: process " << fg_pid << " was stopped"<<std::endl;
	}
    cout <<"after if fg_pid: " << fg_pid << endl;

    // TODO: Add your implementation
    /*
    cout << "smash: got ctrl-Z" << endl;
    SmallShell &instance = SmallShell::getInstance();
    JobEntry* Fg_Job = instance.getFgJob();
    if (Fg_Job==nullptr)
    {
        kill(Fg_Job->getJobPid(),SIGSTOP);
        cout<< "smash: process "<< Fg_Job->getJobPid() <<" was killed" << endl;
        JobsList* jobs_list = instance.getJobsList();
        jobs_list->addJob(Fg_Job);
        instance.setFgJob(nullptr);
    }*/
}

void ctrlCHandler(int sig_num) {
  // TODO: Add your implementation
    cout << "smash: got ctrl-C" << endl;
    SmallShell& instance = SmallShell::getInstance();
    pid_t fg_pid = instance.getForegroundPid();
    if (!(fg_pid==NO_PID_NUMBER))
    {
        kill(fg_pid,SIGKILL);
        cout<< "smash: process "<< fg_pid <<" was killed" << endl;
    }
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

