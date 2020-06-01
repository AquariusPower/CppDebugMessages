//  BSD 3-Clause License
//
//  Copyright (c) 2018-2020, AquariusPower<https://github.com/AquariusPower>
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
//  * Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifdef DBGMSG

#define DBGMSG_OBJ //do not use this define line in your project cpp files, it is meant only for dbgmsg.cpp!!!
#include "dbgmsgproj.h"

/**
 * SELF NOTE: KEEP THESE 4 "single line commented" lines below FOR NOW...
 * this merge indicator trick is here just to prevent compilation before commenting these lines and still let eclipse work! :)
 * TODO find why eclipse needs this to let autocompletion work here :(
 */
//<<<<< dummy1
//#include "dbgmsg.h"
//>>>>> dummy2
//int iDummy=0; //this line is to prevent the merge detection from going further below breaking autocompletion etc...

#include <string.h> //strlen
#include <unistd.h> //std::getpid()
#include <ctime>
#include <thread>
#include <mutex>

//#ifdef UNIX //TODO necessary?
#include <time.h> //strftime
//#endif

#include <sys/stat.h> //to feed strftime
#ifdef WIN32
#define stat _stat
#endif

//#include "extratools.h"

#define DBGLNSELF {DBGOE(DBGFLF);};
#define DBGLNSELFB4INIT {if(!dbgmsg::bInitCompleted){ DBGLNSELF; }};

bool dbgmsg::bInitCompleted;

bool dbgmsg::bAllowOE;
bool dbgmsg::bWaitOnCrash;
bool dbgmsg::bPrependDtTm;
bool dbgmsg::bPrependDbgmsgId;
bool dbgmsg::bPidAllowed;
bool dbgmsg::bAddingLog;
std::stringstream dbgmsg::ssDbgMsgPartTmp;
std::stringstream dbgmsg::ssDbgMsgTmp;
std::stringstream dbgmsg::ssDbgMsgFileName;
std::stringstream dbgmsg::ssDbgMsgFileNameCrash;
std::stringstream dbgmsg::ssDbgMsgPath;
std::stringstream dbgmsg::ssVarIdTmp;
std::stringstream dbgmsg::ssVarValueTmp;
std::ofstream dbgmsg::fldDbgMsg;
unsigned long long dbgmsg::llDbgmsgId;
int dbgmsg::iPid;
int dbgmsg::iMaxLinesInDebugFile;
ulong dbgmsg::iMaxCrashLinesInMemory;
std::vector<std::string> dbgmsg::vLastDbgMsgs;
std::vector<std::pair<std::string,std::string>> dbgmsg::vIdVal;

unsigned long long llDesperateInternalInitRandomKey; // TODO why I need this? :/

dbgmsg::dbgmsg(){DBGOE(DBGFLF<<":DBGMSG:RealConstructorIn"); //TODO never run?
  LazyConstructor();
}

#define DBGVARCLEAR \
  ssVarIdTmp.str(std::string());\
  ssVarIdTmp.clear();\
  ssVarValueTmp.str(std::string());\
  ssVarValueTmp.clear();

std::string dbgmsg::SetVar(){
  std::string strId=ssVarIdTmp.str();
  std::string strValue=ssVarValueTmp.str();
  
  ssVarValueTmp.clear();
  std::string strOld=GetVar();
  
  vIdVal.push_back(std::make_pair(strId,strValue));
  
  DBGVARCLEAR;
  
  return strOld;
}
std::string dbgmsg::GetVar(){
  std::string strId=ssVarIdTmp.str();
  std::string strDefaultValue=ssVarValueTmp.str();
  
  static std::string strIdChk;
  static std::string strValStored;
  static std::string strRet;strRet=strDefaultValue;
  for(uint i=0;i<vIdVal.size();i++){
    strIdChk=vIdVal[i].first;
    if(strId==strIdChk){
      strValStored=vIdVal[i].second;
      strRet=strValStored;
      break;
//      if(strValStored.empty() || strValStored=="0" || strValStored=="0.0"){
//        return strDefaultValue;
//      }else{
//        return strValStored;
//      }
    }
  }
  
  DBGVARCLEAR;
  
  return strRet;
}

