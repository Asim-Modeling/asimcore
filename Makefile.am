#
# Copyright (C) 2002-2006 Intel Corporation
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# 
#

SUBDIRS = lib

#
# we have non-standard install sub-targets
#
.phony: install-exec-hook install-package install-src

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
	rsync -av --exclude .svn feeders $(DESTDIR)$(codedir)
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
	make -C $(DESTDIR)$(codedir) clean
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
EXTRA_DIST = admin config include lib pm feeders util Makefile changes

dist-hook:
	make -C $(distdir) distclean
	find $(distdir) -name .svn -depth -exec rm -rf {} \;

