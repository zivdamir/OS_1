#include <unistd.h>
#include <string.h>
#include <fcntl.h>
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

enum PARAMSTATUS{NO_GOOD=0,GOOD=1};


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

/**external command support**/
//int _parseCommandLine(const char *cmd_line, char **args);
//todo maybe add indicator to which one of the redirection symbols we found...for this momemnt we only test if we can spot it..

/*

bool check_if_redirection_command(const char* cmd_line){
   string str_cmd_line = string(cmd_line);
   for(char letter: str_cmd_line)
   {
      if(letter == '>'){
          return true;
      }
   }
   return false;
    /* int arg_count=0;
    char* cmd_args[COMMAND_MAX_ARGS];
    arg_count = _parseCommandLine(cmd_line,cmd_args);
    bool is_redirection=false;
    bool is_append=false;
    bool is_overwrite=false;
    for(int i=0;i<arg_count;i++)
    {
        if(strcmp(cmd_args[i],">")==0 || strcmp(cmd_args[i],">>")==0)
        {
            is_redirection=true;
            break;
        }
    }
    for (int i = 0; i < arg_count; i++) {
        free(cmd_args[i]);
    }
    return is_redirection;*/
/*}
string _parseFirstPipeCommand(string cmd,int* i)
{
    *i=0;
    string a="";
    for(char s: cmd)
    {
        if(s =='|')
        {
            break;
        }
        else{
            a+=s;
            (*i)++;
        }
    }

    return a;
}
string _ParseSecondPipeCommand(string cmd,int pos,PIPE_CMD_TYPE* pipeCmdType)
{

    string a="";
    bool pipe_hit=false;
    bool amp_after_pipe_hit=false;
    if(cmd[pos+1]!='&') //regular |
    {
        *pipeCmdType=PIPE_STDOUT;
        for (char ch: cmd) {
            if (ch == '|') {
                pipe_hit = true;
                continue;
            }
            if (pipe_hit == true) {
                a += ch;
            }
        }
    }
    else{
        *pipeCmdType=PIPE_STDERR;
        for(char ch: cmd)
        {
            if(ch=='|'){
                pipe_hit=true;
                continue;
            }
            if(pipe_hit==true)
            {
                if(ch=='&')
                {
                    amp_after_pipe_hit=true;
                    continue;
                }
            }
            if(amp_after_pipe_hit)
            {
                a+=ch;
            }
        }
    }
    //cout<<"second cmd " << a<< endl;



    return a;
}
string _parseFirstRedirectionCommand(string cmd,int * i)
{
    *i=0;
    string a="";
    for(char s: cmd)
    {
        if(s =='>')
        {
            break;
        }
        else{
            a+=s;
            (*i)++;
        }
    }
    return a;
}
// we can use check_if_redirection that does recognize cmd_type better, TODO
string  _ParseSecondRedirectionCommand(string cmd,int pos,REDIRECTION_CMD_TYPE* cmd_type){
    string a="";
    bool arrow_hit=false;
    bool scnd_arrow_hit=false;
    if(cmd[pos+1]!='>') //regular >
    {
        *cmd_type=REDIRECTION_OVERWRITE;
        for (char ch: cmd) {
            if (ch == '>') {
                arrow_hit = true;
                continue;
            }
            if (arrow_hit == true) {
                a += ch;
            }
        }
    }
    else if(cmd[pos+1]=='>'){
        *cmd_type=REDIRECTION_APPEND;
        for(char ch: cmd)
        {
            if(ch=='>'&& arrow_hit== false){
                arrow_hit=true;
                continue;
            }
            if(arrow_hit == true)
            {
                if(ch=='>')
                {
                    //cout<< "scnd arrow hitted"<<endl;
                    scnd_arrow_hit=true;
                    continue;
                }
            }
            if(scnd_arrow_hit)
            {
                a+=ch;
            }
        }
    }
    return a;
}

bool check_if_pipe_command(const char* cmd_line)
{
    string str_cmd_line = string(cmd_line);
    for(char letter: str_cmd_line)
    {
        if(letter == '|'){
            return true;
        }
    }
    return false;
    /*
    int arg_count=0;
    char* cmd_args[COMMAND_MAX_ARGS];
    arg_count= _parseCommandLine(cmd_line,cmd_args);
    bool is_pipe=false;
    for(int i=0;i<arg_count;i++) {
        if (strcmp(cmd_args[i], "|") == 0 || strcmp(cmd_args[i], "|&") == 0)
        {
            is_pipe=true;
            break;
        }
    }
    for (int i = 0; i < arg_count; i++) {
        free(cmd_args[i]);
    }
    return is_pipe;*/
