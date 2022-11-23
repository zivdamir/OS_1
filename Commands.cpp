#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "assert.h"
#include "time.h"
#include <algorithm>
using namespace std;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::sort;
const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

#define NO_PID_NUMBER 0

//todo for ziv - implement joblist correctly and work on background stuff.(check valgrind)
string _ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string &s) {
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char *cmd_line, char **args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for (std::string s; iss >> s;) {
        args[i] = (char *) malloc(s.length() + 1);
        memset(args[i], 0, s.length() + 1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundCommand(const char *cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char *cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}
// TODO: Add your implementation for classes in Commands.h

/**external command support**/
bool isExternalComplex(string cmd_line) {
    for (char letter: cmd_line) {
        if (letter == '?' || letter == '*') return true;
    }
    return false;
}
/**external command support**/

/* support function for fgcommand*/
int char_to_int(const char* str)
{
    int value;
    int i = 0;
    while(i<80 && str[i] !='\0')
    {
        if(str[i] < '0' || str[i] > '9') return -1; //val[i] isn't a number..
        value *= 10;
        value += str[i] - 48;
    }
    return value;
}
/* support function for fgcommand*/



/**Command class implementation**/
Command::Command(const char *cmd_line) {
    strcpy(this->cmd_line, cmd_line);
    this->arg_num = _parseCommandLine(cmd_line, this->arg);
}

Command::~Command() {
    for (int i = 0; i < arg_num; i++) {
        free(arg[i]);
    }
}

ostream &operator<<(ostream &os, Command &command) {
    os<<string(command.cmd_line);
    return os;
}
/**Command class implementation**/

/**BuiltInCommand class implementation**/
BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line)
{
    this->job_list=SmallShell::getInstance().getJobsList();
}
/**BuiltInCommand class implementation**/

/**ExternalCommand class implementation**/
//ExternalCommand::ExternalCommand(const char* cmd_line): Command(cmd_line) {}
/**ExternalCommand class implementation**/




/**JobEntry methods implementation**/
JobEntry::JobEntry(int id, Command *command, bool stopped_flag) {
    this->id = id;
    this->command=new Command("");
    memcpy(this->command, command, sizeof(*command));
    this->insertion_time = time(NULL);
    this->stopped_flag = stopped_flag;
}

JobEntry::~JobEntry() {

}

int JobEntry::getJobId() {
    return id;
}

JobEntry* JobsList::find_by_jobid(int id,enum FINDSTATUS* find_status){
    /*find_status - to be returned(our function mallocs it , so we should give empty pointers to it*/
    find_status=(enum FINDSTATUS*)malloc(sizeof(*find_status));
    for(JobEntry* job:this->data)
    {
        if(job->getJobId()==id)
        {
            *find_status=FOUND;
            return job;
        }
    }
    *find_status=NOT_FOUND;
    return nullptr;
}
pid_t JobEntry::getJobPid() {
    return getpid();
}

Command *JobEntry::getCommand() {
    return command;
}

bool JobEntry::isStopped() {
    return stopped_flag;
}
void JobEntry::printCommandForFgCommand() {
    cout<<command<< " : " << this->getJobPid() << endl;
}

ostream & operator<<(ostream &os, JobEntry &jobEntry) {
    string stopped=(jobEntry.stopped_flag)? "(stopped)":"";
    //[<job-id>]<-check <command> : <process id> <seconds elapsed> (stopped)
            os <<"["<< jobEntry.id <<"]"<< " " << *jobEntry.command << " : " << jobEntry.getJobPid() << " "
               << difftime(jobEntry.insertion_time,time(NULL))<<" secs"
               << " " << stopped;
    return os;
}
/**JobEntry methods implementation**/


/**JobList methods implementation**/
JobsList::JobsList() {
    data = vector<JobEntry *>();
}

JobsList::~JobsList() {
    for (JobEntry *job: data) {
        delete job;
    }
}

void JobsList::addJob(Command *cmd, bool isStopped)
{
    JobEntry* jobEntry=new JobEntry((this->curr_jobid_max==0)?  1 : this->getMaxJobId()+1
            ,cmd,isStopped);//todo isStopped neccesary?
    this->data.push_back(jobEntry);
    this->curr_jobid_max=getMaxJobId();
}

void JobsList::printJobsList() {
    //JobsList.removeFinishedJobs();
    for (JobEntry *job: data) {
        cout<<*job<<endl;
    }
}

void JobsList::killAllJobs() {
    for (JobEntry *job: data) {

        //kill(job,SIGKILL);
    }
}

void JobsList::removeFinishedJobs() {
    for (JobEntry *job: data) {

        //remove if finished
    }
}

JobEntry *JobsList::getJobById(int jobId,enum FINDSTATUS* findstatus) {
    JobEntry* jobEntry= find_by_jobid(jobId,findstatus);

}

void JobsList::removeJobById(int jobId) {
    /*todo finish removeJobById implementation
     * so the job will be remove from the list but not deleted!!*/
    FINDSTATUS* fd;
    JobEntry* to_find= find_by_jobid(jobId,fd);
    if(*fd==NOT_FOUND)
    {
        cout<<"not found ,TODO"<<endl;
    }
    if(*fd==FOUND)
    {
        cout<<"found, TODO"<<endl;
    }
}

JobEntry *JobsList::getLastJob(int *lastJobId) {
    //return findlast
}

JobEntry *JobsList::getLastStoppedJob(int *jobId) {
    //return find last stopped
}

void JobsList::sort_JobsList() {
    sort(data.begin(),data.end());
}

int JobsList::getMaxJobId() {
    int max_job_id=0;
    for(JobEntry* job:this->data)
    {
        max_job_id=max(max_job_id,job->getJobId());
    }
    return max_job_id;
}

/**JobList methods implementation**/




SmallShell::SmallShell() {
    foreground_pid = NO_PID_NUMBER;
    prompt_name = "smash";
    jobs_list = new JobsList();
}

SmallShell::~SmallShell() {
    delete jobs_list;
}

/**SmallShell our methods implementation**/
void SmallShell::setPromptName(string new_name) {
    prompt_name = new_name;
}

string SmallShell::getPromptName() {
    return prompt_name;
}

pid_t SmallShell::getForegroundPid() {
    return foreground_pid;
}

pid_t SmallShell::getSmallShellPid() {
    return getpid();
}

void SmallShell::setForegroundPid(pid_t new_fg_pid) {
    foreground_pid = new_fg_pid;
}
/**SmallShell our methods implementation**/




/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
**/
Command *SmallShell::CreateCommand(const char *cmd_line) {

    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
    if (firstWord.compare("chprompt") == 0) {
        return new ChpromptCommand(cmd_line);
    } else if (firstWord.compare("pwd") == 0) {
        return new GetCurrDirCommand(cmd_line);
    } else if (firstWord.compare("showpid") == 0) {
        return new ShowPidCommand(cmd_line);
    } else if (firstWord.compare("cd") == 0) {
        return new ChangeDirCommand(cmd_line);
    } else if (firstWord.compare("jobs") == 0) {
        return new JobsCommand(cmd_line);
    } else {
        //home/student/Desktop/external_test_commands/ziv.o
        return new ExternalCommand(cmd_line);
    }/*
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else {
    return new ExternalCommand(cmd_line);
  }*/
    return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line) {
    //start with build in functions
    Command *cmd = CreateCommand(cmd_line);
    cmd->execute();
    delete cmd;
    // TODO: Add your implementation here
    // for example:
    // Command* cmd = CreateCommand(cmd_line);
    // cmd->execute();
    // Please note that you must fork smash process for some commands (e.g., external commands....)
}

GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}

void GetCurrDirCommand::execute() {
    char *path =getcwd(NULL, 0);

    cout << path << std::endl;
    free((void *) path);
    return;
}

ChpromptCommand::ChpromptCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}

