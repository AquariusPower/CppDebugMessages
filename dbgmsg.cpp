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

#include "dbgmsg.h"

stringstream dbgmsg::ssDbgMsgTmp;
ofstream dbgmsg::fldDbgMsg;
stringstream dbgmsg::ssDbgMsgFileName;
stringstream* dbgmsg::pssDbgMsgPath=NULL;

void dbgmsg::SetDebugLogPath(const char* c){
  if(c!=NULL){
    ssDbgMsgPath().str(std::string()); //actually clear/empty it = ""
    ssDbgMsgPath()<<c;
    cout<<"DBGMSG: set path: "<<ssDbgMsgPath().str()<<endl;
  }
  //TODO validate path ?
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
      ssDbgMsgFileName<<ssDbgMsgPath().str()<<"/"; //TODO test
    }else{
      ssDbgMsgFileName<<"./";
    }

    // filename
#ifdef __USE_GNU
    char* c=program_invocation_short_name; //auto "guess work" that should work fine
    if(c!=NULL){
      ssDbgMsgFileName<<"."<<c; //TODO test
    }
#else
    ssDbgMsgFileName<<".pid"<<::getpid(); //TODO test
#endif

    ssDbgMsgFileName<<".dbgmsg.log"; //suffix

    cerr<<"dbgmsgLogE:"<<ssDbgMsgFileName.str()<<endl; //sometimes cerr wont show anything (broken by NULL?)
    cout<<"dbgmsgLogO:"<<ssDbgMsgFileName.str()<<endl; //sometimes cout wont show anything (broken by NULL?)
  }

  if(!fldDbgMsg.is_open()){
    fldDbgMsg.open(ssDbgMsgFileName.str());
  }

  fldDbgMsg<<ss.str()<<endl;
  ssDbgMsgTmp.str(std::string()); //actually clear/empty it = ""
  fldDbgMsg.flush();
}

#endif
