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

//#include <stdlib.h> //getenv
//#include <errno.h> //program_invocation_short_name
#include <string.h> //strlen
#include <unistd.h> //std::getpid()

std::stringstream dbgmsg::ssDbgMsgTmp;
std::stringstream dbgmsg::ssDbgMsgPartTmp;
std::ofstream dbgmsg::fldDbgMsg;
std::stringstream dbgmsg::ssDbgMsgFileName;
std::stringstream* dbgmsg::pssDbgMsgPath=NULL;
bool dbgmsg::bPidAllowed=false;
unsigned long long dbgmsg::llDbgmsgId=0;

std::stringstream& dbgmsg::ssDbgMsgPath(){ // had to be a pointer, would not initialize causing segfault...
  if(pssDbgMsgPath==NULL)pssDbgMsgPath=new std::stringstream();
  return (*pssDbgMsgPath);
}

void dbgmsg::SetDebugLogPath(const char* c){
  if(!ssDbgMsgPath().str().empty()){
    std::cout<<"DBGMSG: path already set to '"<<ssDbgMsgPath().str()<<"', asked now '"<<c<<"'"<<std::endl;
    return;
  }

  if(c!=NULL){ //TODO validate path b4 setting it
    ssDbgMsgPath().str(std::string()); //actually clear/empty it = ""
    ssDbgMsgPath().clear(); //good?
    ssDbgMsgPath()<<c;
    std::cout.flush();std::cout.clear(); //fit it if needed
    std::cout<<"DBGMSG: set path: "<<ssDbgMsgPath().str()<<std::endl;
  }
}

int iBufSize=256; //TODO 1024? whatsa good value?
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

std::stringstream dbgmsg::getCurrentStackTraceSS(bool bShowNow, bool bLog){
  int iTot=0;
  char** paBtSymb = getCurrentStackTrace(bShowNow,iTot);

  std::stringstream ss;
  //  for(int i=0;i<iBufSize;i++){
  for(int i=0;i<iTot;i++){
    char* c=paBtSymb[i];
    if(c==NULL)break;
    ss<<"\t"<<c<<std::endl;
  }
  free(paBtSymb);

  if(bLog)addDbgMsgLog(ss);

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
  std::cerr.flush();std::cerr.clear(); //try to make it sure STDERR will work!
  std::cerr<<"DBGMSG:SIGNAL["<<iSig<<"]='"<<cSigName<<"'"<<std::endl; //show it if possible

  // send to stdout too
  std::cout.flush();
  std::cout.clear(); //try to make it sure STDOUT will work!
  std::cout<<cSigName<<std::endl; //show it if possible

  //store on log file
  getCurrentStackTraceSS(true,true);

  std::stringstream ss;ss<<"(hit a key to exit)";addDbgMsgLog(ss);

  fldDbgMsg.close(); //this does NOT prevents trunc...

  std::cout<<ss.str()<<std::endl; //granting it will be readable
  std::cout<<ss.str()<<std::endl; //granting it will be readable
  std::cerr<<ss.str()<<std::endl; //granting it will be readable
  std::cerr<<ss.str()<<std::endl; //granting it will be readable
  std::scanf("%s",(char*)ss.str().c_str()); //this helps on reading/copying the dbg file before the trunc!

  exit(iSig); //1 or something else to just identify it was handled?
}

void dbgmsg::init(){
  #ifdef UNIX
    initSignals();
  #endif

  initStream();
}

#ifdef UNIX
void dbgmsg::initSignals(){
  // as specified at `info signal`
  //TODO determine if these handlers are already set (unless it will just add the handler)
  signal(SIGHUP , dbgmsg::SigHndlr);
  signal(SIGINT , dbgmsg::SigHndlr);
  signal(SIGQUIT, dbgmsg::SigHndlr);
  signal(SIGILL , dbgmsg::SigHndlr);
  signal(SIGABRT, dbgmsg::SigHndlr);
  signal(SIGFPE , dbgmsg::SigHndlr);
  signal(SIGKILL, dbgmsg::SigHndlr);
  signal(SIGSEGV, dbgmsg::SigHndlr);
  signal(SIGPIPE, dbgmsg::SigHndlr);
  signal(SIGTERM, dbgmsg::SigHndlr);
  signal(SIGBUS , dbgmsg::SigHndlr);
  signal(SIGSYS , dbgmsg::SigHndlr);
  signal(SIGTRAP, dbgmsg::SigHndlr);
  //? SIGXFSZ
}
#endif

void dbgmsg::initStream(){
  ssDbgMsgPath()<<"";

  // path
#ifdef UNIX
  if(ssDbgMsgPath().str().empty()){
    ssDbgMsgPath()<<getenv("HOME")<<"/";
  }
#endif

  if(!ssDbgMsgPath().str().empty()){
    ssDbgMsgFileName<<ssDbgMsgPath().str()<<"/";
  }else{
    ssDbgMsgFileName<<"./"; //will be relative to current execution path
  }

  // filename
#ifdef __USE_GNU
  char* c=program_invocation_short_name; //auto "guess work" that should work fine
  if(c!=NULL){
    ssDbgMsgFileName<<"."<<c;
  }
#endif

  //TODO add date/time on filename
  if(bPidAllowed)ssDbgMsgFileName<<".pid"<<::getpid();

  ssDbgMsgFileName<<".dbgmsg.log"; //suffix

  std::cerr.flush();std::cerr.clear();//fix if needed
  std::cerr<<"dbgmsgLogE:"<<ssDbgMsgFileName.str()<<std::endl; //sometimes cerr wont show anything (broken by NULL?)
  std::cout.flush();std::cout.clear();//fix if needed
  std::cout<<"dbgmsgLogO:"<<ssDbgMsgFileName.str()<<std::endl; //sometimes cout wont show anything (broken by NULL?)
}

void dbgmsg::addDbgMsgLog(std::stringstream& ss){
  std::ostream& o=std::cout; //keep for self debug if needed (beware to not send NULL to it or that output will break!!!)
  //debug self helper, put anywhere: std::cerr<<"RM:"<<__LINE__<<std::endl;

  if(ssDbgMsgFileName.str().empty()){
    init();
  }

  if(!fldDbgMsg.is_open()){
    fldDbgMsg.open(ssDbgMsgFileName.str());
  }

  fldDbgMsg<<" d"<<(llDbgmsgId++)<<" @ "<<ss.str()<<std::endl;
  fldDbgMsg.flush();

//  fldDbgMsg.close(); //TODO prevents trunc on segfault? no...
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
}

#endif
