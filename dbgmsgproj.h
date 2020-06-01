/**
 * !!!WARNING!!!
 *  This file cannot be included in other .h files TODO why?
 *  These macros cannot be used at inline methods declared in .h files TODO why?
 *  If you do this, it may SEGFAULT only when being run. 
 *   Compiler and linker will warn nothing about the problem!
 *  On linux this check can be added prior to compile: `egrep "dbgmsgproj.h" * -rnI --include "*.h"`
 *  Just copy this file freely anywhere or it's contents.
 */

#ifndef INCLUDE_DBGMSGPROJ_H_

  #define INCLUDE_DBGMSGPROJ_H_

  #ifdef DBGMSG

    #include "dbgmsg.h"

    /************************************************************
     * CUSTOM / PROJECT SPECIFIC, modify at will
     ************************************************************/
    #ifndef DBGMSG_OBJ //do NOT define DBGMSG_OBJ in your project cpp files!
    
    #define DBGEXAMPLE(c) DBGSS(dbgmsgproj::example(c).str())
    class dbgmsgproj{
      public:
      static std::stringstream example(char* pc){ //np to define the full body here to make it easier to maintain
        stringstream ss;
        ss<<pc<<std::endl;
        return ss;
      };
    };

    #endif //DBGMSG_OBJ

  #else
    //dbgmsg macro erasers
    #define DBGOE(s)
    #define DBGSS(s)
    #define DBG1(a)
    #define DBG2(a,b)
    #define DBG3(a,b,c)
    #define DBG4(a,b,c,d)
    #define DBG5(a,b,c,d,e)
    #define DBG6(a,b,c,d,e,f)
    #define DBG7(a,b,c,d,e,f,g)
    #define DBG8(a,b,c,d,e,f,g,h)
    #define DBG9(a,b,c,d,e,f,g,h,i)
    #define DBGLN
    #define DBGSTK
    #define DBGBREAKPOINT
    #define DBGTOSTR_(str)
    #define DBGTOSTR(str)
    #define DBGB(B)
    #define DBGSB(B)
    #define DBGI(I)
    #define DBGSI(I)
    #define DBGF(F)
    #define DBGSF(F)
    #define DBGC(C)
    #define DBGSC(C)
    #define DBGS(SS)
    #define DBGEXEC(cmds)
    #define DBGSETV(id,val)
    #define DBGGETV(id,defval)
    #define DBGGETVD(id,defval)

  // project specific erasers:
  //TODO you do it

  #endif //DBGMSG

#endif //INCLUDE_DBGMSGPROJ_H_