/*}*/

string _parseCommandLinePipeFirst(string cmd_line, bool *is_stderr) {
    if (cmd_line.find("|&") != string::npos){
        *is_stderr = true;
        return cmd_line.substr(0, cmd_line.find("|&"));
    }
    else { //' | ' is in cmd_line
        *is_stderr = false;
        return cmd_line.substr(0, cmd_line.find("|"));
    }
}

string _parseCommandLinePipeSecond(string cmd_line){
    if (cmd_line.find("|&") != std::string::npos){
        return cmd_line.substr(cmd_line.find("|&")+2);
    }
    else { //'|' is in cmd_line
        return cmd_line.substr(cmd_line.find("|")+1);
    }
}

string _parseCommandLineRedirectionFirst(string cmd_line, bool *is_append) {
    if (cmd_line.find(">>") != string::npos){
        *is_append = true;
        return cmd_line.substr(0, cmd_line.find(">>"));
    }
    else { //' > ' is in cmd_line
        *is_append = false;
        return cmd_line.substr(0, cmd_line.find(">"));
    }
}

string _parseCommandLineRedirectionSecond(string cmd_line){
    if (cmd_line.find(">>") != std::string::npos){
        return cmd_line.substr(cmd_line.find(">>")+2);
    }
    else { //'|' is in cmd_line
        return cmd_line.substr(cmd_line.find(">")+1);
    }
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


/*the char_to_int function assumes that the given argument is a number*/
int char_to_int(const char* str)
{
    int value = 0;
    string temp = str;
    bool first_letter = true;
    for(char letter: temp)
    {
        if(first_letter && letter == '-')
        {
            value = -value;
            continue;
        }
        value *= 10;
        value += letter - 48;
        first_letter = false;
    }
    return value;
}
/* support function for fgcommand*/

/**Command class implementation**/
Command::Command(const char* cmd_line) {
    strcpy(this->cmd_line, cmd_line);

    char temp[COMMAND_MAX_LENGTH];
    strcpy(temp, cmd_line);
    _removeBackgroundSign(temp); //todo check if cause problems
    this->arg_num = _parseCommandLine(temp, this->arg);
    this->is_background = _isBackgroundCommand(cmd_line);
    this->job_list = SmallShell::getInstance().getJobsList();

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
 char *Command::getCmdLine()  {
	return cmd_line;
}

/**BuiltInCommand class implementation**/
BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line){}

/**JobEntry class implementation**/
JobEntry::JobEntry(int id,pid_t pid,char cmd_line[80], bool stopped_flag)
{
    this->id = id;
    this->pid = pid;
    strcpy(this->cmd_line,cmd_line);
    this->insertion_time = time(NULL);
    this->stop_time = time(NULL);
    this->stopped_flag = stopped_flag;
}
JobEntry::~JobEntry() {}
int JobEntry::getJobId() {
    return id;
}
pid_t JobEntry::getJobPid() {
    return pid;
}
char *JobEntry::getCommand() {
    return this->cmd_line;
}
bool JobEntry::isStopped() {
    return stopped_flag;
}
void JobEntry::printCommandForFgAndBgCommand() {
    cout<< this->getCommand() << " : " << this->getJobPid() << endl;
}
void JobEntry::setJobStoppingTime() {
    stop_time = time(NULL);
}
time_t JobEntry::getJobStoppingTime()
{
    return stop_time;
}
ostream & operator<<(ostream &os, JobEntry &jobEntry) {
    string stopped = (jobEntry.stopped_flag)? "(stopped)":"";
     os <<"["<< jobEntry.id <<"]"<< " " << string(jobEntry.cmd_line) << " : " << jobEntry.getJobPid()
     << " " << difftime(time(NULL),jobEntry.insertion_time)<<" secs" << " " << stopped;
    return os;
}
void JobEntry::stopJob()
{
    this->stopped_flag = true;
    setJobStoppingTime();
}
void JobEntry::continueJob()
{
    this->stopped_flag = false;
}



/**JobList class implementation**/
JobsList::JobsList() {
    data = vector<JobEntry *>();
}
JobsList::~JobsList() {
    for (JobEntry *job: data) {
        delete job;
    }
}
void JobsList::addJob(char cmd_line[80], pid_t job_pid, bool isStopped)
{
    JobEntry* jobEntry = new JobEntry((this->data.size())? this->getMaxJobId()+1 : 1
            ,job_pid,cmd_line,isStopped);//todo isStopped neccesary?
    this->data.push_back(jobEntry);
   // this->curr_job_id_max=getMaxJobId();
}
void JobsList::printJobsList() {
    removeFinishedJobs();
    for (JobEntry *job: data) {
        cout<<*job<<endl;
    }
}
bool isFinished(JobEntry *job)
{
     if(waitpid((*job).getJobPid(), nullptr, WNOHANG)) {
         return true;
     }
     return false;
}
void JobsList::removeFinishedJobs()
{
    for(auto iterator = data.begin(); iterator != data.end();)
    {
        int son_is_potent = waitpid((*iterator)->getJobPid(), nullptr, WNOHANG);
        if (son_is_potent) { // alive and strong, very powerful son very potent
            data.erase(iterator);
        }else{
                iterator++;

        }
    }
}
JobEntry *JobsList::getJobById(int job_id,enum FINDSTATUS& find_status) {
    for (JobEntry *job: this->data) {
        if (job->getJobId() == job_id) {
            find_status = FOUND;
            return job;
        }
    }
    find_status = NOT_FOUND;
    return nullptr;
}
void JobsList::removeJobById(int jobId)
{
    for(auto iterator = data.begin(); iterator != data.end();)
    {
        if((*iterator)->getJobId() == jobId)
        {
            data.erase(iterator);
        }
    }

}
JobEntry* JobsList::getLastStoppedJob()
{
    time_t last_stopped_job_time;
    JobEntry* last_stopped_job = nullptr;
    for(JobEntry* job: this->data)
    {
        if(job->isStopped())
        {
            if(last_stopped_job == nullptr || last_stopped_job_time < job->getJobStoppingTime())
            {
                last_stopped_job = job;
                last_stopped_job_time = job->getJobStoppingTime();
            }
        }
    }
    return last_stopped_job;
}
void JobsList::sort_JobsList() {
    sort(data.begin(),data.end());
}
int JobsList::getMaxJobId() {
    int max_job_id=NO_ID_NUMBER;
    for(JobEntry* job:this->data)
    {
        max_job_id=max(max_job_id,job->getJobId());
    }
    return max_job_id;
}

/**SmallShell class implementation**/
SmallShell::SmallShell() {
    foreground_pid = NO_PID_NUMBER;
    prompt_name = "smash";
    jobs_list = new JobsList();
}
SmallShell::~SmallShell() {
    delete jobs_list;
}
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
Command* SmallShell::CreateCommand(const char *cmd_line) {

    bool is_cmd_pipe = check_if_pipe_command(cmd_line);
    bool is_cmd_redirection = check_if_redirection_command(cmd_line);

    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    /** ignore the & sign **/
    _removeBackgroundSign(const_cast<char*>(firstWord.c_str()));
    firstWord = _trim(firstWord.c_str());
    /** ignore the & sign **/

    assert(!(is_cmd_pipe&&is_cmd_redirection));
    //we allow only one "type" of command or none of them, but never both.
    //we first check if its pipe or redirection..always!

    if (is_cmd_pipe){
        return new PipeCommand(cmd_line);
    } else if(is_cmd_redirection) {
        return new RedirectionCommand(cmd_line);
    }else if (firstWord.compare("chprompt") == 0) {
        return new ChpromptCommand(cmd_line);
    } else if (firstWord.compare("pwd") == 0) {
        return new GetCurrDirCommand(cmd_line);
    } else if (firstWord.compare("showpid") == 0) {
        return new ShowPidCommand(cmd_line);
    } else if (firstWord.compare("cd") == 0) {
        return new ChangeDirCommand(cmd_line);
    } else if (firstWord.compare("jobs") == 0) {
        return new JobsCommand(cmd_line);
    }  else if (firstWord.compare("fg") == 0) {
        return new ForegroundCommand(cmd_line);
    }else if (firstWord.compare("bg") == 0) {
        return new BackgroundCommand(cmd_line);
    }else if(firstWord.compare("quit") == 0){
        return new QuitCommand(cmd_line);
    } else if(firstWord.compare("kill") == 0)
    {
        return new KillCommand(cmd_line);
    }else{
        return new ExternalCommand(cmd_line);
    }
    return nullptr;
}
void SmallShell::executeCommand(const char *cmd_line) {
    Command *cmd = CreateCommand(cmd_line);
    cmd->execute();
    delete cmd;
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
char *SmallShell::getFgCommand() {
    return this->fg_command_line;
}
void SmallShell::setFgCommand(char* cmd_line) {
    strcpy(this->fg_command_line,cmd_line);

}

/**Command classes implementation**/

/**GetCurrDirCommand implementation**/
GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}
void GetCurrDirCommand::execute() {
    char *path = getcwd(NULL, 0);

    cout << path << std::endl;
    free((void *) path);
    return;
}
/**ChpromptCommand implementation**/
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

ChangeDirCommand::ChangeDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}
ChangeDirCommand::~ChangeDirCommand(){}
void ChangeDirCommand::execute() {
    if (this->arg_num > 2) {
        cerr << "smash error: cd: too many arguments" << endl;
    }
    SmallShell &instance = SmallShell::getInstance();
    if (strcmp(this->arg[1], "-") == 0) {
        if (instance.wasCDCalled == false) {
            cerr << "smash error: cd: OLDPWD not set" << endl;
        } 
        else {
            char *to_switch_cwd = getcwd(NULL, 0);

            chdir(instance.getLastPwd().c_str());

            instance.setLastPwd(string(to_switch_cwd));
            free(to_switch_cwd);
        }
    } else {
        char *to_switch_cwd = getcwd(NULL, 0);
		if(to_switch_cwd==nullptr)
		{
			perror("smash error: getcwd failed");
			exit(1);
		}
        instance.wasCDCalled = true;
        instance.setLastPwd(string(to_switch_cwd));

        if(chdir(arg[1])==-1)
		{
			perror("smash error: chdir failed");
			exit(1);
		}
        free(to_switch_cwd);
    }
}

