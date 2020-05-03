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

#include <string>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <ratio>
#include <iomanip>

#include "extratools.h"

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

ulong misctools::RandomClock100()
{
//  static ulong ulNow=0;
//  static ulong ulAdd=0;
  static ulong ulRet=0;
  
//  static ulong retry=0;
//  retry = (GetNowInNanos()/10)%10;
////  std::cout<<"RETRY="<<retry<<std::endl;
//  for(uint i=0;i<retry;i++){
//    ulNow=GetNowInNanos()/10; //ignore the last decimal that is always multiple of 2
//  }
  
  /**
   * /10 to Ignore the last decimal that is always multiple of 2.
   * %100 consider only the most varying decimals.
   */
  #define USEFULLVALUE ((GetNowInNanos()/10)%100)
  
//  ulNow = GetNowInNanos();
//  
//  //ignore the last decimal that is always multiple of 2
//  ulRet = ulNow/10; 
//  
//  //consider only remaining 2 decimals: 0 to 99
//  ulRet %= 100;
//  
//  //trick
//  ulAdd = 1+((GetNowInNanos()/10)%100);
//  ulNow += ulAdd;
  
  ulRet = USEFULLVALUE + (USEFULLVALUE%2==0 ? USEFULLVALUE : 0);
  
  return ulRet%100; 
}

//TODO timeout function

int main()
{
  std::cout.fill('@');
  for(int i=0;i<100;i++){
//    ulong ul=misctools::Random();
//    std::cout<<ul<<std::endl;
    
    ulong ul=misctools::RandomClock100();
    std::cout<<std::setw(ul)<<std::right<<ul<<std::endl;
    
//    std::cout<<misctools::RandomClock100()<<std::endl;
    
//    std::cout<<misctools::GetNowInNanos()<<std::endl;
  }
  
  return 0;
}

