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
BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line) {}
/**BuiltInCommand class implementation**/

/**ExternalCommand class implementation**/
//ExternalCommand::ExternalCommand(const char* cmd_line): Command(cmd_line) {}
/**ExternalCommand class implementation**/




/**JobEntry methods implementation**/
JobEntry::JobEntry(int id, Command *command, bool stopped_flag) {
    this->id = id;
    memcpy(this->command, command, sizeof(*command));
    this->insertion_time = time(NULL);
    this->stopped_flag = stopped_flag;
}

JobEntry::~JobEntry() {}

int JobEntry::getJobId() {
    return id;
}
JobEntry* JobsList::find_by_jobid(int id){
    for(JobEntry* job:this->data)
    {
        if(job->getJobId()==id)
        {
            return job;
        }
    }
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

ostream & operator<<(ostream &os, JobEntry &jobEntry) {
    //[<job-id>]<-check <command> : <process id> <seconds elapsed> (stopped)
            os<<jobEntry.id<<" "<<jobEntry.command<<" : "<<jobEntry.getJobPid()<<" "
            <<difftime(jobEntry.insertion_time,time(NULL))
            <<" "<<jobEntry.stopped_flag?"(stopped)":"";
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

void JobsList::addJob(Command *cmd, bool isStopped) {
    //first, try to find..
    JobEntry* jobEntry=new JobEntry(getpid(),cmd,isStopped);//todo isStopped neccesary?
    this->data.push_back(jobEntry);
}

void JobsList::printJobsList() {
    //JobsList.removeFinishedJobs();
    for (JobEntry *job: data) {
        cout<<job;
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

JobEntry *JobsList::getJobById(int jobId) {
    return find_by_jobid(jobId);
}

void JobsList::removeJobById(int jobId) {
    //delete this->find
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

/**JobList methods implementation**/




SmallShell::SmallShell() {
    foreground_pid = NO_PID_NUMBER;
    prompt_name = "smash";
    // jobs_list = new JobsList();
}

SmallShell::~SmallShell() {
    //delete jobs_list;
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
    char *path = getcwd(NULL, 0);
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

        chdir(arg[1]);
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
            execl("/bin/bash", "bash", "-c", cmd_line, nullptr);
        } else {
            execv(this->arg[0], this->arg);
        }
    } else // fatha'
    {
        waitpid(pid, NULL, 0);
    }
}




