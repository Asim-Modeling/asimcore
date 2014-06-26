/*****************************************************************************
*
* @brief Source file for Cache Manager
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

#include "asim/cache_manager.h"
#include "asim/mesg.h"
#include <iostream>

using namespace std;

LINE_STATUS
CACHE_MANAGER::LINE_MANAGER::LINE_STATE::GetStatus() const
{
    ASSERTX(state_.size() > 0);

    // FIXME I choose to stay on the safe side
    if (state_.size() == 1)
    {
        return S_EXCLUSIVE;
    }
    else
    {
        return S_SHARED;
    }
}

LINE_STATUS
CACHE_MANAGER::LINE_MANAGER::LINE_STATE::GetStatus(UINT32 owner) const
{
    map<UINT32, LINE_STATUS>::const_iterator it = state_.find(owner);
    if (it != state_.end())
    {
        return it->second;
    }
    else
    {
        return S_INVALID;
    }
}

void 
CACHE_MANAGER::LINE_MANAGER::LINE_STATE::SetStatus(UINT32 owner, LINE_STATUS status)
{
    state_[owner] = status;
    // FIXME The line deallocation is deactivated by default and the setClearLines() cache_manager
    // method should be called to activate it. However, with it activated we have race problems
    // on the clients when running with asim/cache.h warming activated.
    if ( ((status == S_INVALID) || (status == S_RESERVED)) &&
         CACHE_MANAGER::GetInstance().getClearLines() )
    {
        state_.erase(owner);
    }
    
}

CACHE_MANAGER::LINE_MANAGER::LINE_MANAGER()
{ }

CACHE_MANAGER::LINE_MANAGER::~LINE_MANAGER()
{ }

LINE_STATUS
CACHE_MANAGER::LINE_MANAGER::GetStatus(ADDRESS address) const
{
    map<ADDRESS, LINE_STATE>::const_iterator it = addr2status_.find(address);

    LINE_STATUS ret_status = S_INVALID;
    if (it != addr2status_.end())
    {
        // Found
        ret_status = it->second.GetStatus();
    }

    return ret_status;
}


LINE_STATUS
CACHE_MANAGER::LINE_MANAGER::GetStatus(UINT32 owner, ADDRESS address) const
{
    std::map<ADDRESS, LINE_STATE>::const_iterator it = addr2status_.find(address);

    LINE_STATUS ret_status = S_INVALID;
    if (it != addr2status_.end())
    {
        // Found
        ret_status = it->second.GetStatus(owner);
    }

    return ret_status;
}

void
CACHE_MANAGER::LINE_MANAGER::SetStatus(UINT32 owner, ADDRESS address, LINE_STATUS status)
{
    // Look for it. Implicitly create it if it doesn't exist
    LINE_STATE& state = addr2status_[address];
    state.SetStatus(owner, status);
    if (!state.IsValid())
    {
        // No cache has it in a valid state -> remove the entry
        addr2status_.erase(address);
    }    
}

CACHE_MANAGER::CACHE_MANAGER():
    clear_lines(false),
    activated(true)
{}

CACHE_MANAGER::~CACHE_MANAGER()
{}

CACHE_MANAGER&
CACHE_MANAGER::GetInstance()
{
    static CACHE_MANAGER the_manager;
    return the_manager;
}

void
CACHE_MANAGER::Register(std::string level)
{
    // Create new manager if it doesn't exist
    str2manager_[level];
}

CACHE_MANAGER::LINE_MANAGER *
CACHE_MANAGER::find_line_manager(std::string level)
{
    map<std::string, LINE_MANAGER>::const_iterator it = str2manager_.find(level);
    return (it != str2manager_.end()) ? ((CACHE_MANAGER::LINE_MANAGER *)&it->second) : NULL;
}

LINE_STATUS
CACHE_MANAGER::GetStatus(std::string level, UINT32 index, UINT64 tag)
{
    LINE_MANAGER *line_manager = find_line_manager( level );
    if ( line_manager != NULL )
    {
        return line_manager->GetStatus(std::pair<UINT32, UINT64>(index, tag));
    }
    else
    {
        return S_INVALID;
    }
}


LINE_STATUS
CACHE_MANAGER::GetStatus(std::string level, UINT32 owner, UINT32 index, UINT64 tag)
{
    LINE_MANAGER *line_manager = find_line_manager( level );
    if ( line_manager != NULL )
    {
        return line_manager->GetStatus(owner, std::pair<UINT32, UINT64>(index, tag));
    }
    else
    {
        return S_INVALID;
    }
}

void
CACHE_MANAGER::SetStatus(std::string level, UINT32 owner, UINT32 index, UINT64 tag, LINE_STATUS status)
{
    if(!activated) return;
    
    LINE_MANAGER *line_manager = find_line_manager( level );
    if ( line_manager != NULL )
    {
        line_manager->SetStatus(owner, std::pair<UINT32, UINT64>(index,tag), status);
    }
}

