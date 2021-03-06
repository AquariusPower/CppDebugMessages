//  BSD 3-Clause License
//
//  Copyright (c) 2020, AquariusPower<https://github.com/AquariusPower>
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

#include <algorithm>
#include <chrono>
#include <ctime>
#include <future>
#include <iomanip>
#include <iostream>
#include <ratio>
#include <string>
#include <thread>
#include <type_traits>

#include "extratools.h"

using namespace std::chrono_literals;

const char* envvars::GetStr(cchar* pcID,cchar* pcDefaultValue)
{
  const char* pcValue = std::getenv(pcID);
  if(pcValue!=NULL)
    return pcValue;
  return pcDefaultValue;
}

long envvars::GetLong(cchar* pcID,long lDefaultValue)
{
  const char* pcValue = std::getenv(pcID);
  if(pcValue!=NULL)
    return atol(pcValue);
  return lDefaultValue;
}

ulong envvars::GetULong(cchar* pcID,ulong ulDefaultValue)
{
  const char* pcValue = std::getenv(pcID);
  if(pcValue!=NULL)
    return strtoul(pcValue,NULL,0);
  return ulDefaultValue;
}

double envvars::GetFloatingDouble(cchar* pcID,double dDefaultValue)
{
  const char* pcValue = std::getenv(pcID);
  if(pcValue!=NULL)
    return strtod(pcValue,NULL);
  return dDefaultValue;
}

bool envvars::IsTrue(cchar* pcID)
{
  const char* pcValue = std::getenv(pcID);
  if(pcValue!=NULL){
    std::string str = pcValue;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    static std::string strTrue="true";
    return str.compare(strTrue)==0;
  }
  return false;
}

/**
 * problem: this always return a multiple of 2 (on my machine at least)
 */
ulong misctools::GetNowInNanos()
{
  using namespace std::chrono;
  
  duration<double> now = duration_cast<duration<double>>(
    high_resolution_clock::now().time_since_epoch());
  
  static ulong ulMultToNano = 1000000000;
  
  return (ulong)(now.count()*ulMultToNano);
}

uint CheckDiv2()
{
  for(int i=0;i<1000;i++){
    if(misctools::GetNowInNanos()%2==1)
      return 1;
  }
  
  std::cerr<<"WARNING: nano time is always multiple of 2, using workaround."<<std::endl;
  
  /**
   * dividing by 10 lets it discard the last decimal that is always multiple of 2
   */
  return 10; 
}

/**
 * This may be considered a pseudo random (predictable in some way).
 * @return 0 to 99
 */
ulong misctools::RandomClock100()
{
  /**
   * This looks good on my test machine, for instant and subsequent random value requests.
   * The decimals that varied most were the last 3 ones.
   * The last one is discarded here tho as my test machine has the CheckDiv2() problem.
   * PROBLEM: this needs to be further tested on a extremelly fast hardware to determine if there swill have easily predictible results.
   */
  static uint iRemainderOf = 100; // to consider only the most varying decimals
  
  static uint iDiv=CheckDiv2();
  #define USEFULLVALUE ((GetNowInNanos()/iDiv)%iRemainderOf)
  static ulong ulRet=USEFULLVALUE;
  static ulong ulRetPrevious=USEFULLVALUE;
  
  ulRet += ulRetPrevious;
  static int tot;tot = USEFULLVALUE%10; //std::cout<<"TOT="<<tot<<std::endl;
  for(int i=0;i<tot;i++) // this also creates a delay for the next reading
    ulRet += USEFULLVALUE;
  
  ulRet = ulRet%iRemainderOf;
  
  ulRetPrevious = ulRet; 
  
  return ulRet;
}

void TestRandomClock100()
{
  std::cout.fill('@');
  for(int i=0;i<50;i++){
    ulong ul=misctools::RandomClock100();
    std::cout<<std::setw(ul)<<std::right<<ul<<std::endl;
  }
}

template <typename TypeFunc, typename TypeDur, class... TypeArgs> 
std::result_of_t<TypeFunc&&(TypeArgs&&...)> 
misctools::TimeoutAnyFunction(TypeFunc&& func, TypeDur wait, TypeArgs&&... args)
{
  using res = std::result_of_t<TypeFunc&&(TypeArgs&&...)>;
  std::packaged_task<res(TypeArgs...)> pkgtsk(func);
  auto fut = pkgtsk.get_future();
  std::thread trd(std::move(pkgtsk), std::forward<TypeArgs>(args)...);
  bool bDbg=true;
  if (fut.wait_for(wait) != std::future_status::timeout){
    if(bDbg)std::cerr << "TimeoutAnyFunction:OK:A:" << std::this_thread::get_id()<<std::endl;
    trd.join();
    if(bDbg)std::cerr << "TimeoutAnyFunction:OK:B:" << std::this_thread::get_id()<<std::endl;
    return fut.get(); // propagates exception from timeoutfunc if had any
  }else{
    if(bDbg)std::cerr << "TimeoutAnyFunction:fail&detach:A:" << std::this_thread::get_id()<<std::endl;
    std::cerr << "WARNING: detaching function thread that timed out:" << std::this_thread::get_id()<<std::endl;
    trd.detach();
    if(bDbg)std::cerr << "TimeoutAnyFunction:fail&detach:B:" << std::this_thread::get_id()<<std::endl;
    
    auto tid = std::this_thread::get_id();
    std::stringstream ss;
    ss << "ERROR: Waiting function to finish but it timed out:";
    ss << tid;
    throw std::runtime_error(ss.str().c_str());
  }
}

void TestTimeout()
{
  std::cerr << "TestTimeout:begin:"<<std::this_thread::get_id()<<std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cerr << "TestTimeout:end:"<<std::this_thread::get_id()<<std::endl;
}
void TestTimeoutAnyFunction()
{
  try{
    std::cerr << "TestTimeoutAnyFunction: join" << std::endl;
    misctools::TimeoutAnyFunction(TestTimeout,std::chrono::seconds(3));

//    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cerr << "TestTimeoutAnyFunction: detach" << std::endl;
    misctools::TimeoutAnyFunction(TestTimeout,std::chrono::seconds(1));
  }catch(std::exception& e){
    std::cerr << "ExceptionCatch:" << e.what() << std::endl;
  }
  
  std::cerr << "TestTimeoutAnyFunction:ENDED" << std::endl;
}

int main(int argc, char** argv)
{
  
  static std::string strTstCmd="--test";
  std::string strTstWhat;
  if(argc > 1 && std::string(argv[1]) == strTstCmd)
    strTstWhat=std::string(argv[2]);
  
  #define TESTET(func) { \
    std::cerr << strTstCmd << " \"" << #func << "\"" << std::endl; \
    if(strTstWhat.compare(#func)==0){ \
      std::cerr << "ExtraTools Running: " << #func << std::endl; \
      func; \
    } \
  }

  std::cerr << "ExtraTools: Options" << std::endl;
  TESTET(TestRandomClock100());
  TESTET(TestTimeoutAnyFunction());
  std::cerr << "ExtraTools: ENDED TESTS" << std::endl;
  
  return 0;
}