void dbgmsg::LazyConstructor(){
  if(llDesperateInternalInitRandomKey==39854029834543289LL)return;

  DBGOE(DBGFLF<<":DBGMSG:In");

  bInitCompleted=false;DBGLNSELFB4INIT; //FIRST!

  bAllowOE=false;
#ifdef DBGMSG_ALLOWOE
  bAllowOE=true;
#endif
  
  ssDbgMsgPartTmp.clear();DBGLNSELFB4INIT; //just to init
  ssDbgMsgTmp.clear();DBGLNSELFB4INIT; //just to init
  vLastDbgMsgs.clear();DBGLNSELFB4INIT;DBGOE(vLastDbgMsgs.size()) //just to init
  fldDbgMsg.is_open();DBGLNSELFB4INIT; //just to init
  ssDbgMsgFileName.clear();DBGLNSELFB4INIT; //just to init
  bWaitOnCrash=false;DBGLNSELFB4INIT;
  bPrependDtTm=true;DBGLNSELFB4INIT;
  bPrependDbgmsgId=true;DBGLNSELFB4INIT;
  ssDbgMsgFileNameCrash.clear();DBGLNSELFB4INIT; //just to init
  ssDbgMsgPath.clear();DBGLNSELFB4INIT; //just to init
  bPidAllowed=false;DBGLNSELFB4INIT;
  iPid=0;DBGLNSELFB4INIT;
  llDbgmsgId=0;DBGLNSELFB4INIT;
  iMaxLinesInDebugFile = 100000;DBGLNSELFB4INIT;
  iMaxCrashLinesInMemory = 1000;DBGLNSELFB4INIT;
  ssVarIdTmp.clear();DBGLNSELFB4INIT;
  vIdVal.clear();DBGLNSELFB4INIT;
  ssVarValueTmp.clear();DBGLNSELFB4INIT;
  bAddingLog=false;DBGLNSELFB4INIT;DBGOE(bAddingLog);

  llDesperateInternalInitRandomKey=39854029834543289LL;

  /**
   * CALLS ONLY AFTER llDesperateInternalInitRandomKey is set !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   */
  
#ifdef UNIX
  getCurrentStackTraceSS(true,false);
#endif

  DBGOE(DBGFLF<<":DBGMSG:Out");
}

void dbgmsg::SetDebugLogPath(const char* c){
  if(!ssDbgMsgPath.str().empty()){
    if(ssDbgMsgPath.str()==c)
      return;
    DBGOE("DBGMSG: will not change path that is already set to '"<<ssDbgMsgPath.str()<<"', asked now '"<<c<<"'");
    return;
  }

  if(c!=NULL){ //TODO validate (and/or create?) path b4 setting it
    ssDbgMsgPath.str(std::string()); //actually clear/empty it = ""
    ssDbgMsgPath.clear(); //good?
    ssDbgMsgPath<<c;
    DBGOE("DBGMSG: set path: "<<ssDbgMsgPath.str());
  }
}

std::string dbgmsg::id(const char* cId){ //TODO not working... is duplicating the id...
  ssDbgMsgPartTmp.str(std::string()); //actually clear/empty it = ""
  if(cId!=NULL)ssDbgMsgPartTmp<<cId<<"=";
  return ssDbgMsgPartTmp.str();
}

std::string dbgmsg::b(bool b,const char* cId){
  ssDbgMsgPartTmp.str(std::string()); //actually clear/empty it = ""
  if(cId!=NULL)ssDbgMsgPartTmp<<cId<<"=";
  ssDbgMsgPartTmp<<(b?"true":"false");
  return ssDbgMsgPartTmp.str();
}

std::string dbgmsg::i(long l,const char* cId){
  ssDbgMsgPartTmp.str(std::string()); //actually clear/empty it = ""
  if(cId!=NULL)ssDbgMsgPartTmp<<cId<<"=";
  ssDbgMsgPartTmp<<l;
  return ssDbgMsgPartTmp.str();
}

std::string dbgmsg::f(double d,const char* cId){
  ssDbgMsgPartTmp.str(std::string()); //actually clear/empty it = ""
  if(cId!=NULL)ssDbgMsgPartTmp<<cId<<"=";
  ssDbgMsgPartTmp<<d;
  return ssDbgMsgPartTmp.str();
}

std::string dbgmsg::str(const char* c,const char* cId){
  ssDbgMsgPartTmp.str(std::string()); //actually clear/empty it = ""
  if(cId!=NULL)ssDbgMsgPartTmp<<cId<<"=";
  ssDbgMsgPartTmp<<"'"<<c<<"'";
  return ssDbgMsgPartTmp.str();
}

