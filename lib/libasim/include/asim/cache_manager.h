/*****************************************************************************
 *
 * @brief Header file for Cache Manager
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

#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include "asim/line_status.h"
#include "asim/syntax.h"
#include <map>
#include <string>
#include <utility>

class CACHE_MANAGER
{
  protected:
    class LINE_MANAGER
    {
        struct LINE_STATE
        {
            LINE_STATUS GetStatus() const;
            LINE_STATUS GetStatus(UINT32 owner) const;
            void SetStatus(UINT32 owner, LINE_STATUS status);
            bool IsValid() const
            {
                return !state_.empty();
            };

            std::map<UINT32 /* Owner */, LINE_STATUS /* Status */> state_;
        };

      public:
        typedef std::pair<UINT32 /* Index */, UINT64 /* Tag */> ADDRESS;

        LINE_MANAGER();
        ~LINE_MANAGER();

        LINE_STATUS GetStatus(ADDRESS address) const;
        LINE_STATUS GetStatus(UINT32 owner, ADDRESS address) const;
        void SetStatus(UINT32 owner, ADDRESS address, LINE_STATUS status);
        
      private:
        std::map<ADDRESS, LINE_STATE> addr2status_;
        std::string level_;
    };

    typedef LINE_MANAGER::ADDRESS ADDRESS;

    CACHE_MANAGER();
    virtual ~CACHE_MANAGER();

  public:
    static CACHE_MANAGER& GetInstance();
    virtual void Register(std::string level);

    LINE_STATUS GetStatus(std::string level, UINT32 index, UINT64 tag);
    LINE_STATUS GetStatus(std::string level, UINT32 owner, UINT32 index, UINT64 tag);
    void SetStatus(std::string level, UINT32 owner, UINT32 index, UINT64 tag, LINE_STATUS status);
    
  private:
    std::map<std::string, LINE_MANAGER> str2manager_;
    
    bool clear_lines;
    bool activated;
  
  protected:
    virtual LINE_MANAGER *find_line_manager(std::string level);

  public:
    void setClearLines() { clear_lines = true; }
    bool getClearLines() { return clear_lines; }
    
    void deactivate() { activated = false; }

};

#endif