/**external command implementation**/
bool isExternalComplex(string cmd_line) {
    for (char letter: cmd_line) {
        if (letter == '?' || letter == '*') return true;
    }
    return false;
}
ExternalCommand::ExternalCommand(const char *cmd_line) : Command(cmd_line) {}
void ExternalCommand::execute() {
  SmallShell& shell=SmallShell::getInstance();
    pid_t child_pid = fork();
	if(child_pid==-1)
	{
		perror("smash error: fork failed");
		exit(1);
	}
    if (child_pid == 0) // my son
    {
		setpgrp();
        if (isExternalComplex(string(cmd_line))) {
            if(execl("/bin/bash", "bash", "-c",/* “complex-external-command” need to be added by the instructions*/ cmd_line, nullptr)==-1){
				perror("smash error: execv failed");
				exit(1);
			}
        }
    else {
            if(execvp(this->arg[0], this->arg)==-1)
			{
				perror("smash error: execv failed");
				exit(1);
			}
        }

    }
    else // fatha'
    {
        switch (this->is_background) {
            case true:
                this->job_list->addJob(cmd_line, child_pid);
                break;
            case false:
                shell.setForegroundPid(child_pid);
                shell.setFgCommand(cmd_line);
				//shell.setFgCommand(this);
                //printf("waiting \n");
                if(waitpid(child_pid, NULL, WUNTRACED)==-1)
				{
					perror("smash error: waitpid failed");
					exit(1);
				}
                shell.setForegroundPid(NO_PID_NUMBER);
                break;
        }
    }

}

