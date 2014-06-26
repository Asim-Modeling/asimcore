/*****************************************************************************
 *
 *
 * @author Oscar Rosell
 *
 * Copyright (c) 2014, Intel Corporation
 *
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright 
 *   notice, this list of conditions and the following disclaimer in the 
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the Intel Corporation nor the names of its 
 *   contributors may be used to endorse or promote products derived from 
 *   this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PORT_HANDLER_H
#define PORT_HANDLER_H

#include "asim/syntax.h"

/**********************************************************
*
*   Port Handlers can be used when we want to use ports
*   polymorphically. Normal ports don't allow polymorphism
*   for performance reasons but sometimes code can be much
*   simpler this way.
*
**********************************************************/

template<class T>
class WRITE_PORT_HANDLER_BASE
{
  public:
    WRITE_PORT_HANDLER_BASE(){}; 
    virtual ~WRITE_PORT_HANDLER_BASE(){};
    virtual bool Write(T& msg, UINT64 cycle) = 0;
};

template <class WRITE_PORT, class Type>
class WRITE_PORT_HANDLER : public WRITE_PORT_HANDLER_BASE<Type>
{
  public:
    WRITE_PORT_HANDLER(WRITE_PORT& port)
      : port_(port)
    { }
    virtual bool Write(Type& msg, UINT64 cycle)
    {
        return port_.Write(msg, cycle);
    }
  private:
    WRITE_PORT& port_;
};

template<class T>
class READ_PORT_HANDLER_BASE
{
  public:
    READ_PORT_HANDLER_BASE(){};
    virtual ~READ_PORT_HANDLER_BASE(){};
    virtual bool Read(T& msg, UINT64 cycle) = 0;
};

template <class READ_PORT, class Type>
class READ_PORT_HANDLER : public READ_PORT_HANDLER_BASE<Type>
{
  public:
    READ_PORT_HANDLER(READ_PORT& port)
      : port_(port)
    { }
    virtual bool Read(Type& msg, UINT64 cycle)
    {
        return port_.Read(msg, cycle);
    }
  private:
    READ_PORT& port_;
};

#endif // PORT_HANDLER_H
