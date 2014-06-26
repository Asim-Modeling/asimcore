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

/*
 * *****************************************************************
 * *                                                               *

/**
 * @file
 * @author ??
 * @brief
 */

// (r2r) what the heck is that?

#define TRAP_RESET	0x0001  /* Immediate */
#define TRAP_ITB_ACV	0x0081  /* PreMap */
#define TRAP_INTERRUPT	0x0101  /* Immediate */
#define TRAP_DTB_MISS	0x0201  /* Exec - basic D-tb miss */
#define TRAP_DDTB_MISS	0x0281  /* Exec - nestetd D-tb miss */
#define TRAP_UNALIGN	0x0301  /* Retire */
#define TRAP_DFAULT	0x0381  /* Retire */
#define TRAP_ITB_MISS	0x0181  /* PreMap - I-tb miss */
#define TRAP_OPCDEC	0x0481  /* Retire */
#define TRAP_MCHK	0x0401  /* Immediate */
#define TRAP_ARITH	0x0501  /* Retire */
#define TRAP_FEN	0x0581  /* Retire */
#define TRAP_PAL	0x2001  /* PreMap */

#define TRAP_DTB_ACV    TRAP_DTB_FAULT