JobsCommand::JobsCommand(const char *cmd_line): BuiltInCommand(cmd_line) {}
void JobsCommand::execute() {
    this->job_list->removeFinishedJobs();
    this->job_list->printJobsList();
}

PARAMSTATUS checkFgAndBgCommandParams(char** arg,int arg_num)
{
    if(arg_num>2) return NO_GOOD;
    if(arg_num==2)
    {
        string number_param = arg[1];
        for(char letter: number_param)
        {
            if (letter < '0' || letter > '9') {
                return NO_GOOD; //number_param[i] isn't a number..
            }
        }
    }
    return GOOD;
}

ForegroundCommand::ForegroundCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
void ForegroundCommand::execute()
{
	SmallShell& shell=SmallShell::getInstance();
    PARAMSTATUS param_status = checkFgAndBgCommandParams(arg,arg_num);
    if(param_status==NO_GOOD)
    {
        cerr << "smash error: fg: invalid arguments" << endl;
        return;
    }

    this->job_list->removeFinishedJobs();

    /*find the job in the job list, remove it and print it*/
    JobEntry* job_to_front;
    int job_id = NO_ID_NUMBER;
    FINDSTATUS status;
    if(arg_num<2) {
        job_id = job_list->getMaxJobId(); // arg_num == 1
        job_to_front = job_list->getJobById(job_id,status);
        if (status==NOT_FOUND) {
            cerr << "smash error: fg: jobs list is empty" << endl;
            return;
        }
    }
    else {
        job_id = char_to_int(arg[1]);// arg_num == 2
        job_to_front = job_list->getJobById(job_id, status);
        if (status==NOT_FOUND) {
            cerr << "smash error: fg: job-id " << job_id << " does not exist" << endl;
            return;
        }
    }
	//shell.setFgCommand(job_to_front->getCommand());
    job_to_front->printCommandForFgAndBgCommand();
    pid_t job_pid = job_to_front->getJobPid();
    job_list->removeJobById(job_id);
	shell.setForegroundPid(job_pid);
	shell.setFgCommand(job_to_front->getCommand());
    /*----------------------------------------------------*/

    /*tell the process to continue and then wait for it*/
    if(kill(job_pid,SIGCONT)==-1){
		perror("smash error: kill failed");
		exit(1);
	}
    if(waitpid(job_pid,NULL,WUNTRACED)==-1){
		perror("smash error: waitpid failed");
		exit(1);
	}
	//shell.setFgCommand(nullptr);
    /*----------------------------------------------------*/
}

