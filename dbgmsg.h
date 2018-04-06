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

#ifndef __DBGMSG_H__
  #define __DBGMSG_H__

  // filename only depends on cpp core
  #define __FILENAME__ (__builtin_strrchr(__FILE__,'/') ? __builtin_strrchr(__FILE__,'/')+1 : __FILE__)

  #ifdef DBGMSG
    #include <iostream>
    #include <fstream>
    #include <sstream>

    #ifdef UNIX
    #include <execinfo.h>
    #include <signal.h>
    #endif

    // It would force std on everything... do not use! //using namespace std;

    class dbgmsg{
      public:
        static std::stringstream ssDbgMsgTmp;
        static void addDbgMsgLogTmp();
        static void addDbgMsgLog(std::stringstream& ss);
        static void SetDebugLogPath(const char* c);
        static void SetAllowPidOnLogName(){bPidAllowed=true;}
        static std::stringstream& ssDbgMsgPath();
        ~dbgmsg(){ if(fldDbgMsg.is_open())fldDbgMsg.close(); }
        #ifdef UNIX
          static void SigHndlr(int iSig);
          static char**            getCurrentStackTrace  (bool bShowNow, int& riTot);
          static std::stringstream getCurrentStackTraceSS(bool bShowNow, bool bLog );
        #endif
      private:
        static void init();
        #ifdef UNIX
          static void initSignals();
        #endif
        static void initStream();
        static unsigned long long llDbgmsgId;
        static std::ofstream fldDbgMsg;
        static std::stringstream ssDbgMsgFileName;
        static std::stringstream* pssDbgMsgPath;
        static bool bPidAllowed;
    };

    /* easy/non-cumbersome debug messages */
    // base stream SS
    #define DBGSS(s) { dbgmsg::ssDbgMsgTmp<<__FILENAME__<<":"<<__LINE__<<":"<<s; dbgmsg::addDbgMsgLogTmp(); }
    //TODO #define DBGN(chk) "("<<(chk==NULL?"NULL":chk)<<")"
    // below wasnt intended to look cool, but... it does IMHO :)
    #define DBG1(a) DBGSS("("<<a<<")")
    #define DBG2(a,b) DBGSS("("<<a<<")("<<b<<")")
    #define DBG3(a,b,c) DBGSS("("<<a<<")("<<b<<")("<<c<<")")
    #define DBG4(a,b,c,d) DBGSS("("<<a<<")("<<b<<")("<<c<<")("<<d<<")")
    #define DBG5(a,b,c,d,e) DBGSS("("<<a<<")("<<b<<")("<<c<<")("<<d<<")("<<e<<")")
    #define DBG6(a,b,c,d,e,f) DBGSS("("<<a<<")("<<b<<")("<<c<<")("<<d<<")("<<e<<")("<<f<<")")
    #define DBG7(a,b,c,d,e,f,g) DBGSS("("<<a<<")("<<b<<")("<<c<<")("<<d<<")("<<e<<")("<<f<<")("<<g<<")")
    #define DBG8(a,b,c,d,e,f,g,h) DBGSS("("<<a<<")("<<b<<")("<<c<<")("<<d<<")("<<e<<")("<<f<<")("<<g<<")("<<h<<")")
    #define DBG9(a,b,c,d,e,f,g,h,i) DBGSS("("<<a<<")("<<b<<")("<<c<<")("<<d<<")("<<e<<")("<<f<<")("<<g<<")("<<h<<")("<<i<<")")
    #define DBGLN DBGSS("(ReachedHere)")
    #define DBGB(b) DBG1((b?"true":"false"))
    #ifdef UNIX
      #define DBGSTK DBGSS("DBGMSG:ShowCurrentStackTrace:"<<std::endl<<dbgmsg::getCurrentStackTraceSS(true,true).str()<<std::endl)
    #endif

  #endif //DBGMSG

#endif //__DBGMSG_H__
