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
  #define DBGFLF __FILENAME__<<":"<<__LINE__<<":"<<__FUNCTION__

  #ifdef DBGMSG
    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <vector>

    #ifdef UNIX
    #include <execinfo.h>
    #include <signal.h>
    #endif

    // It would force std on everything... do not use! //using namespace std;

    class dbgmsg{
      public:
        static std::stringstream ssDbgMsgTmp;

//        static const char* b(bool b){return b?"true":"false";}
        static std::string b(bool b,const char* cId=NULL);
        static std::string i(long l,const char* cId=NULL);
        static std::string f(double d,const char* cId=NULL);
        static std::string str(const char* c,const char* cId=NULL);
        static void addDbgMsgLogTmp();
        static void addDbgMsgLog(std::stringstream& ss);
        static void SetDebugLogPath(const char* c);
        static void SetAllowPidOnLogName(){bPidAllowed=true;}
        static void SetWaitOnCrash(bool b){bWaitOnCrash=b;}
        static void SetPrependDtTm(bool b){bPrependDtTm=b;}
        static void SetPrependDbgmsgId(bool b){bPrependDbgmsgId=b;}
        static void SetMaxLinesInDebugFile(int i){iMaxLinesInDebugFile=i;} //no limit if 0
        ~dbgmsg(){ if(fldDbgMsg().is_open())fldDbgMsg().close(); }
        #ifdef UNIX
          static void SigHndlr(int iSig);
          static char**            getCurrentStackTrace  (bool bShowNow, int& riTot);
          static std::stringstream getCurrentStackTraceSS(bool bShowNow, bool bLog );
        #endif
      private:
        #ifdef UNIX
          static void initSignals();
        #endif

        static void init();
        static void initStream();
        static std::string id(const char* cId);
        static std::stringstream& ssDbgMsgPath();
        static std::stringstream& ssDbgMsgFileName();
        static std::stringstream& ssDbgMsgFileNameCrash();
        static std::ofstream& fldDbgMsg();

        static unsigned long long llDbgmsgId;
        static std::ofstream* pfldDbgMsg;
        static std::stringstream* pssDbgMsgFileName;
        static std::stringstream* pssDbgMsgFileNameCrash;
        static bool bWaitOnCrash;
        static std::stringstream* pssDbgMsgPath;
        static bool bPidAllowed;
        static std::stringstream ssDbgMsgPartTmp;
        static int iPid;
        static int iMaxLinesInDebugFile;
        static std::vector<std::string> vLastDbgMsgs;
        static bool bPrependDtTm;
        static bool bPrependDbgmsgId;
    };

    #define DBGCTSV(ex) "{"<<DBGTOSTR(ex)<<"}=\""<<(ex)<<"\";" //DBG "code to string" and value
    #define DBGSCTSV(ex) DBGSS(DBGSCTSV(ex))

    /* easy/non-cumbersome debug messages */
    // base stream SS
    #define DBGSS(s) { dbgmsg::ssDbgMsgTmp<<DBGFLF<<":"<<s; dbgmsg::addDbgMsgLogTmp(); }
    //TODO #define DBGN(chk) "("<<(chk==NULL?"NULL":chk)<<")"
    #define DBG1(a) DBGSS(DBGCTSV(a))
    #define DBG2(a,b) DBGSS(DBGCTSV(a)<<DBGCTSV(b))
    #define DBG3(a,b,c) DBGSS(DBGCTSV(a)<<DBGCTSV(b)<<DBGCTSV(c))
    #define DBG4(a,b,c,d) DBGSS(DBGCTSV(a)<<DBGCTSV(b)<<DBGCTSV(c)<<DBGCTSV(d))
    #define DBG5(a,b,c,d,e) DBGSS(DBGCTSV(a)<<DBGCTSV(b)<<DBGCTSV(c)<<DBGCTSV(d)<<DBGCTSV(e))
    #define DBG6(a,b,c,d,e,f) DBGSS(DBGCTSV(a)<<DBGCTSV(b)<<DBGCTSV(c)<<DBGCTSV(d)<<DBGCTSV(e)<<DBGCTSV(f))
    #define DBG7(a,b,c,d,e,f,g) DBGSS(DBGCTSV(a)<<DBGCTSV(b)<<DBGCTSV(c)<<DBGCTSV(d)<<DBGCTSV(e)<<DBGCTSV(f)<<DBGCTSV(g))
    #define DBG8(a,b,c,d,e,f,g,h) DBGSS(DBGCTSV(a)<<DBGCTSV(b)<<DBGCTSV(c)<<DBGCTSV(d)<<DBGCTSV(e)<<DBGCTSV(f)<<DBGCTSV(g)<<DBGCTSV(h))
    #define DBG9(a,b,c,d,e,f,g,h,i) DBGSS(DBGCTSV(a)<<DBGCTSV(b)<<DBGCTSV(c)<<DBGCTSV(d)<<DBGCTSV(e)<<DBGCTSV(f)<<DBGCTSV(g)<<DBGCTSV(h)<<DBGCTSV(i))
    #define DBGLN DBGSS("(ReachedHere)")

    // double expansion pre-processor variable identifier trick
    #define DBGTOSTR_(str)  #str
    #define DBGTOSTR(str)  DBGTOSTR_(str)

    #define DBGB(B) (dbgmsg::b(B,DBGTOSTR(B)))
    #define DBGSB(B) DBG1(B)

    #define DBGI(I) (dbgmsg::i(I,DBGTOSTR(I))) //integer numbers
    #define DBGSI(I) DBG1(I)

    #define DBGF(F) (dbgmsg::f(F,DBGTOSTR(F))) //floating numbers
    #define DBGSF(F) DBG1(F)

    #define DBGC(C) (dbgmsg::str(C,DBGTOSTR(C))) //char*
    #define DBGSC(C) DBG1(C)

    #define DBGS(SS) (dbgmsg::str(SS.str().c_str(),DBGTOSTR(SS))) //stringstream

    //too messy... #define DBGEXEC(cmds) {DBGSS(DBGTOSTR(cmds));cmds;}
    #define DBGEXEC(cmds) {cmds;} //this helps a lot by avoiding #ifdef for DBGMSG

    #ifdef UNIX
      #define DBGSTK DBGSS("DBGMSG:ShowCurrentStackTrace:"<<std::endl<<dbgmsg::getCurrentStackTraceSS(true,true).str()<<std::endl)
    #endif

  #endif //DBGMSG

#endif //__DBGMSG_H__