BackgroundCommand::BackgroundCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
void BackgroundCommand::execute()
{
    PARAMSTATUS param_status = checkFgAndBgCommandParams(arg,arg_num);
    if(param_status==NO_GOOD)
    {
        cerr << "smash error: bg: invalid arguments" << endl;
        return;
    }

    this->job_list->removeFinishedJobs();

    /*find the job in the job list and print it*/
    JobEntry* stopped_job;
    FINDSTATUS status;
    if(arg_num<2) {
        stopped_job = this->job_list->getLastStoppedJob(); // arg_num=1
        if(stopped_job == nullptr) {
            cerr << "smash error: bg: there is no stopped jobs to resume" << endl;
            return;
        }
    }
    else
    {
        int job_id = char_to_int(arg[1]); // arg_num == 2
        stopped_job = job_list->getJobById(job_id,status);
        if(status==NOT_FOUND){
            cerr << "smash error: bg: job-id " << job_id << " does not exist" << endl;
            return;
        }
        else if(!(stopped_job->isStopped()))
        {
            cerr << "smash error: bg: job-id " << job_id << " is already running in the background" << endl;
            return;
        }
    }

    stopped_job->printCommandForFgAndBgCommand();
    stopped_job->continueJob();
    pid_t job_pid = stopped_job->getJobPid();
    if(kill(job_pid,SIGCONT)==-1)
	{
		perror("smash error: kill failed");
		exit(1);
	}
    /*----------------------------------------------------*/
}

