#ifndef FELIB_INCLUDE_DBGMSG_PROJECT_H_
#define FELIB_INCLUDE_DBGMSG_PROJECT_H_

/************************************************************
 * CUSTOM / PROJECT SPECIFIC, modify at will
 ************************************************************/
#ifndef DBGMSG_OBJ //do NOT define this in your project cpp files!

  #define DBGEXAMPLE(c) DBGSS(dbgmsg_project::example(c).str())
  class dbgmsg_project{
    public:
    static std::stringstream example(char* pc){ //np to define the full body here to make it easier to maintain
      stringstream ss;
      ss<<pc<<std::endl;
      return ss;
    };
  };

#endif //DBGMSG_GLOBAL

#endif /* FELIB_INCLUDE_DBGMSG_PROJECT_H_ */