void dbgmsg::init(){DBGLNSELFB4INIT;
  LazyConstructor();

  #ifdef UNIX
    initSignals();DBGLNSELFB4INIT;
  #endif

  initStream();DBGLNSELFB4INIT;

#ifdef UNIX
  getCurrentStackTraceSS(true,false);DBGLNSELFB4INIT;
#endif
//  std::stringstream ss;ss<<"DBGMSG INIT COMPLETED!";DBGLNSELFB4INIT;
//  addDbgMsgLog(ss);DBGLNSELFB4INIT;
  DBGOE("DBGMSG INIT COMPLETED!");DBGLNSELFB4INIT;

  bInitCompleted=true;DBGLNSELFB4INIT;
}

#ifdef UNIX
void dbgmsg::initSignals(){DBGLNSELFB4INIT;
  // as specified at `info signal`
  //TODO determine if these handlers are already set (unless it will just add the handler)
  signal(SIGHUP , dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGINT , dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGQUIT, dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGILL , dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGABRT, dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGFPE , dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGKILL, dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGSEGV, dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGPIPE, dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGTERM, dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGBUS , dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGSYS , dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  signal(SIGTRAP, dbgmsg::SigHndlr);DBGLNSELFB4INIT;
  //? SIGXFSZ
}
#endif

void dbgmsg::initStream(){DBGLNSELFB4INIT;
  ssDbgMsgPath<<"";DBGLNSELFB4INIT;

  // path
#ifdef UNIX
  if(ssDbgMsgPath.str().empty()){DBGLNSELFB4INIT;
    ssDbgMsgPath<<getenv("HOME")<<"/";DBGLNSELFB4INIT;
  }DBGLNSELFB4INIT;
#endif

  if(!ssDbgMsgPath.str().empty()){DBGLNSELFB4INIT;
    ssDbgMsgFileName<<ssDbgMsgPath.str()<<"/";DBGLNSELFB4INIT;
  }else{DBGLNSELFB4INIT;
    ssDbgMsgFileName<<"./";DBGLNSELFB4INIT; //will be relative to current execution path
  }DBGLNSELFB4INIT;

  // filename
#ifdef __USE_GNU
  char* c=program_invocation_short_name;DBGLNSELFB4INIT; //auto "guess work" that should work fine
  if(c!=NULL){DBGLNSELFB4INIT;
    ssDbgMsgFileName<<"."<<c;DBGLNSELFB4INIT;
  }DBGLNSELFB4INIT;
#endif

  //TODO add date/time on filename
  iPid=::getpid();DBGLNSELFB4INIT;
  if(bPidAllowed){DBGLNSELFB4INIT;
    ssDbgMsgFileName<<".pid"<<iPid;DBGLNSELFB4INIT;
  }DBGLNSELFB4INIT;

  ssDbgMsgFileNameCrash<<ssDbgMsgFileName.str()<<".Crash";DBGLNSELFB4INIT;
  if(!bPidAllowed){DBGLNSELFB4INIT;
    ssDbgMsgFileNameCrash<<".pid"<<iPid;DBGLNSELFB4INIT; //this will only be generated if it crashes
  }DBGLNSELFB4INIT;

  ssDbgMsgFileNameCrash<<".dbgmsg.log";DBGLNSELFB4INIT; //suffix
  ssDbgMsgFileName     <<".dbgmsg.log";DBGLNSELFB4INIT; //suffix

  DBGOE(ssDbgMsgFileName.str());DBGLNSELFB4INIT;
}

#define SYNCHRONIZED(mutexVar) for(std::unique_lock<std::recursive_mutex> ulk(mutexVar); ulk; ulk.unlock())
std::recursive_mutex mtxAddMsg;
std::thread::id otherAddMsgThreadId;
void dbgmsg::addDbgMsgLog(std::stringstream& ss)
{
  /**
   * easify multilines
   */
  std::string strPart;
  while(std::getline(ss, strPart)) {
    while(strPart[strPart.size()-1]=='\n'){
      if(strPart.size()==1)return;//ignore empty (multi)lines
      strPart=strPart.substr(0,strPart.size()-1);
    }
    std::stringstream ssL;ssL<<strPart;
    addDbgMsgLogLine(ssL);
  }
}
void dbgmsg::addDbgMsgLogLine(std::stringstream& ss)
{
  SYNCHRONIZED(mtxAddMsg){
    LazyConstructor();

    if(bAddingLog){
      // dbgmsg internal error
      DBGOE("already adding log!!!");
      DBGOE("id="<<llDbgmsgId);
      DBGOE("v size = "<<vLastDbgMsgs.size());
      DBGOE("Other thread id: "<<otherAddMsgThreadId);
      DBGOE("This  thread id: "<<std::this_thread::get_id());

      DBGOE("exiting now!!!");

#ifdef UNIX
      getCurrentStackTraceSS(true,false);
#endif
      
      exit(1); //TODO not completely exiting... crash it?
    }

    bAddingLog=true;

    otherAddMsgThreadId=std::this_thread::get_id();

    //keep this //std::ostream& o=std::cout;DBGLNSELFB4INIT; //keep for self debug if needed (beware to not send NULL to it or that output will break!!!)

    if(ssDbgMsgFileName.str().empty()){DBGLNSELFB4INIT;
      init();DBGLNSELFB4INIT;
    }DBGLNSELFB4INIT;

    if(!fldDbgMsg.is_open()){DBGLNSELFB4INIT;
      fldDbgMsg.open(ssDbgMsgFileName.str());DBGLNSELFB4INIT;
    }DBGLNSELFB4INIT;

    std::stringstream ssDump;DBGLNSELFB4INIT;

    if(bPrependDtTm){DBGLNSELFB4INIT;
#ifdef UNIX
      /**
       * This is to try to prevent a rare freeze/lockdown when calling strftime()
       * that calls __GI___xstat() that uses /etc/localtime.
       * May be it happens only in debug mode using a debugger?
       */
      static bool bInitTZenv = [](){if(!getenv("TZ"))setenv("TZ", ":/etc/localtime", 0);;return true;}();if(bInitTZenv){};
#endif
      static int iTmSz=100;
      char cTime[iTmSz];
      time_t rawtime;
      time(&rawtime);
//      misctools::TimeoutAnyFunction([cTime,iTmSz,rawtime](){strftime(cTime,iTmSz,"%Y/%m/%d-%H:%M:%S",localtime(&rawtime));}(),
//        100000,false,false)
      strftime(cTime,iTmSz,"%Y/%m/%d-%H:%M:%S",localtime(&rawtime));
    //  strftime(cTime,iTmSz,"%Y/%m/%d-%H:%M:%S",localtime(&(attr.st_mtime)));
      ssDump<<cTime;
    }DBGLNSELFB4INIT;

    if(bPrependDbgmsgId){DBGLNSELFB4INIT;
      ssDump<<"("<<llDbgmsgId<<")";
    }DBGLNSELFB4INIT;

    ssDump<<" "<<ss.str();DBGLNSELFB4INIT;

    vLastDbgMsgs.push_back(ssDump.str());DBGLNSELFB4INIT;
    if(vLastDbgMsgs.size()>100) //fail safe TODO iMaxCrashLinesInMemory may be not initialized?
      while(vLastDbgMsgs.size()>iMaxCrashLinesInMemory){
        vLastDbgMsgs.erase(vLastDbgMsgs.begin());
    }DBGLNSELFB4INIT;

  //  fldDbgMsg<<" d"<<(llDbgmsgId++)<<" @ "<<ss.str()<<std::endl;
  //  fldDbgMsg()<<" ";DBGLNSELFB4INIT;fldDbgMsg()<<ssDump.str();DBGLNSELFB4INIT;fldDbgMsg()<<std::endl;DBGLNSELFB4INIT;
    fldDbgMsg<<" "<<ssDump.str()<<std::endl;DBGLNSELFB4INIT;
    fldDbgMsg.flush();DBGLNSELFB4INIT; //TODO unnecessary?

    if(iMaxLinesInDebugFile>0 && llDbgmsgId>0 && ((llDbgmsgId % iMaxLinesInDebugFile) == 0)){DBGLNSELFB4INIT; //TODO is it helping preventing high IO ?
      std::stringstream ssOldFileName;DBGLNSELFB4INIT;
      ssOldFileName<<ssDbgMsgFileName.str()<<".old";DBGLNSELFB4INIT;
      
      std::remove(ssOldFileName.str().c_str());DBGLNSELFB4INIT; //clean older b4 renaming
      
      fldDbgMsg.close();DBGLNSELFB4INIT;
      
      std::rename(ssDbgMsgFileName.str().c_str(), ssOldFileName.str().c_str());DBGLNSELFB4INIT;
    }DBGLNSELFB4INIT;

    llDbgmsgId++; //prepare next id

  //  fldDbgMsg().close(); //TODO prevents trunc on segfault? no...
    bAddingLog=false;
  }
}

bool bAnnoyingNullInfo=false;
void dbgmsg::addDbgMsgLogTmp(){
  addDbgMsgLog(ssDbgMsgTmp);
  ssDbgMsgTmp.str(std::string()); //empty it

  if(bAnnoyingNullInfo){
    // if there was a NULL, it will break the temp stream variable
    ssDbgMsgTmp<<"test";
    if(ssDbgMsgTmp.rdbuf()->in_avail()==0){
      fldDbgMsg<<" "<<"!!!!!!!!!!!! DbgMsg problem: some NULL went to the stream !!!!!!!!!!!!"<<std::endl;
    }
    ssDbgMsgTmp.str(std::string()); //empty it from "test" now
  }
  ssDbgMsgTmp.clear(); //properly clear it (even clearing problems caused by passing NULL to it)

//  //////////////////////////////////////////////////////////////////
//  //TODO remove below here, just checking if it is really working...
//  ssDbgMsgTmp<<"test";
//  if(ssDbgMsgTmp.rdbuf()->in_avail()==0){
//    DBGOEL("!!!!!!!!!!!!!! unable to fix the stream !!!!!!!!!!!!!!");
//    exit(1);
//  }
//  ssDbgMsgTmp.str(std::string()); //empty it from "test" now
//  ssDbgMsgTmp.clear(); //properly clear it (even clearing problems caused by passing NULL to it)
}

//////////////////////////////////////////////////////////////////////////////////////////
#ifdef UNIX
  void dbgmsg::SigHndlr(int iSig)
  {
    char* cSigName=strsignal(iSig);

    //1st! make it promptly visible!
    std::stringstream ssSig;ssSig<<"DBGMSG:SIGNAL["<<iSig<<"]='"<<cSigName<<"'";
    DBGOE(ssSig.str());

    //store on log file
    getCurrentStackTraceSS(true,true);

    std::stringstream ss;ss<<"(hit ENTER to exit)";

    if(bWaitOnCrash)addDbgMsgLog(ss);
    addDbgMsgLog(ssSig);
    fldDbgMsg.close(); //this does NOT prevents trunc...

    std::ofstream fldDbgMsgCrash;
    fldDbgMsgCrash.open(ssDbgMsgFileNameCrash.str());
    long long llDbgmsgIdCrash = llDbgmsgId - iMaxCrashLinesInMemory;
    if(llDbgmsgIdCrash<0)llDbgmsgIdCrash=0;
    ulong iMaxCrashLines = 100;
    if(vLastDbgMsgs.size()<iMaxCrashLines)iMaxCrashLines=vLastDbgMsgs.size();
    for(ulong i=0;i<iMaxCrashLines;i++){
  //    fldDbgMsgCrash<<" d"<<(llDbgmsgIdCrash++)<<" @ "<<vLastDbgMsgs[i]<<std::endl;
      fldDbgMsgCrash<<" "<<vLastDbgMsgs[i]<<std::endl;
      fldDbgMsgCrash.flush(); //just to be sure
    }
    fldDbgMsgCrash.close();
    DBGOE("CrashSaved: "<<ssDbgMsgFileNameCrash.str());

    if(bWaitOnCrash){
      DBGOE(ss.str()); //granting it will be readable
      int i=std::scanf("%s",(char*)ss.str().c_str()); //this helps on reading/copying the dbg file before the random inevitable(?) trunc!
      DBGOE(i);
    }

    exit(iSig); //1 or something else to just identify it was handled?
  }

  std::stringstream dbgmsg::dbgPOpen(std::string cmd,bool bEchoCmd)
  {
    std::stringstream ss;
    std::ostringstream osStkCmd;
    osStkCmd<<cmd;
    if(bEchoCmd)
      ss<<"popen():"<<osStkCmd.str()<<std::endl;
    FILE* pipeFile = popen(osStkCmd.str().c_str(),"r");
    if(pipeFile!=NULL){
      const int i=10*1024;
      char buf[i];
      int j=fread(buf,1,i,pipeFile);
      ss<<"fread():"<<j;
      if(j>0)
        ss<<(buf);
      pclose(pipeFile);
    }else{
      ss<<"unable to execute popen() with cmd: "<<osStkCmd.str();
    }

    return ss;
  }

  /**
   * TODO WIP (not working?)
   */
  std::stringstream dbgmsg::DemangledPStackTrace(bool bShowNow, bool bLog, std::stringstream& ssStk) //TODO opt: show log
  {
    std::stringstream ssRet;
    
    if(bShowNow){}

    ssRet<<"DemangledStackTrace:";
    
    /**
     * TODO 
     * sometimes pstack will not return freezing the application, just use the stack collected from here instead!
     * pstack is not working anyway...
    {
      std::stringstream ss;

      ss<<"pstack "<<iPid; 
      ssRet<<dbgPOpen(ss.str()).str()<<std::endl;

      ss<<" |c++filt";
      ssRet<<dbgPOpen(ss.str()).str()<<std::endl;
    }
     */

    {
      std::stringstream ss;
//      ss.str(std::string());
//      ss.clear();
      ssRet<<"echo '...' |c++filt"<<std::endl; //just a short info about what will be done
      ss<<"echo '"<<ssStk.str()<<"' |c++filt";
      ssRet<<dbgPOpen(ss.str(),false).str()<<std::endl;
    }

    if(bLog){
      std::string line;
      std::stringstream ss;
      while(std::getline(ssRet,line,'\n')){
        ss.str(std::string());ss.clear();
        ss<<line;
        addDbgMsgLog(ss);
      }
    }

    return ssRet;
  }

  long dbgmsg::debuggerPid(){DBGLNSELF;
    std::ostringstream osStkCmd;DBGLNSELF;
    osStkCmd<<"cat /proc/"<<dbgmsg::iPid<<"/status |grep TracerPid |egrep \"[[:digit:]]*\" -o";DBGLNSELF; //TODO is this broad enough?
    FILE* pipeFile = popen(osStkCmd.str().c_str(),"r");DBGLNSELF;
    if(pipeFile!=NULL){DBGLNSELF;
      static const int i=10*1024;DBGLNSELF;
      char buf[i];DBGLNSELF;
      if(fread(buf,1,i,pipeFile)>0){DBGOEL(buf);DBGOEL(atol(buf));
        return atol(buf);
      }
    }else{
      DBGOEL("unable to execute popen() with cmd: "<<osStkCmd.str().c_str());
    }

    DBGLNSELF;
    return 0;
  }

  void dbgmsg::breakPointSimulator(){DBGLNSELF;
    if(debuggerPid()!=0){DBGLNSELF;
      bool b=true;
      if(b){DBGLNSELF; //if possible, set to false AT THE DEBUGGER to continue
        DBGOEL("Simulating a breakpoint.");
        int* i=NULL;(*i)++;
      }
    }
  }

  int iBufSize=1024*10; //TODO 1024? whatsa good value?
  std::stringstream dbgmsg::getCurrentStackTraceSS(bool bShowNow, bool bLog){
    void* paStkBuff[iBufSize];
    int iTot = backtrace(paStkBuff, iBufSize); //get it
    char** paBtSymb = backtrace_symbols(paStkBuff,iTot);

    std::stringstream ssStk;
    for(int i=0;i<iTot;i++){
      char* c=paBtSymb[i];
      if(c==NULL)break;
      std::stringstream ss;ss<<"\t"<<c;
      if(bLog)addDbgMsgLog(ss);
      ssStk<<ss.str()<<std::endl;
      break;
    }
    free(paBtSymb);

    if(bShowNow){
      DBGOE(""
        <<"DBGMSG:CurrentStackTrace:Begin >>--->\n"
        <<ssStk.str().c_str()<<"\n"
        <<"DBGMSG:CurrentStackTrace:End   <---<<"
      );
      
      /**
       * For quality/safety/failProof try, just directly show the details on term
       * 
       * btw, all of these randomly freezes the app on linux :(, may be due to several outputs happening very fast to cout and cerr?
      DBGOE(c);
      backtrace_symbols_fd(paStkBuff,riTot,STDOUT_FILENO);DBGLNSELF;
      backtrace_symbols_fd(paStkBuff,riTot,STDERR_FILENO);DBGLNSELF;
      */
    }
    
    return DemangledPStackTrace(bShowNow,bLog,ssStk);
  }
#endif //UNIX

#ifdef DBGMSG_SELF_TEST
int main(){  // just to compile...
  DBGGETV("strTst1","B");
  DBGGETV("strTst1","C");
  DBGSETV("strTst1","A");
  DBGGETV("strTst1","D");
  return 0;
}
#endif

#endif //DBGMSG
