#
# Copyright (c) 2014, Intel Corporation
#
# All rights reserved.
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are 
# met:
# 
# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright 
#   notice, this list of conditions and the following disclaimer in the 
#   documentation and/or other materials provided with the distribution.
# - Neither the name of the Intel Corporation nor the names of its 
#   contributors may be used to endorse or promote products derived from 
#   this software without specific prior written permission.
#  
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

lib_LIBRARIES = libasim.a

libasim_a_SOURCES =	src/mesg.cpp \
			src/profile.cpp \
			src/stripchart.cpp \
			src/utils.cpp \
			src/event.cpp \
			src/disasm.cpp \
			src/module.cpp \
			src/atoi.cpp \
			src/xmlout.cpp \
			src/registry.cpp \
			src/thread.cpp \
			src/xcheck.cpp \
			src/except.cpp \
			src/stackdump.cpp \
			src/trace.cpp \
			src/trace_legacy.cpp \
			src/ioformat.cpp \
			src/port.cpp \
			src/stateout.cpp \
			src/trackmem.cpp \
			src/arch_register.cpp \
			src/clockserver.cpp \
			src/clockserver_lookahead_param.cpp \
			src/clockserver_threaded_lockfree.cpp \
			src/clockable.cpp \
			src/atomic.cpp \
			src/smp.cpp \
			src/regexobj.cpp \
                        src/cache_dyn.cpp \
			src/cache_manager.cpp \
			src/cache_manager_smp.cpp \
			src/plru_masks.cpp 

# Inside ASIM we reuse AGT definitions (UINT64 and so on)
WARNFLAGS = -Wall -W -Wpointer-arith -Wredundant-decls -Wcast-align \
            -Wno-long-long -Wno-unused -Wno-unknown-pragmas

AM_CXXFLAGS += ${STLPORTINC} -I./include -I../libdral/include ${XML_CFLAGS} \
               -DASIM_ENABLE_EVENTS -DASIM_ENABLE_TRACE -DCLOCKSERVER_IN_LIBASIM @asim_pthreads@

AM_LDFLAGS += ${XML_LFLAGS}

# pkgconfig configuration file
pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = pkgconfig/libasim.pc

# pkgconfig uninstalled configuration file
pkgconfig_uninstalled_data=libasim-uninstalled.pc
pkgconfig_uninstalled_dir=${top_srcdir}/pkgconfig
pkgconfig_uninstalled=${pkgconfig_uninstalled_dir}/${pkgconfig_uninstalled_data}
all-local : ${pkgconfig_uninstalled}
${pkgconfig_uninstalled} : pkgconfig/${pkgconfig_uninstalled_data} ${lib_LIBRARIES}
	${mkinstalldirs} ${pkgconfig_uninstalled_dir}
	${INSTALL_DATA} pkgconfig/${pkgconfig_uninstalled_data} ${pkgconfig_uninstalled}

CLEANFILES=${pkgconfig_uninstalled}

DIST_SUBDIRS=include
SUBDIRS=include
