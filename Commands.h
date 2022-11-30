#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#define DO_SYS( syscall ) if ((syscall) == -1 ){ perror(#syscall); exit(1); }

//to make cd& and cd the same , what we should is the following
// in internal commnads, copy the stirng ot antoer string, check if its has & sign and then remove it if it has it.(and then we won't remove it from the external)
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define COMMAND_MAX_LENGTH (80)

#define NO_PID_NUMBER 0
#define NO_ID_NUMBER -1

using std::string;
using std::ostream;
enum PIPE_CHANNEL{RD=0,WR=1};//used to make life easier .
enum PIPE_CMD_TYPE{PIPE_IILEGAL=-1,PIPE_STDOUT=0,PIPE_STDERR=1};// will be returned in pipe parser.
enum REDIRECTION_CMD_TYPE{REDIRECTION_ILLEGAL=-1,REDIRECTION_OVERWRITE=2,REDIRECTION_APPEND=3}; // will be returned in redirection parser.
enum FINDSTATUS{NOT_FOUND=0,FOUND=1};//serves as status for find method.
class JobsList;
class Command {

// TODO: Add your data members
protected:
    bool is_pipe_command;
    bool is_redirection_command;
    char cmd_line[COMMAND_MAX_LENGTH];
	char* arg[COMMAND_MAX_ARGS];
    int arg_num;
    bool is_background;
    JobsList* job_list;
 public:
  Command(const char* cmd_line);
  virtual ~Command();
  char *getCmdLine() ;
  virtual void execute(){
      std::cout<<"Command execute function";
      throw;
  }
  friend ostream& operator<<(ostream& os,Command& command);
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};


class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
	string frst="",scnd="";
	PIPE_CMD_TYPE cmdType=PIPE_IILEGAL;
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
	string cmd,file_name;
	REDIRECTION_CMD_TYPE cmdType=REDIRECTION_ILLEGAL;
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};
class BuiltInCommand : public Command {
public:
    BuiltInCommand(const char* cmd_line);
    virtual ~BuiltInCommand() {}
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
private:
   // string* plast_pwd = nullptr;//todo remove if redundant
public:
  ChangeDirCommand(const char* cmd_line);
  virtual ~ChangeDirCommand();
  void execute() override;
  //string* getPLastPwd();//todo remove if redundant
 // void setLastPwd(string* new_last_pwd);//todo remove if redundant
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};
//a
class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};
class ChpromptCommand : public BuiltInCommand {
public:
    ChpromptCommand(const char* cmd_line);
    virtual ~ChpromptCommand() {}
    void execute() override;
};


class JobEntry {
private:
    int id;
    pid_t pid;
    //Command* command;
    char cmd_line[80]={0};
    time_t insertion_time;
    time_t work_time;//for stopped jobs we will measure it as soon as we stop it(stopped jobs dont "work")
    bool stopped_flag;
public:
    int getJobId();
    pid_t getJobPid();
    char* getCommand();
    void printCommandForFgCommand(); // for foreground command
    bool isStopped();
    JobEntry(int id,int pid,char cmd_line[80], bool stopped_flag);//later remove command*
    ~JobEntry();
    bool operator==(JobEntry jobEntry)//by job entry. we dont care about the command actual things.
    {
        return this->id==jobEntry.id;
    }
    bool operator<=(JobEntry jobEntry)
    {
        //same idea as == operator, we don't care about commands string content. only about the job id.
        return this->id<=jobEntry.id;
    }
    friend ostream& operator<<(ostream& os,JobEntry& jobEntry);


};

class JobsList {
private:
 // TODO: Add your data members
 std::vector<JobEntry*> data;
 //int curr_job_id_max = 0;//todo implement ziv ????ziv you are no good we no need this ok? no need! what is ziv what is?
public:
  JobsList();
  void sort_JobsList();
  ~JobsList();
  int getMaxJobId();
  JobEntry* find_by_job_id(int id,enum FINDSTATUS& find_status);
  void addJob(char cmd_line[80],pid_t job_pid, bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int ,enum FINDSTATUS& find_status);//findstatus should be sent as empty POINTER!!!
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  int getLastStoppedJobId();
  // TODO: Add extra methods or modify exisitng ones as needed
  void printAllJobsForQuitCommand();
  int getListSize()
  {
      return data.size();
  }
};

class QuitCommand : public BuiltInCommand {
// TODO: Add your data members
public:
    QuitCommand(const char* cmd_line);
    virtual ~QuitCommand() {}
    void execute() override;
};


class JobsCommand : public BuiltInCommand {
 public:
  JobsCommand(const char* cmd_line);//jobslist* jobs_list
  virtual ~JobsCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class TimeoutCommand : public BuiltInCommand {
/* Optional */
// TODO: Add your data members
 const char* cmd_line;
 public:
  explicit TimeoutCommand(const char* cmd_line);
  virtual ~TimeoutCommand() {}
  void execute() override;
};

class FareCommand : public BuiltInCommand {
  /* Optional */
  // TODO: Add your data members
 public:
  FareCommand(const char* cmd_line);
  virtual ~FareCommand() {}
  void execute() override;
};

class SetcoreCommand : public BuiltInCommand {
  /* Optional */
  // TODO: Add your data members
 public:
  SetcoreCommand(const char* cmd_line);
  virtual ~SetcoreCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
  /* Bonus */
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line);
  virtual ~KillCommand() {}
  void execute() override;
};

class SmallShell {
 private:
 /**our additional parameters**/
 char fg_command_line[80];
 string prompt_name;
 pid_t foreground_pid;
 JobsList* jobs_list;
 string last_pwd = "";
 /**our additional parameters**/
  SmallShell();
 public:
 /**the original methods**/
  char* getFgCommand();
  void setFgCommand(char* cmd_line);
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(const char* cmd_line);
  /**the original methods**/

    /**our additional methods**/
  void setPromptName(string new_name="smash");
  string getPromptName();
  pid_t getForegroundPid();
  pid_t getSmallShellPid();
  JobsList* getJobsList();
  void setForegroundPid(pid_t new_fg_pid);

    /**change dir support methods**/
    string getLastPwd();
    void setLastPwd(string new_last_pwd);
    bool wasCDCalled=false; // indicates whether CD command was already called, false if not (if not last_pwd is not valid) (used in "cd -" command)
    /**change dir support methods**/


    /**our additional methods**/

};
string _trim(const std::string &s);
#endif //SMASH_COMMAND_H_
