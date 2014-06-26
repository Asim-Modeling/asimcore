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

bin_PROGRAMS= dbtest

dbtest_LDFLAGS = $(QTLIBDIR) $(QTLIBOBJ) -L../../../../lib/libdral -L../../../../lib/libdraldb -ldraldb -ldral -lm 

dbtest_SOURCES =   dbtest.cpp 
 
WARNFLAGS = -ansi -pedantic -W -Wall -Wcast-align -Wformat=2 -Wconversion -Winline -Wno-unused -Wno-parentheses -Wno-long-long
AM_CXXFLAGS += $(QTINCDIR) -I../../../../lib/libdral/include -I../../../../lib/libdraldb/include


# maybe explicitly link this, avoiding use of broken libtool:
if X86_64_LIBTOOL_HACK
dbtest: $(dbtest_OBJECTS)
	$(CXXLD) -o dbtest $(dbtest_OBJECTS) $(dbtest_LDFLAGS)
endif
