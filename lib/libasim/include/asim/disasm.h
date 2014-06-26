/*
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
 * @file
 * @author ??
 * @brief
 */

#ifndef __DISASM_H__
#define __DISASM_H__

// ASIM core
#include "asim/syntax.h"

/* Alpha opcode format */
#define RA(x) (((x)>>21)& 0x1f)
#define RB(x) (((x)>>16)& 0x1f)
#define RC(x) (((x)>>0) & 0x1f)
#define UDISP(x) ((x) & 0xffff)
#define DISP(x) ((((x) & 0xffff) ^ 0x8000)-0x8000)
#define BDISP(x) ((((x) & 0x1fffffu) ^ 0x100000u)-0x100000u)
#define OPCODE(x) (((x) >>26)&0x3f)
#define JUMP_OPTYPE(x) (((x)>>14) & 0x3)
#define JUMP_HINT(x) ((x) & 0x3fff)
#define JDISP(x) ((((x) & 0x3fff) ^ 0x2000)-0x2000)
#define OP_OPTYPE(x) (((x)>>5)&0x7f)
#define OP_IS_CONSTANT(x) ((x) & (1<<12))
#define LITERAL(x) (((x)>>13) & 0xff)
#define PAL_FUN(x) ((x) & 0x03ffffff)

/* Shapes

   Memory instruction format    oooo ooaa aaab bbbb dddd dddd dddd dddd
   Memory with function         oooo ooaa aaab bbbb ffff ffff ffff ffff
   Memory branch                oooo ooaa aaab bbbb BBhh hhhh hhhh hhhh
   Branch                       oooo ooaa aaad dddd dddd dddd dddd dddd
   Operate reg                  oooo ooaa aaab bbbb 0000 ffff fffc cccc
   Operate cont                 oooo ooaa aaal llll lll1 ffff fffc cccc
   FP reg                       oooo ooaa aaab bbbb 000f ffff fffc cccc
   Pal                          oooo oodd dddd dddd dddd dddd dddd dddd
   Reserved                     oooo oodd dddd dddd dddd dddd dddd dddd

   Key:
     o = opcode
     a = register a
     b = register b
     B = branch prediction
     c = register c
     d = displacement
     f = function code
     h = hint
     l = literal

*/

#define MEMORY_FORMAT_MASK              0xfc000000
#define MEMORY_FUNCTION_FORMAT_MASK     0xfc00ffff
#define MEMORY_BRANCH_FORMAT_MASK       0xfc00c000
#define BRANCH_FORMAT_MASK              0xfc000000
#define OPERATE_FORMAT_MASK             0xfc000fe0
#define FLOAT_FORMAT_MASK               0xfc00ffe0
#define RESERVED_FORMAT_MASK            0xfc000000
#define PAL_GENERIC_FORMAT_MASK         0xfc000000

#define MEMORY_FORMAT_CODE 1
#define MEMORY_FORMAT(op, name) \
 { (unsigned)op << 26, name, MEMORY_FORMAT_CODE }

#define MEMORY_FLOAT_FORMAT_CODE 2
#define MEMORY_FLOAT_FORMAT(op, name) \
 { (unsigned)op << 26, name, MEMORY_FLOAT_FORMAT_CODE }

#define MEMORY_BRANCH_FORMAT_CODE 3
#define MEMORY_BRANCH_FORMAT(op, func, name) \
{ ((unsigned)op<<26)+(func<<14),name, MEMORY_BRANCH_FORMAT_CODE }

#define MEMORY_FUNCTION_FORMAT_CODE 4
#define MEMORY_FORMAT_FUNCTION(op, func, name) \
 { ((unsigned)op<<26)+(func), name, MEMORY_FUNCTION_FORMAT_CODE }

#define BRANCH_FORMAT_CODE  5
#define BRANCH_FORMAT(op, name) \
 { ((unsigned)op<<26), name , BRANCH_FORMAT_CODE }

#define OPERATE_FORMAT_CODE 6
#define OPERATE_FORMAT(op, extra,name)  \
 {((unsigned)op<<26)+(extra<<5),name , OPERATE_FORMAT_CODE}

#define FLOAT_FORMAT_CODE 7
#define FLOAT_FORMAT(op, extra,name) \
{((unsigned)op<<26)+(extra<<5),name , FLOAT_FORMAT_CODE }

#define PAL_FORMAT_CODE 8
#define PAL_FORMAT(op, extra, name) \
{((unsigned)op<<26)+(extra),name, PAL_FORMAT_CODE}

#define PAL_GENERIC_FORMAT_CODE 9
#define PAL_GENERIC_FORMAT(op, name) \
{((unsigned)op<<26),name, PAL_GENERIC_FORMAT_CODE}

#define RESERVED_FORMAT_CODE 10
#define RESERVED_FORMAT(op, name) \
{((unsigned)op<<26),name, RESERVED_FORMAT_CODE}

#define FP_BRANCH_FORMAT_CODE  11
#define FP_BRANCH_FORMAT(op, name) \
 { ((unsigned)op<<26), name , FP_BRANCH_FORMAT_CODE }

typedef struct AlphaInstruction
{
    unsigned int i;
    const char *name;
    int type;

} alpha_insn;

extern const char *disassemble (UINT32 encoded, UINT64 pc);


#endif /* __DISASM_H__ */
