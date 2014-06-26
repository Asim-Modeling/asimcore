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

SUBDIRS = lib

#
# we have non-standard install sub-targets
#
.phony: install-exec-hook install-package install-src print-release-name

install-exec-hook: all
	@echo "Done."


#
# This horrible sequence copies just the modules and other files needed to build 
# models to $(codedir)
# It also removes the .svn subdirectories in the installed tree.
#
install-package:
	@rm -rf  $(DESTDIR)$(codedir)
	$(mkinstalldirs)  $(DESTDIR)$(codedir)
	rsync -av --exclude .svn admin   $(DESTDIR)$(codedir)
	rsync -av --exclude .svn modules $(DESTDIR)$(codedir)

#
# This horrible sequence copies the source to $(codedir) and
# tries to make the ownership and permissions reasonable
# It also removes the .svn subdirectories in the installed tree.
#
install-src:
	$(mkinstalldirs)  $(DESTDIR)$(codedir)
	rm -rf  $(DESTDIR)$(codedir)/*
	cp -r . $(DESTDIR)$(codedir)
	chgrp --recursive --reference $(datadir) $(DESTDIR)$(codedir)
	if [ `ls -ld $(datadir) | cut -b 6` = w ]; then chmod -R g+w $(DESTDIR)$(codedir); fi
	find    $(DESTDIR)$(codedir) -name .svn -depth -exec rm -rf {} \;

#
# Install the source tree, and make it world-readable
#
install-src-public: install-src
	find $(DESTDIR)$(codedir) -type d -exec chmod g+r+x {} \;
	chmod -R o+r $(DESTDIR)$(codedir)
	find         $(DESTDIR)$(codedir) -type d -exec chmod o+x {} \;


#
# we also need to fix up the distribution to contain the whole source tree
# minus the .svn directories
#
EXTRA_DIST = admin config include lib pm util Makefile changes

dist-hook:
	make -C $(distdir) distclean
	find $(distdir) -name .svn -depth -exec rm -rf {} \;

#
# Utility target to print the release being built
#
print-release-name:
	@echo ${release}
