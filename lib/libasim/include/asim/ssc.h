/****************************************************************************
 *
 * @brief Header file for SSC instruction marker support
 *
 *Copyright (C) 2003-2006 Intel Corporation
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
