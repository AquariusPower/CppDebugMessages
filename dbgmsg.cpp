//  BSD 3-Clause License
//
//  Copyright (c) 2018, AquariusPower<https://github.com/AquariusPower>
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

#define DBGLNSELF {if(!bInitCompleted){ DBGOE(DBGFLF); }};

bool dbgmsg::bInitCompleted;

std::stringstream dbgmsg::ssDbgMsgPartTmp;
std::stringstream dbgmsg::ssDbgMsgTmp;
std::vector<std::string> dbgmsg::vLastDbgMsgs;
std::ofstream dbgmsg::fldDbgMsg;
std::stringstream dbgmsg::ssDbgMsgFileName;
bool dbgmsg::bWaitOnCrash;
bool dbgmsg::bPrependDtTm;
bool dbgmsg::bPrependDbgmsgId;
std::stringstream dbgmsg::ssDbgMsgFileNameCrash;
std::stringstream dbgmsg::ssDbgMsgPath;
bool dbgmsg::bPidAllowed;
int dbgmsg::iPid;
unsigned long long dbgmsg::llDbgmsgId;
int dbgmsg::iMaxLinesInDebugFile;
int dbgmsg::iMaxCrashLinesInMemory;
bool dbgmsg::bAddingLog;

unsigned long long llDesperateInternalInitRandomKey; // TODO why I need this? :/

dbgmsg::dbgmsg(){DBGOE(DBGFLF<<":DBGMSG:RealConstructorIn"); //TODO never run?
  LazyConstructor();
}

