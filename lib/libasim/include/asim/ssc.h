/****************************************************************************
 *
 * @brief Header file for SSC instruction marker support
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

#ifndef __SSC_H__
#define __SSC_H__

template<class MACRO_INST>
class SSC {
    int     state;
    UINT32  marker_id;
    bool    marker_valid;

  public:
    SSC() { clear(); }

    void clear() { state=0; marker_valid=false; }
    void reset() { clear(); }

    bool CollectMacroInst(MACRO_INST macro, UINT32 &marker) {
        const UINT8 *bytes = macro->GetRawInstruction();
        UINT32 length = macro->GetLength();

        bool re_check = false;
        do {
            switch( state ) {
              case 0:
                //  No instructions matched yet, look for MOV inst
                marker_valid = false;
                if (length == 5) {
                    if (bytes[0] == 0xbb) {
                        marker_id = *(UINT32 *)(&bytes[1]);
                        state = 1;
                    }
                }

                re_check = false;

                break;
                
              case 1:
                //  First instruction matched
                
                //  Next "instruction" must be a 3-byte sequence
                //
                //  If we don't match, we have to check to see if this is the start of a marker,
                //  so we reset to state zero and test again.
                
                if (length == 3) {
                    if ((bytes[0] == 0x64) &&
                        (bytes[1] == 0x67) &&
                        (bytes[2] == 0x90))
                    {
                        marker = marker_id;
                        return true;
                    }
                }

                //  If we didn't match we reach this point,
                //  which means that we've failed the check. However, The
                //  current instruction may be the start of a new sequence
                //  so we need to re-check...
                state = 0;

                if (length==5)
                    re_check = true;
                
                break;
                
              default:
                break;
            }
        } while (re_check);

        return false;
    }
};


#endif  //  #ifdef __SSC_H__
