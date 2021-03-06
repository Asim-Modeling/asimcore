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
 **************************************************************************/

#ifndef _PTLIB__H_
#define _PTLIB__H_

#if COMPILE_DRAL_WITH_PTV

#include <stdint.h>
#include <limits.h>

// for var args
#include <stdarg.h>

enum Pipe_Color {
        Pipe_No_Color,
        Pipe_Dark_Blue,
        Pipe_Blue,
        Pipe_Cyan,
        Pipe_Dark_Green,
        Pipe_Green,
        Pipe_Yellow,
        Pipe_Orange,
        Pipe_Red,
        Pipe_Pink,
        Pipe_Magenta,
        Pipe_Purple,
        Pipe_Brown,
        Pipe_Gray,
        Pipe_Light_Gray,
        Pipe_White,
        Pipe_Black
};

typedef enum Pipe_Color Pipe_Color;

enum Base_Data_Type {
        Pipe_Invalid_Data=0,
        Pipe_Boolean,
        Pipe_Integer,
        Pipe_Hex_Integer,
        Pipe_String,
        Pipe_Association,
        Pipe_Enumeration,
        Pipe_ByteArray,
        Pipe_Hex_Integer64,
        Pipe_Max_Data_Types
};
typedef enum Base_Data_Type Base_Data_Type;
struct Pipe_Record;
struct Pipe_Event;
struct Pipe_Data;
typedef uint32_t Pipe_RecId;
typedef class Pipe_Recordtype Pipe_Recordtype;
typedef class Pipe_Datatype Pipe_Datatype;
typedef class Pipe_Eventtype Pipe_Eventtype;
typedef struct Pipe_Cookie Pipe_Cookie;
#define PIPE_PDL_MIN 0x0
#define PIPE_PDL_MAX 0xff 
#define NO_TRACE_FILE  0
#define TRACE_FILE_DEFAULT 1
#define PIPE_NO_DATATYPE	((Pipe_Datatype *) -3)
#define PIPE_NO_RECORD		((Pipe_RecId) 0)
#define PIPE_NO_THREADID	(-1)
typedef enum {
        PT_Binary,
        PT_ASCII,
        PT_Unknown
} PT_Format;
extern void pipe_init(uint32_t min_records);
extern void pipe_enable_collection();
extern void pipe_open_file(const char *filename, 
                           PT_Format format,
                           Pipe_RecId first, 
                           int pdl = PIPE_PDL_MAX,
			   int * pipetrace_id = 0);
extern void pipe_close_file(Pipe_RecId last, int pTraceId = TRACE_FILE_DEFAULT );
extern Pipe_Recordtype *pipe_new_recordtype(const char *name, const char *desc);
extern Pipe_Datatype *pipe_new_datatype(const char *name, Base_Data_Type bdt,
					const char *desc, int len = 0, char **strs = 0);
extern Pipe_Eventtype *pipe_new_eventtype(const char *name, int letter,
					  Pipe_Color color, const char *desc,  uint32_t pdl = 0xff, uint32_t options = 0, char knob[64] = "");
extern Pipe_RecId pipe_open_record_inst(Pipe_Recordtype *rt, int32_t thread_id, Pipe_RecId parent, int pTraceId = TRACE_FILE_DEFAULT, const char *filename = 0, int linenum = 0 );
extern Pipe_RecId pipe_reference_record_inst(Pipe_RecId recid, const char *filename = 0, int linenum = 0);
extern void pipe_close_record_inst(Pipe_RecId recid, const char *filename = 0, int linenum = 0);
extern void pipe_close_record_inst_completely(Pipe_RecId recid, const char *filename = 0, int linenum = 0);

//extern void pipe_record_data(Pipe_RecId recid, Pipe_Datatype *dt, ...);
extern void pipe_record_data_va(Pipe_RecId recid, Pipe_Datatype *dt, va_list* ap);
extern void pipe_record_event(Pipe_RecId recid, Pipe_Eventtype *et,
			      uint32_t duration, Pipe_Datatype *dt = PIPE_NO_DATATYPE, ...);
extern void pipe_record_event(Pipe_RecId recid, Pipe_Eventtype *et,
			      uint32_t duration, Pipe_Datatype *dt, va_list* ap);
extern Pipe_Cookie *pipe_record_event_begin(Pipe_RecId recid, Pipe_Eventtype *et);
extern void pipe_record_event_end(Pipe_Cookie *cookie);
extern void pipe_comment(char *comment, char newline_delimiter = 0);

// Please only use this next one if ABSOLUTELY NEED to specify a different time
//  than the current thread_cycle.  The time cannot be before the opening time
//  of the record it is posted to.
//extern void pipe_record_event_time(Pipe_RecId recid, Pipe_Eventtype *et,
//				   uint32_t time, uint32_t duration,
//				   Pipe_Datatype *dt = PIPE_NO_DATATYPE, ...);
extern void pipe_record_event_time(Pipe_RecId recid, Pipe_Eventtype *et,
				   uint32_t time, uint32_t duration,
				   Pipe_Datatype *dt, va_list* ap);
extern bool pipe_record_exists(Pipe_RecId recid);
extern void pipe_threads_per_cores_per_procs(int nt, int nc, int np);
extern void pipe_threads_per_proc(int n);
extern void pipe_threads_total(int n);

extern void thread_initialize(int init_size = 0);
extern void pipe_disable_implicit_stats();
extern void pipe_dump_buffer(const char *filename, uint32_t max_records, PT_Format format);
extern void pipe_set_will_dump();


#endif

#endif
