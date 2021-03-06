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

#ifndef __EXTRATOOLS_H__
  #define __EXTRATOOLS_H__
  
  typedef const char cchar;

  class envvars
  {
    public:
      static const char* GetStr(cchar* pcID,cchar* pcDefaultValue="");
      static long GetLong(cchar* pcID,long lDefaultValue=0);
      static bool IsTrue(cchar* pcID);
      ulong GetULong(cchar* pcID,ulong ulDefaultValue);
      double GetFloatingDouble(cchar* pcID,double dDefaultValue);
  };
  
  typedef void (*timeoutfunc)();
  class misctools
  {
    public:
      static ulong RandomClock100();
      static ulong GetNowInNanos();
      template <typename TypeFunc, typename TypeDur, class... TypeArgs> 
        static std::result_of_t<TypeFunc&&(TypeArgs&&...)> 
          TimeoutAnyFunction(TypeFunc&& func, TypeDur wait, TypeArgs&&... args);
  };
  
#endif //__EXTRATOOLS_H__