void dbgmsg::LazyConstructor(){
  if(llDesperateInternalInitRandomKey==39854029834543289LL)return;

  DBGOE(DBGFLF<<":DBGMSG:In");

  bInitCompleted=false;DBGLNSELF; //FIRST!

  ssDbgMsgPartTmp.clear();DBGLNSELF; //just to init
  ssDbgMsgTmp.clear();DBGLNSELF; //just to init
  vLastDbgMsgs.clear();DBGLNSELF;DBGOE(vLastDbgMsgs.size()) //just to init
  fldDbgMsg.is_open();DBGLNSELF; //just to init
  ssDbgMsgFileName.clear();DBGLNSELF; //just to init
  bWaitOnCrash=false;DBGLNSELF;
  bPrependDtTm=true;DBGLNSELF;
  bPrependDbgmsgId=true;DBGLNSELF;
  ssDbgMsgFileNameCrash.clear();DBGLNSELF; //just to init
  ssDbgMsgPath.clear();DBGLNSELF; //just to init
  bPidAllowed=false;DBGLNSELF;
  iPid=0;DBGLNSELF;
  llDbgmsgId=0;DBGLNSELF;
  iMaxLinesInDebugFile = 100000;DBGLNSELF;
  iMaxCrashLinesInMemory = 1000;DBGLNSELF;
  bAddingLog=false;DBGLNSELF;DBGOE(bAddingLog);

  getCurrentStackTraceSS(true,false);

  llDesperateInternalInitRandomKey=39854029834543289LL;

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

int iBufSize=1024*10; //TODO 1024? whatsa good value?
/**
 * return value needs to be free()
 */
char** dbgmsg::getCurrentStackTrace(bool bShowNow,int& riTot){
  void* paStkBuff[iBufSize];
  riTot = backtrace(paStkBuff, iBufSize); //get it

  if(bShowNow){ // STDERR only
    //for safety/failProof try, just directly show the details on term
    //TODO try/catch?
    std::cerr.flush();std::cerr.clear(); //fit it if needed
    std::cerr<<"DBGMSG:CurrentStackTrace:Begin >>--->"<<std::endl;
    backtrace_symbols_fd(paStkBuff,riTot,STDERR_FILENO);
    std::cerr<<"DBGMSG:CurrentStackTrace:End   <---<<"<<std::endl;
  }

  return backtrace_symbols(paStkBuff,riTot);;
}

void dbgmsg::DemangledPStackTrace(bool bShowNow, bool bLog) //TODO opt: show log
{ //TODO collect per line to log properly TODO use ALSO the stack collected not thru pstack
  std::ostringstream osStkCmd;
  osStkCmd<<"pstack "<<iPid<<" |c++filt";
  FILE* pipeFile = popen(osStkCmd.str().c_str(),"r");
  if(pipeFile!=NULL){
    const int i=10*1024;
    char buf[i];
    if(fread(buf,1,i,pipeFile)>0)
      DBGOEL("DemangledStackTrace:\n"<<buf);
  }else{
    DBGOEL("unable to execute popen() with cmd: "<<osStkCmd.str().c_str());
  }
}

std::stringstream dbgmsg::getCurrentStackTraceSS(bool bShowNow, bool bLog){
  DemangledPStackTrace(bShowNow,bLog);

  int iTot=0;
  char** paBtSymb = getCurrentStackTrace(bShowNow,iTot);

  std::stringstream ss;
  //  for(int i=0;i<iBufSize;i++){
  for(int i=0;i<iTot;i++){
    char* c=paBtSymb[i];
    if(c==NULL)break;
    std::stringstream ss2;ss2<<"\t"<<c;
    if(bLog)addDbgMsgLog(ss2);
    ss<<ss2.str()<<std::endl;
  }
  free(paBtSymb);

//  if(bLog)addDbgMsgLog(ss);

  return ss;
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
  int iMaxCrashLines = 100;
  if(vLastDbgMsgs.size()<iMaxCrashLines)iMaxCrashLines=vLastDbgMsgs.size();
  for(int i=0;i<iMaxCrashLines;i++){
//    fldDbgMsgCrash<<" d"<<(llDbgmsgIdCrash++)<<" @ "<<vLastDbgMsgs[i]<<std::endl;
    fldDbgMsgCrash<<" "<<vLastDbgMsgs[i]<<std::endl;
    fldDbgMsgCrash.flush(); //just to be sure
  }
  fldDbgMsgCrash.close();
  DBGOE("CrashSaved: "<<ssDbgMsgFileNameCrash.str());

  if(bWaitOnCrash){
    DBGOE(ss.str()); //granting it will be readable
    int i=std::scanf("%s",(char*)ss.str().c_str()); //this helps on reading/copying the dbg file before the random inevitable(?) trunc!
  }

  exit(iSig); //1 or something else to just identify it was handled?
}

void dbgmsg::init(){DBGLNSELF;
  LazyConstructor();

  #ifdef UNIX
    initSignals();DBGLNSELF;
  #endif

  initStream();DBGLNSELF;

  getCurrentStackTraceSS(true,false);DBGLNSELF;
//  std::stringstream ss;ss<<"DBGMSG INIT COMPLETED!";DBGLNSELF;
//  addDbgMsgLog(ss);DBGLNSELF;
  DBGOE("DBGMSG INIT COMPLETED!");DBGLNSELF;

  bInitCompleted=true;DBGLNSELF;
}

#ifdef UNIX
void dbgmsg::initSignals(){DBGLNSELF;
  // as specified at `info signal`
  //TODO determine if these handlers are already set (unless it will just add the handler)
  signal(SIGHUP , dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGINT , dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGQUIT, dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGILL , dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGABRT, dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGFPE , dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGKILL, dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGSEGV, dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGPIPE, dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGTERM, dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGBUS , dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGSYS , dbgmsg::SigHndlr);DBGLNSELF;
  signal(SIGTRAP, dbgmsg::SigHndlr);DBGLNSELF;
  //? SIGXFSZ
}
#endif

void dbgmsg::initStream(){DBGLNSELF;
  ssDbgMsgPath<<"";DBGLNSELF;

  // path
#ifdef UNIX
  if(ssDbgMsgPath.str().empty()){DBGLNSELF;
    ssDbgMsgPath<<getenv("HOME")<<"/";DBGLNSELF;
  }DBGLNSELF;
#endif

  if(!ssDbgMsgPath.str().empty()){DBGLNSELF;
    ssDbgMsgFileName<<ssDbgMsgPath.str()<<"/";DBGLNSELF;
  }else{DBGLNSELF;
    ssDbgMsgFileName<<"./";DBGLNSELF; //will be relative to current execution path
  }DBGLNSELF;

  // filename
#ifdef __USE_GNU
  char* c=program_invocation_short_name;DBGLNSELF; //auto "guess work" that should work fine
  if(c!=NULL){DBGLNSELF;
    ssDbgMsgFileName<<"."<<c;DBGLNSELF;
  }DBGLNSELF;
#endif

  //TODO add date/time on filename
  iPid=::getpid();DBGLNSELF;
  if(bPidAllowed){DBGLNSELF;
    ssDbgMsgFileName<<".pid"<<iPid;DBGLNSELF;
  }DBGLNSELF;

  ssDbgMsgFileNameCrash<<ssDbgMsgFileName.str()<<".Crash";DBGLNSELF;
  if(!bPidAllowed){DBGLNSELF;
    ssDbgMsgFileNameCrash<<".pid"<<iPid;DBGLNSELF; //this will only be generated if it crashes
  }DBGLNSELF;

  ssDbgMsgFileNameCrash<<".dbgmsg.log";DBGLNSELF; //suffix
  ssDbgMsgFileName     <<".dbgmsg.log";DBGLNSELF; //suffix

  DBGOE(ssDbgMsgFileName.str());DBGLNSELF;
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

      getCurrentStackTraceSS(true,false);
      DBGOE("exiting now!!!");
      exit(1); //TODO not completely exiting... crash it?
    }

    bAddingLog=true;

    otherAddMsgThreadId=std::this_thread::get_id();

    //keep this //std::ostream& o=std::cout;DBGLNSELF; //keep for self debug if needed (beware to not send NULL to it or that output will break!!!)

    if(ssDbgMsgFileName.str().empty()){DBGLNSELF;
      init();DBGLNSELF;
    }DBGLNSELF;

    if(!fldDbgMsg.is_open()){DBGLNSELF;
      fldDbgMsg.open(ssDbgMsgFileName.str());DBGLNSELF;
    }DBGLNSELF;

    std::stringstream ssDump;DBGLNSELF;

    if(bPrependDtTm){DBGLNSELF;
      static int iTmSz=100;
      char cTime[iTmSz];
      time_t rawtime;
      time(&rawtime);
      strftime(cTime,iTmSz,"%Y/%m/%d-%H:%M:%S",localtime(&rawtime));
    //  strftime(cTime,iTmSz,"%Y/%m/%d-%H:%M:%S",localtime(&(attr.st_mtime)));
      ssDump<<cTime;
    }DBGLNSELF;

    if(bPrependDbgmsgId){DBGLNSELF;
      ssDump<<"("<<llDbgmsgId<<")";
    }DBGLNSELF;

    ssDump<<" "<<ss.str();DBGLNSELF;

    vLastDbgMsgs.push_back(ssDump.str());DBGLNSELF;
    if(vLastDbgMsgs.size()>100) //fail safe TODO iMaxCrashLinesInMemory may be not initialized?
      while(vLastDbgMsgs.size()>iMaxCrashLinesInMemory){
        vLastDbgMsgs.erase(vLastDbgMsgs.begin());
    }DBGLNSELF;

  //  fldDbgMsg<<" d"<<(llDbgmsgId++)<<" @ "<<ss.str()<<std::endl;
  //  fldDbgMsg()<<" ";DBGLNSELF;fldDbgMsg()<<ssDump.str();DBGLNSELF;fldDbgMsg()<<std::endl;DBGLNSELF;
    fldDbgMsg<<" "<<ssDump.str()<<std::endl;DBGLNSELF;
    fldDbgMsg.flush();DBGLNSELF; //TODO unnecessary?

    if(iMaxLinesInDebugFile>0 && llDbgmsgId>0 && ((llDbgmsgId % iMaxLinesInDebugFile) == 0)){DBGLNSELF; //TODO is it helping preventing high IO ?
      std::stringstream ssOldFileName;DBGLNSELF;
      ssOldFileName<<ssDbgMsgFileName.str()<<".old";DBGLNSELF;

  //    std::stringstream ssTmp;
  //    ssTmp<<" [removing old debug file "<<ssOldFileName.str()<<"]";
  //    std::cerr<<ssTmp.str()<<std::endl;
  //    fldDbgMsg<<ssTmp.str()<<std::endl;
      std::remove(ssOldFileName.str().c_str());DBGLNSELF; //clean older b4 renaming

  //    ssTmp<<" [renaming this debug file to "<<ssOldFileName.str()<<"]";
  //    std::cerr<<ssTmp.str()<<std::endl;
  //    fldDbgMsg<<ssTmp.str()<<std::endl;
      fldDbgMsg.close();DBGLNSELF;

      std::rename(ssDbgMsgFileName.str().c_str(), ssOldFileName.str().c_str());DBGLNSELF;
    }DBGLNSELF;

    llDbgmsgId++; //prepare next id

  //  fldDbgMsg().close(); //TODO prevents trunc on segfault? no...
    bAddingLog=false;
  }
}

void dbgmsg::addDbgMsgLogTmp(){
  addDbgMsgLog(ssDbgMsgTmp);
  ssDbgMsgTmp.str(std::string()); //empty it

  // if there was a NULL, it will break the temp stream variable
  ssDbgMsgTmp<<"test";
  if(ssDbgMsgTmp.rdbuf()->in_avail()==0){
    fldDbgMsg<<" "<<"!!!!!!!!!!!! DbgMsg problem: some NULL went to the stream !!!!!!!!!!!!"<<std::endl;
  }
  ssDbgMsgTmp.str(std::string()); //empty it from "test" now

  ssDbgMsgTmp.clear(); //properly clear it (even clearing problems caused by passing NULL to it)




  //////////////////////////////////////////////////////////////////
  //TODO remove below here, just checking if it is really working...
  ssDbgMsgTmp<<"test";
  if(ssDbgMsgTmp.rdbuf()->in_avail()==0){
    DBGOEL("!!!!!!!!!!!!!! unable to fix the stream !!!!!!!!!!!!!!");
    exit(1);
  }
  ssDbgMsgTmp.str(std::string()); //empty it from "test" now
  ssDbgMsgTmp.clear(); //properly clear it (even clearing problems caused by passing NULL to it)
}

#endif
