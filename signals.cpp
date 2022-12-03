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
	if(fg_pid ==NO_PID_NUMBER)
	{
		return;
	}
    if (!(fg_pid==NO_PID_NUMBER))
	{
        FINDSTATUS fg_found_in_list;
        JobEntry* fg_job_in_list = shell.getJobsList()->getJobByPid(fg_pid,fg_found_in_list);
        if(fg_found_in_list==FOUND)
        {
            fg_job_in_list->setJobInsertionTime();
            fg_job_in_list->stopJob();
        }
        else
        {
            shell.getJobsList()->addJob(cmd_line,fg_pid,true);
        }
		shell.setForegroundPid(NO_PID_NUMBER);
		if(kill(fg_pid,SIGSTOP)==-1)
        {
            perror("smash error: kill failed");
            exit(1);
        }
		std::cout<< "smash: process " << fg_pid << " was stopped" << std::endl;

	}
	return;

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

    cout << "smash: got ctrl-C" << endl;
    SmallShell& instance = SmallShell::getInstance();
    pid_t fg_pid = instance.getForegroundPid();
    if (!(fg_pid==NO_PID_NUMBER))
    {
        if(kill(fg_pid,SIGKILL)==-1)
        {
            perror("smash error: kill failed");
            exit(1);
        }
        cout<< "smash: process "<< fg_pid <<" was killed" << endl;
    }
	instance.setForegroundPid(NO_PID_NUMBER);

}



/*
class TimedJob {
private:
    pid_t pid;
    char cmd_line[80]={0};
    time_t timeout;
public:
    TimedJob(char* cmd_line,pid_t pid)
    {
        this->cmd_line = cmd_line;
        this->pid = pid;
        timeout = ?;
    }
}
class TimedJobsList {
private:
    std::vector<JobEntry *> data;

}
void alarmHandler(int sig_num) {
    std::cout<<"smash: got an alarm"<<std::endl;
    SmallShell& shell = SmallShell::getInstance();

    JobsList* timed_jobs = shell.getJobsList();
    for(JobEntry* job : timed_jobs->getData())
    {
        if(job->isTimed())
        {
            kill(job->getJobPid(),SIGKILL);
            std::cout<<"smash: timeout "<<job->getCommand()<<" timed out!"<<std::endl;
        }
    }
}

TimeoutCommand::TimeoutCommand(const char* cmd_line) {}
void TimeoutCommand::execute()
{
    SmallShell& shell = SmallShell::getInstance();
    char* real_cmd_line = this->cutFirstWordFromCmdLine();
    Command* cmd = shell.CreateCommand(real_cmd_line);
    cmd->execute();
    delete cmd;

}*/


