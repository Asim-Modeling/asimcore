/**************************************************************************
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

/**
 * @file dralServerDefines.h
 * @author Pau Cabre 
 * @brief dral server defines
 */


#ifndef DRAL_SERVER_DEFINES_H
#define DRAL_SERVER_DEFINES_H

#define DRAL_SERVER_VERSION_MAJOR 4    /**< Interface version */
#define DRAL_SERVER_VERSION_MINOR 0    /**< Interface implementation version */

#include <iostream>
using namespace std;

#define DRAL_ASSERT(condition,msg) \
    if (! (condition)) { \
        cerr << "DRAL error in file " << __FILE__ " line " << __LINE__<< ": " \
        << __PRETTY_FUNCTION__ << ": " << msg << endl; \
        abort(); \
    }

#define DRAL_WARNING(msg) \
    cerr << "Warning: " << msg << endl;


#ifndef DRAL_ANY
#define DRAL_ANY         UINT32_MAX
#endif

#ifndef DRAL_ALL
#define DRAL_ALL         (UINT32_MAX-1)
#endif

#ifndef DRAL_EMPTY
#define DRAL_EMPTY       (UINT32_MAX-2)
#endif

#endif /* DRAL_SERVER_DEFINES_H */

