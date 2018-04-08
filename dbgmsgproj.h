#ifndef INCLUDE_DBGMSGPROJ_H_
#define INCLUDE_DBGMSGPROJ_H_

// just copy this file freely anywhere or it's contents
#ifdef DBGMSG
  #include "dbgmsg.h"

  /************************************************************
   * CUSTOM / PROJECT SPECIFIC, modify at will
   ************************************************************/
  #ifndef DBGMSG_OBJ //do NOT define this in your project cpp files!
    
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
  #define DBGB(b)
  #define DBGSTK
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

  // project specific erasers:
  //TODO you do it

#endif //DBGMSG

#endif //INCLUDE_DBGMSGPROJ_H_
