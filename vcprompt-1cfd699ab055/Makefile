CC = gcc -std=gnu99
CPPFLAGS = 
CFLAGS = -g -O2 -Wall -Wextra -Wno-unused-parameter
LDFLAGS = 
LIBS = -lsqlite3 

headers = $(wildcard src/*.h) config.h
sources = $(wildcard src/*.c)
objects = $(subst .c,.o,$(sources))

vcprompt: $(objects) Makefile
	$(CC) $(LDFLAGS) -o $@ $(objects) $(LIBS)

Makefile: Makefile.in configure
	./configure -C

config.h: config.h.in
	rm -f $@
	./configure -C

configure: configure.ac
	autoconf

# build a standalone version of capture_child() library for testing
src/capture: src/capture.c src/capture.h src/common.c src/common.h
	$(CC) -DTEST_CAPTURE $(CFLAGS) -o $@ src/capture.c src/common.c

# Maximally pessimistic view of header dependencies.
$(objects): $(headers) Makefile

.PHONY: check check-simple check-hg check-git check-svn check-fossil grind
check: check-simple check-hg check-git check-svn check-fossil

hgrepo = tests/hg-repo.tar
gitrepo = tests/git-repo.tar
svnrepos = tests/svn-repo-1.tar tests/svn-repo-2.tar
fossilrepo = tests/fossil-repo

check-simple: vcprompt
	cd tests && ./test-simple

check-hg: vcprompt $(hgrepo)
	cd tests && ./test-hg

$(hgrepo): tests/setup-hg
	cd tests && ./setup-hg

check-git: vcprompt $(gitrepo)
	cd tests && ./test-git

$(gitrepo): tests/setup-git
	cd tests && ./setup-git

check-svn: vcprompt $(svnrepos)
	cd tests && ./test-svn

$(svnrepos): tests/setup-svn
	cd tests && ./setup-svn

check-fossil: vcprompt $(fossilrepo)
	cd tests && ./test-fossil

$(fossilrepo): tests/setup-fossil
	cd tests && ./setup-fossil

grind:
	make check VCPVALGRIND=y

clean:
	rm -f $(objects) vcprompt $(hgrepo) $(gitrepo) $(fossilrepo)

DESTDIR =
PREFIX = /usr/local
BINDIR = $(DESTDIR)$(PREFIX)/bin
MANDIR = $(DESTDIR)$(PREFIX)/man/man1

.PHONY: install
install: vcprompt
	install -d $(BINDIR) $(MANDIR)
	install vcprompt $(BINDIR)
	install vcprompt.1 $(MANDIR)

.PHONY: dist
dist: configure
	[ "$$ver" ] || (echo "\$$ver not set" >&2; exit 1)
	hg archive -r $$ver -t files -X ".hg*" dist/vcprompt-$$ver
	(cd dist/vcprompt-$$ver && autoconf && rm -rf autom4te.cache)
	(cd dist && tar -czf vcprompt-$$ver.tar.gz vcprompt-$$ver)
	rm -rf dist/vcprompt-$$ver