#define IDENTICAL 0

void JobsList::printAllJobsForQuitCommand()
{
    for(JobEntry* job : data)
    {
        cout << job->getJobPid() << ": " << job->getCommand() << endl;
    }
}
void JobsList::killAllJobs()
{
    for(JobEntry* job : data)
    {
        kill(job->getJobPid(),SIGKILL);
    }
}

QuitCommand::QuitCommand(const char *cmd_line): BuiltInCommand(cmd_line) {};
void QuitCommand::execute() {
    if (arg_num > 1 && strcmp(arg[1], "kill") == IDENTICAL) {
        job_list->removeFinishedJobs();
        cout << "smash: sending SIGKILL signal to " << job_list->getListSize() << " jobs:" << endl;
        job_list->printAllJobsForQuitCommand();
        this->job_list->killAllJobs();
    }
    exit(1); //what argument to send?
}

RedirectionCommand::RedirectionCommand(const char *cmd_line) : Command(cmd_line) {
	int position_arrow=0;
	this->cmd = _parseFirstRedirectionCommand(string(cmd_line),&position_arrow);//filling the arrow position
    this->file_name = _ParseSecondRedirectionCommand(string(cmd_line),position_arrow,&this->cmdType);//using the arrow position...
	assert(cmdType!=REDIRECTION_ILLEGAL);
}

void RedirectionCommand::execute() {
 int fd_stdout=dup(1);
 if(fd_stdout==-1)
 {
		 perror("smash error: dup failed");
		 exit(1);

 }
	int fd_opened_file;
	SmallShell& shell=SmallShell::getInstance();
	Command* cmd=shell.CreateCommand(this->cmd.c_str());
 if(this->cmdType==REDIRECTION_OVERWRITE) {
	  fd_opened_file = open(file_name.c_str(),O_CREAT|O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);//mode
	  if(fd_opened_file==-1)
	  {
		  perror("smash error: open failed");
		  exit(1);
	  }
 }
	if(this->cmdType==REDIRECTION_APPEND) {
		fd_opened_file = open(file_name.c_str(),O_CREAT|O_WRONLY|O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);//mode
		if(fd_opened_file==-1)
		{
			perror("smash error: open failed");
			exit(1);
		}
	}
	if(dup2(fd_opened_file,1)==-1)
	{
		perror("smash error: dup2 failed");
		exit(1);
	}
	cmd->execute();
	if(dup2(fd_stdout,1)==-1)
		{
			perror("smash error: dup2 failed");
			exit(1);
		}

	if(close(fd_opened_file)==-1)
	{
		perror("smash error: close failed");
		exit(1);
	}
	if(close(fd_stdout)==-1)
	{
		perror("smash error: close failed");
		exit(1);
	}
	delete cmd;
}

