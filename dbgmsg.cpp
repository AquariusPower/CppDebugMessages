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

//#include <stdlib.h> //getenv
//#include <errno.h> //program_invocation_short_name
#include <string.h> //strlen
#include <unistd.h> //std::getpid()

#include "dbgmsg.h"

stringstream dbgmsg::ssDbgMsgTmp;
ofstream dbgmsg::fldDbgMsg;
stringstream dbgmsg::ssDbgMsgFileName;
stringstream* dbgmsg::pssDbgMsgPath=NULL;
bool dbgmsg::bPidAllowed=false;
unsigned long long dbgmsg::llDbgmsgId=0;

void dbgmsg::SetDebugLogPath(const char* c){
  if(c!=NULL){
    ssDbgMsgPath().str(std::string()); //actually clear/empty it = ""
    ssDbgMsgPath()<<c;
    cout<<"DBGMSG: set path: "<<ssDbgMsgPath().str()<<endl;
  }
  //TODO validate path ?
}

void dbgmsg::addDbgmsgTmp(){
  addDbgmsg(ssDbgMsgTmp);
  ssDbgMsgTmp.str(std::string()); //empty it

  // if there was a NULL, it will break the temp stream variable
  ssDbgMsgTmp<<"test";
  if(ssDbgMsgTmp.rdbuf()->in_avail()==0){
    fldDbgMsg<<" "<<"!!!!!!!!!!!! DbgMsg problem: some NULL went to the stream !!!!!!!!!!!!"<<endl;
  }
  ssDbgMsgTmp.str(std::string()); //empty it from "test" now

  ssDbgMsgTmp.clear(); //properly clear it (even clearing problems caused by passing NULL to it)
}
void dbgmsg::addDbgmsg(stringstream& ss){
  ostream& o=cout; //self debug if needed (beware to not send NULL to it or that output will break!!!)
  if(ssDbgMsgFileName.str().empty()){
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
      ssDbgMsgFileName<<"./";
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

    cerr<<"dbgmsgLogE:"<<ssDbgMsgFileName.str()<<endl; //sometimes cerr wont show anything (broken by NULL?)
    cout<<"dbgmsgLogO:"<<ssDbgMsgFileName.str()<<endl; //sometimes cout wont show anything (broken by NULL?)
  }

  if(!fldDbgMsg.is_open()){
    fldDbgMsg.open(ssDbgMsgFileName.str());
  }

  fldDbgMsg<<" "<<(llDbgmsgId++)<<" #"<<ss.str()<<endl;
  fldDbgMsg.flush();
}

#endif
