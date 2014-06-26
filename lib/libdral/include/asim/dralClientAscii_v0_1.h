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

#ifndef DRAL_CLIENT_ASCII_V0_1_H
#define DRAL_CLIENT_ASCII_V0_1_H

#include <string.h>
using namespace std;

#include "asim/dralClientImplementation.h"

#define DRAL_NO_FLAGS       0 /* 0000b */
#define DRAL_FLG_IMMUTABLE  1 /* 0001b */
#define DRAL_FLG_PAST       2 /* 0010b */
#define DRAL_FLG_CURRENT    4 /* 0100b */
#define DRAL_FLG_FUTURE     8 /* 1000b */

/**
 * ASCII client specific implementation class
 * It works with all the version numbers, since the ascii format has not
 * changed
 */
class DRAL_CLIENT_ASCII_IMPLEMENTATION_CLASS
    : public DRAL_CLIENT_IMPLEMENTATION_CLASS
{
  public:

    DRAL_CLIENT_ASCII_IMPLEMENTATION_CLASS (
        DRAL_BUFFERED_READ dral_read, DRAL_LISTENER dralListener);

    UINT16 ProcessNextEvent (bool blocking, UINT16 num_events);

  private:

    /**
     * Private method used to fill the buffer with a line read
     * from the file descriptor
     * All the lines with a command have to end with a '\n' character
     * It returns false if any error found
     */
    bool Get_line (char ** buffer, bool * end_of_file);

    /**
     * Private method used to convert the time_span string
     * format from the string format to the unsigned byte format
     */
    unsigned char TimeSpanStrToByte(const char * s);


    /**
     * Private methods to proccess theses specific events
     */
    void SetTagSingleValue (istringstream * s);
    void SetTagString (istringstream * s);
    void SetTagSet (istringstream * s, char * b);
    void MoveItems (istringstream * s, char * b);
    void SetCapacity (istringstream * s, char * b);



    /**
     * This private method  counts how many times the character c is
     * found in the string.
     * It is used to know how many elements are in a command
     */
    UINT16 HowManyElements(char * buffer, char c);

    bool firstCycle;

};



#endif /* DRAL_CLIENT_ASCII_V0_1_H */
