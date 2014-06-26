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

lib_LIBRARIES = libdral.a

libdral_a_SOURCES = \
	src/dralServer.cpp \
	src/dralServerBinary.cpp \
	src/dralServerAscii.cpp \
	src/dralServerImplementation.cpp \
	src/dralStorage.cpp \
	src/dralClientBinary_v0.cpp \
	src/dralClientBinary_v1.cpp \
	src/dralClientBinary_v2.cpp \
	src/dralClientBinary_v3.cpp \
	src/dralClientBinary_v4.cpp \
	src/dralClientAscii_v0_1.cpp \
	src/dralClient.cpp \
	src/dralClientImplementation.cpp \
	src/dralStringMapping.cpp \
	src/dralListenerConverter.cpp \
	src/dralWrite.cpp \
	src/dralRead.cpp \
	src/dralInterface.cpp \
	src/dralDesc.cpp \
	src/dralTar.cpp

src/dralTar.cpp : tar_files/tarFile.tar scripts/makeDralTar.pl
	perl scripts/makeDralTar.pl tar_files/tarFile.tar > src/dralTar.cpp

# Inside ASIM we reuse AGT definitions (UINT64 and so on)
WARNFLAGS = -pedantic -W -Wall -Wcast-align -Wno-unused -Wno-parentheses -Wno-long-long
AM_CXXFLAGS += ${STLPORTINC} -I./include 
# pkgconfig configuration file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/libdral.pc

# pkgconfig uninstalled configuration file
pkgconfig_uninstalled_data=libdral-uninstalled.pc
pkgconfig_uninstalled_dir=${top_srcdir}/pkgconfig
pkgconfig_uninstalled=${pkgconfig_uninstalled_dir}/${pkgconfig_uninstalled_data}
all-local : ${pkgconfig_uninstalled}
${pkgconfig_uninstalled} : pkgconfig/${pkgconfig_uninstalled_data} ${lib_LIBRARIES}
	${mkinstalldirs} ${pkgconfig_uninstalled_dir}
	${INSTALL_DATA} pkgconfig/${pkgconfig_uninstalled_data} ${pkgconfig_uninstalled}

CLEANFILES=${pkgconfig_uninstalled}

DIST_SUBDIRS=include
SUBDIRS=include
