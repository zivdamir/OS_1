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
		shell.getJobsList()->addJob(cmd,"",pid,true);//todo replace "" with real cmd_line.
		kill(pid,sig_num);
		shell.setFgCommand(nullptr);
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

/*
#define IDENTICAL 0

void JobsList::printAllJobsForQuitCommand()
{
    for(JobEntry* job : data)
    {
        cout << job->getJobPid() << ": " << *(job->getCommand()) << endl;
    }
}
void JobsList::killAllJobs()
{
    for(JobEntry* job : data)
    {
    kill(job->pid,SIGKILL);
    }
}

QuitCommand::QuitCommand(const char *cmd_line) {};
void QuitCommand::execute()
{
    if(arg_num>1 && strcmp(arg[1],"kill")==IDENTICAL)
    {
        job_list->removeFinishedJobs();
        cout<< "smash: sending SIGKILL signal to "<< job_list->getListSize() <<" jobs:" <<endl;
        job_list->printAllJobsForQuitCommand();
        this->job_list->killAllJobs();
    }
    exit(1); //what argument to send?
}
*/