void ChpromptCommand::execute() {
    SmallShell &instance = SmallShell::getInstance();
    if (arg_num == 1) {
        instance.setPromptName();
    } else {
        instance.setPromptName(this->arg[1]);
    }
}

ShowPidCommand::ShowPidCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}

void ShowPidCommand::execute() {
    SmallShell &instance = SmallShell::getInstance();
    cout << "smash pid is " << instance.getSmallShellPid() << endl;
}

ChangeDirCommand::ChangeDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {
    // plast_pwd = new string(*plastPwd);
}

ChangeDirCommand::~ChangeDirCommand() {
    // delete plast_pwd;
    // plast_pwd = nullptr;
}

string SmallShell::getLastPwd() {
    return last_pwd;
}

void SmallShell::setLastPwd(string new_last_pwd) {
    last_pwd = new_last_pwd;
}

JobsList *SmallShell::getJobsList() {
    return this->jobs_list;
}

void ChangeDirCommand::execute() {

    //todo check if the command arguments not empty
    //todo for ziv create 'do syscall' and treat the syscall error handling
    assert(this->arg_num >= 0);

    if (this->arg_num > 2) {
        cout << "smash error: cd: too many arguments" << endl;
    }
    SmallShell &instance = SmallShell::getInstance();
    // instance.setLastPwd("ziv-levi");
    if (strcmp(this->arg[1], "-") == 0) {
        if (instance.wasCDCalled == false) {
            cout << "smash error: cd: OLDPWD not set" << endl;
        } else {

            char *to_switch_cwd = getcwd(NULL, 0);

            chdir(instance.getLastPwd().c_str());

            instance.setLastPwd(string(to_switch_cwd));
            free(to_switch_cwd);

            //test
            cout << instance.getLastPwd() << endl;
        }
    } else {
        char *to_switch_cwd = getcwd(NULL, 0);
        //we need to check if the syscall works
        instance.wasCDCalled = true;
        instance.setLastPwd(string(to_switch_cwd));

        DO_SYS(chdir(arg[1]));
        free(to_switch_cwd);
        //we need to check if the syscall works

    }
}

