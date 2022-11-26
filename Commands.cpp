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

enum PARAMSTATUS{NO_GOOD=0,GOOD=1};

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
bool check_if_redirection_command(const char* cmd_line)
//todo maybe add indicator to which one of the redirection symbols we found...for this momemnt we only test if we can spot it..
{
    int arg_count=0;
    char* cmd_args[COMMAND_MAX_ARGS];
    arg_count= _parseCommandLine(cmd_line,cmd_args);
    bool is_redirection=false;
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
    return is_redirection;
}
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
	cout<<a<<"pos is"<<*i<<"char is"<<(cmd[*i])<<endl;
	return a;
}
string _ParseSecondPipeCommand(string cmd,int pos)
{
	string a="";
	bool pipe_hit=false;
	bool amp_after_pipe_hit=false;
	if(cmd[pos+1]!='&') //regular |
	{
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
		cout<<"second cmd " << a<< endl;



	return a;
}
string _parseRedirectionCommand()
{
	return "";
}
bool check_if_pipe_command(const char* cmd_line)
{
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
    return is_pipe;
}

/* support function for fgcommand*/
/*the function assums that the argument is a number*/
int char_to_int(const char* str)
{
    int value = 0;
    string temp = str;
    for(char letter: temp)
    {
        value *= 10;
        value += letter - 48;
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
//    this->is_pipe_command= check_if_pipe_command(cmd_line);
//    this->is_redirection_command= check_if_redirection_command(cmd_line);
    //assert(!(is_redirection_command&&is_pipe_command));
   // if(!(is_pipe_command || is_redirection_command)) //if not pipe or redirection, check for bg command
 //   {
        //why am i doing this like that- in the wet , it is mentioened that pipe commands IGNORE & and cannot be background tasks...
        this->is_background = _isBackgroundCommand(cmd_line);
    this->job_list = SmallShell::getInstance().getJobsList();
    //  }
    /*else{
        strcpy(this->cmd_line,cmd_line);
        this->is_background=false;
    }*/
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
BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line){}
/**BuiltInCommand class implementation**/

/**ExternalCommand class implementation**/
//ExternalCommand::ExternalCommand(const char* cmd_line): Command(cmd_line) {}
/**ExternalCommand class implementation**/




/**JobEntry methods implementation**/
JobEntry::JobEntry(int id,pid_t pid,char cmd_line[80], Command *command, bool stopped_flag) {
    this->id = id;
    this->pid = pid;
    this->command = command;
    strcpy(this->cmd_line,cmd_line);
    this->insertion_time = time(NULL);
    this->stopped_flag = stopped_flag;
}

JobEntry::~JobEntry() {

}

int JobEntry::getJobId() {
    return id;
}

pid_t JobEntry::getJobPid() {
    return pid;
}

Command *JobEntry::getCommand() {
    return command;
}

bool JobEntry::isStopped() {
    return stopped_flag;
}
void JobEntry::printCommandForFgCommand() {
    cout<< *command << " : " << this->getJobPid() << endl;
}

ostream & operator<<(ostream &os, JobEntry &jobEntry) {
    string stopped=(jobEntry.stopped_flag)? "(stopped)":"";

    //[<job-id>]<-check <command> : <process id> <seconds elapsed> (stopped)
            os <<"["<< jobEntry.id <<"]"<< " " << string(jobEntry.cmd_line) << " : " << jobEntry.getJobPid() << " "
               << difftime(time(NULL),jobEntry.insertion_time)<<" secs"
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

void JobsList::addJob(Command *cmd,char cmd_line[80], pid_t job_pid, bool isStopped)
{
    JobEntry* jobEntry = new JobEntry((this->curr_job_id_max==0)?  1 : this->getMaxJobId()+1
            ,job_pid,cmd_line,cmd,isStopped);//todo isStopped neccesary?
    this->data.push_back(jobEntry);
    this->curr_job_id_max=getMaxJobId();
}

void JobsList::printJobsList() {
    //JobsList.removeFinishedJobs();
    for (JobEntry *job: data) {
        cout<<*job<<endl;
    }
}

void JobsList::killAllJobs() {
    std::cout<<"killAllJobs"<<std::endl;
    for (JobEntry *job: data) {

        //kill(,SIGKILL);
    }
}
bool isFinished(JobEntry *job)
{
    //cout << bool(waitpid(job->getJobPid(), nullptr, WNOHANG))<<endl;
     (waitpid((*job).getJobPid(), nullptr, WNOHANG));
    return true;

}

void JobsList::removeFinishedJobs()
{
    //printf("removing finished jobs.. \n");
    for(auto iterator = data.begin(); iterator != data.end(); )
    {
        //printf("waiting for %d ... \n",(*iterator)->getJobPid());
        int son_is_potent = waitpid((*iterator)->getJobPid(), nullptr, WNOHANG);
        //printf("wait was successfull\n");
        if (son_is_potent) { // alive and strong, very powerful son very potent

           // printf("earsing.. \n");
            data.erase(iterator);
          //  printf("erased \n");

        }else{
                //printf("iterator++ \n ");
                iterator++;

        }
    }
    //printf("jobslist removed all fnished jobs succesfully \n");
}

JobEntry *JobsList::getJobById(int job_id,enum FINDSTATUS& find_status) {
    /*find_status - to be returned(our function mallocs it , so we should give empty pointers to it
     * ziv i you read this i think this line is pure sh*t:
     * find_status=(enum FINDSTATUS*)malloc(sizeof(*find_status));
     * your obedient servant,
     * L.R.
    */
    for (JobEntry *job: this->data) {
        if (job->getJobId() == job_id)
        {
            find_status = FOUND;
            return job;
        }
    }
    find_status = NOT_FOUND;
    return nullptr;
}

void JobsList::removeJobById(int jobId) {
    /*todo finish removeJobById implementation
     * so the job will be remove from the list but not deleted!!*/
    for(auto iterator = data.begin(); iterator != data.end();)
    {
        if((*iterator)->getJobId() == jobId)
        {
            data.erase(iterator);
        }
    }
}

int getLastStoppedJobId()
{
  /*  int last_stopped_job_id=NO_ID_NUMBER;
    int
    for(JobEntry* job:this->data)
    {
        if(last_stopped_job_id<job->getJobStoppingTime())
        {

        }

        last_stopped_job_id=max(last_stopped_job_id,job->getJobStoppingTime());
    }
    return max_job_id;*/
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
    int max_job_id=NO_ID_NUMBER;
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
    //todo check if command is pipe or redirection or none...
    bool is_cmd_pipe= false;
    bool is_cmd_redirection=false;
    is_cmd_pipe = check_if_pipe_command(cmd_line);
    is_cmd_redirection = check_if_redirection_command(cmd_line);
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    /** ignore the & sign **/
    _removeBackgroundSign(const_cast<char*>(firstWord.c_str()));
    firstWord = _trim(firstWord.c_str());
    /** ignore the & sign **/

    assert(!(is_cmd_pipe&&is_cmd_redirection));//we allow only one "type" of command or none of them, but never both.
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
    }/*else if (firstWord.compare("bg") == 0) {
        return new BackgroundCommand(cmd_line);
    }else if(firstWord.compare("quit") == 0){
        return new QuitCommand(cmd_line);
    }*/else{
        return new ExternalCommand(cmd_line);
    }
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

    //todo check if the command not empty
    //todo think whether we need to insert a fg process to the job list!

    pid_t child_pid = fork();
    if (child_pid == 0) // my son
    {
        if (isExternalComplex(string(cmd_line))) {
            DO_SYS(execl("/bin/bash", "bash", "-c",/* “complex-external-command” need to be added by the instructions*/ cmd_line, nullptr));
        }
    else /*if(the first argument start with ./)*/ {
            DO_SYS(execvp(this->arg[0], this->arg));
        }

    }
    else // fatha'
    {
        switch (this->is_background) {
            case true:
                this->job_list->addJob(this,cmd_line, child_pid);
                break;
            case false:
                SmallShell::getInstance().setForegroundPid(child_pid);
                printf("waiting \n");
                DO_SYS(waitpid(child_pid, NULL, 0));
                SmallShell::getInstance().setForegroundPid(NO_PID_NUMBER);
                break;
        }
    }

}

// todo test the jobs command
JobsCommand::JobsCommand(const char *cmd_line): BuiltInCommand(cmd_line) {}

void JobsCommand::execute() {
    //todo check error cases and treat them
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

// todo test the foreground command
// todo check which errors occure first
ForegroundCommand::ForegroundCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
void ForegroundCommand::execute()
{
    PARAMSTATUS param_status = checkFgAndBgCommandParams(arg,arg_num);
    if(param_status==NO_GOOD)
    {
        cout << "smash error: fg: invalid arguments" << endl;
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
            cout << "smash error: fg: jobs list is empty" << endl;
            return;
        }
    }
    else {
        job_id = char_to_int(arg[1]);// arg_num == 2
        job_to_front = job_list->getJobById(job_id, status);
        if (status==NOT_FOUND) {
            cout << "smash error: fg: job-id " << job_id << " does not exist" << endl;
            return;
        }
    }
    job_to_front->printCommandForFgCommand();
    job_list->removeJobById(job_id);
    /*----------------------------------------------------*/

    /*tell the process to continue and then wait for it*/
    pid_t job_pid = job_to_front->getJobPid();
    kill(job_pid,SIGCONT);
    waitpid(job_pid,NULL,0);
    /*----------------------------------------------------*/
}

BackgroundCommand::BackgroundCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
void BackgroundCommand::execute()
{
    PARAMSTATUS param_status = checkFgAndBgCommandParams(arg,arg_num);
    if(param_status==NO_GOOD)
    {
        cout << "smash error: fg: invalid arguments" << endl;
        return;
    }

    this->job_list->removeFinishedJobs();

    /*find the job in the job list and print it*/
    JobEntry* stopped_job;
    int job_id = NO_ID_NUMBER;
    FINDSTATUS status;
    if(arg_num<2) {
        //this->job_list->getLastStoppedJobId(); // arg_num=1
        stopped_job = job_list->getJobById(job_id,status);
        if(status==NOT_FOUND) {
            cout << "smash error: bg: there is no stopped jobs to resume" << endl;
            return;
        }
    }
    else
    {
        job_id = char_to_int(arg[1]);// arg_num == 2
        stopped_job = job_list->getJobById(job_id,status);
        if(status==NOT_FOUND){
            cout << "smash error: fg: job-id " << job_id << " does not exist" << endl;
            return;
        }
        else if(!(stopped_job->isStopped()))
        {
            cout << "smash error: bg: job-id " << job_id << " is already running in the background" << endl;
            return;
        }
    }

    stopped_job->printCommandForFgCommand();
    /*----------------------------------------------------*/


    /*tell the process to continue and then wait for it*/
    pid_t job_pid = stopped_job->getJobPid();
    kill(job_pid,SIGCONT);
    waitpid(job_pid,NULL,0);
    /*----------------------------------------------------*/
}


RedirectionCommand::RedirectionCommand(const char *cmd_line) : Command(cmd_line) {
    std::cout<<"redirection constructor"<<std::endl;
    assert(false);
}

void RedirectionCommand::execute() {
    std::cout<<"redirection execute"<<std::endl;
    assert(false);
}

PipeCommand::PipeCommand(const char *cmd_line) : Command(cmd_line) {
    std::cout<<"pipe constructor"<<std::endl;
	int first_pipe_pos=0;
	string frst= _parseFirstPipeCommand(string(cmd_line),&first_pipe_pos);
	string scnd= _ParseSecondPipeCommand(string(cmd_line),first_pipe_pos);


}

void PipeCommand::execute() {
    std::cout<<"pipe execute"<<std::endl;

}