PipeCommand::PipeCommand(const char *cmd_line) : Command(cmd_line) {
	int first_pipe_pos=0;

	 this->frst= _parseFirstPipeCommand(string(cmd_line),&first_pipe_pos);
	 this->scnd= _ParseSecondPipeCommand(string(cmd_line),first_pipe_pos,&this->cmdType);
	 assert(this->cmdType!=PIPE_IILEGAL);
}

void PipeCommand::execute() {

	SmallShell& shell=SmallShell::getInstance();
	Command* command_1=shell.CreateCommand(frst.c_str());
	Command* command_2=shell.CreateCommand(scnd.c_str());
	int fd[2];
	if(pipe(fd)==-1)
	{
			perror("smash error: pipe failed");
			exit(1);
	}
	pid_t frst_child=fork();
	if(frst_child==-1)
	{
		perror("smash error: fork failed");
		exit(1);
	}
	else if (frst_child == 0) {
		// first child
		if(setpgrp()==-1)
		{
			perror("smash error: setpgrp failed");
			exit(1);
		}
		if(this->cmdType==PIPE_STDOUT) {
			if(dup2(fd[WR], 1)==-1)
			{
				perror("smash error: dup2 failed");
				exit(1);
			}//stdout
		}
		else if (this->cmdType==PIPE_STDERR)
		{
			if(dup2(fd[WR],2)==-1)//2==STDERR?
			{
				perror("smash error: dup2 failed");
				exit(1);
			}//stdout
		}

		if(close(fd[RD])==-1)
		{
			perror("smash error: close failed");
			exit(1);
		}//stdout
		if(close(fd[WR])==-1)
		{
			perror("smash error: close failed");
			exit(1);
		}//stdout
		command_1->execute();
		delete command_1;
		delete command_2;
		exit(1);
	}
	else {
		pid_t scnd_child=fork();
		if(scnd_child==-1)
		{
			perror("smash error: fork failed");
			exit(1);
		}
		else if (scnd_child == 0) {
			if(setpgrp()==-1)
			{
				perror("smash error: setpgrp failed");
				exit(1);
			}
			//todo need to continue from here
			// second child
			dup2(fd[0], 0);
			close(fd[0]);
			close(fd[1]);
			command_2->execute();
			exit(1);
		}
	}
	close(fd[0]);
	close(fd[1]);
	//exit(1);
}

PARAMSTATUS checkKillParams(char** arg, int arg_num)
{
    if(arg_num!=3)
    {
        return NO_GOOD;
    }
    string signum_param = arg[1];
    bool first_letter = true;
    for(char letter: signum_param)
    {
        if(first_letter)
        {
           if(letter != '-')
           {
               return NO_GOOD;
           }
           else
           {
               first_letter=false;
               continue;
           }
        }

        if (letter < '0' || letter > '9') {
            return NO_GOOD; //number_param[i] isn't a number..
        }
        first_letter=false;
    }
    string id_num_param = arg[2];
    for(char letter: id_num_param)
    {
        if (letter < '0' || letter > '9') {
            return NO_GOOD; //number_param[i] isn't a number..
        }
    }
    return GOOD;
}

KillCommand::KillCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
void KillCommand::execute()
{
    if(checkKillParams(arg,arg_num)==NO_GOOD)
    {
        cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }
    int sig_num = char_to_int(arg[1]);
    int job_id = char_to_int(arg[2]);

    FINDSTATUS job_exists_in_the_background;
    JobEntry* job_to_send_signal_to = job_list->getJobById(job_id,job_exists_in_the_background);
    if(!job_exists_in_the_background)
    {
        cerr << "smash error: kill: job-id "<< job_id << " does not exist" << endl;
        return;
    }
    if(kill(job_to_send_signal_to->getJobPid(), sig_num)==-1)
    {
        perror("smash error: kill failed");
        exit(1);
    }
    if(sig_num==SIGSTOP)
    {
        job_to_send_signal_to->stopJob();
    }
    cout << "signal number "<< sig_num <<" was sent to pid " << job_to_send_signal_to->getJobPid() << endl;
}