ExternalCommand::ExternalCommand(const char *cmd_line) : Command(cmd_line) {}

void ExternalCommand::execute() {
    bool is_background = _isBackgroundCommand(cmd_line);
    if (is_background) {
        _removeBackgroundSign(cmd_line);
    }

    //todo check if the command not empty
    //todo insert to the job list!

    pid_t pid = fork();
    if (pid == 0) // my son
    {
        if (isExternalComplex(string(cmd_line))) {
            DO_SYS(execl("/bin/bash", "bash", "-c", cmd_line, nullptr));
        } else {
            DO_SYS(execv(this->arg[0], this->arg));
        }
    } else // fatha'
    {
        DO_SYS(waitpid(pid, NULL, 0));
    }
}

// todo test the jobs command
JobsCommand::JobsCommand(const char *cmd_line): BuiltInCommand(cmd_line) {}

void JobsCommand::execute() {
    //todo check error cases and treat them
    this->job_list->removeFinishedJobs();
    this->job_list->printJobsList();
}
// todo test the foreground command
ForegroundCommand::ForegroundCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
void ForegroundCommand::execute()
{
    //todo check error cases and treat them
    //todo check char_to_int  function
    this->job_list->removeFinishedJobs();
    FINDSTATUS* status;
    int job_id = char_to_int(arg[1]);

    /*find the job in the job list, remove it and print it*/
    JobEntry* job_to_front = job_list->getJobById(job_id,status);
    //todo check status?
    //todo finish removeJobById implementation
    job_list->removeJobById(job_id);
    job_to_front->printCommandForFgCommand();
    /*----------------------------------------------------*/


    /*tell the process to continue and then wait for it*/
    pid_t job_pid = job_to_front->getJobPid();
    kill(job_pid,SIGCONT);
    waitpid(job_pid,NULL,0);
    /*----------------------------------------------------*/
}





