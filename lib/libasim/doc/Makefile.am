# reuse libcameroon Doxy config file but substitute the project name:
html-local: html/index.html
html/index.html:
	cd ..; sed -e 's/libcameroon/libasim/' ../libcameroon/doc/Doxyfile | $(DOXYGEN) -

install-html-local: html
	$(mkinstalldirs)      $(DESTDIR)$(htmldir)/libasim
	$(INSTALL_TREE) html/ $(DESTDIR)$(htmldir)/libasim

clean-local:
	$(RM) -rf html
