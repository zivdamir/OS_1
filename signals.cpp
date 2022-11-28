#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	std::cout<<"smash: got ctrl-Z"<<std::endl;
	SmallShell& shell=SmallShell::getInstance();
	Command* cmd=shell.getFgCommand();
	if(cmd!=nullptr)
	{
		pid_t pid=shell.getForegroundPid();
		shell.getJobsList()->addJob(cmd,cmd->getCmdLine(),pid,true);//todo replace "" with real cmd_line.
		kill(pid,sig_num);
		//shell.setFgCommand(nullptr);
		std::cout<<"smash: process "<<pid<<" was stopped"<<std::endl;
	}
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
    /*cout << "smash: got ctrl-C" << endl;
    SmallShell &instance = SmallShell::getInstance();
    pid_t fg_pid = instance.getForegroundPid();
    if (!(fg_pid==NO_PID_NUMBER))
    {
        kill(fg_pid,SIGKILL);
        cout<< "smash: process "<< fg_pid <<" was killed" << endl;
    }*/
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

