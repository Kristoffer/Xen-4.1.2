# Copyright (c) 1998 Cygnus Support
#
# The authors hereby grant permission to use, copy, modify, distribute,
# and license this software and its documentation for any purpose, provided
# that existing copyright notices are retained in all copies and that this
# notice is included verbatim in any distributions. No written agreement,
# license, or royalty fee is required for any of the authorized uses.
# Modifications to this software may be copyrighted by their authors
# and need not follow the licensing terms described here, provided that
# the new terms are clearly indicated on the first page of each file where
# they apply.

DESTDIR =
VPATH = ../../../../newlib-1.16.0/libgloss/libnosys
srcdir = ../../../../newlib-1.16.0/libgloss/libnosys
objdir = .
srcroot = $(srcdir)/../..
objroot = $(objdir)/../..

prefix = /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/cross-root-i686
exec_prefix = ${prefix}

host_alias = i686-xen-elf
target_alias = i686-xen-elf
program_transform_name = s,^,i686-xen-elf-,

bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
tooldir = $(exec_prefix)/$(target_alias)

# Multilib support variables.
# TOP is used instead of MULTI{BUILD,SRC}TOP.
MULTIDIRS = 
MULTISUBDIR = 
MULTIDO = true
MULTICLEAN = true

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = /usr/bin/install -c
INSTALL_DATA = /usr/bin/install -c -m 644

SHELL =	/bin/sh

CC = gcc -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include -D__MINIOS__ -DHAVE_LIBC -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/posix -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../tools/xenstore  -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/x86 -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/x86/x86_32 -U __linux__ -U __FreeBSD__ -U __sun__ -nostdinc -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/posix -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/cross-root-i686/i686-xen-elf/include -isystem /usr/lib/gcc/x86_64-linux-gnu/4.6/include -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/lwip-x86_32/src/include -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/lwip-x86_32/src/include/ipv4 -I/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/include -I/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../xen/include  -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls  -m32 -march=i686 -m32 -march=i686 -g -fno-strict-aliasing -std=gnu99 -Wall -Wstrict-prototypes -Wno-unused-value -Wdeclaration-after-statement -Wno-unused-but-set-variable  -fno-stack-protector -fno-exceptions -D_I386MACH_ALLOW_HW_INTERRUPTS -B/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/newlib-x86_32/i686-xen-elf/newlib/ -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/newlib-x86_32/i686-xen-elf/newlib/targ-include -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/newlib-1.16.0/newlib/libc/include -B/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/newlib-x86_32/i686-xen-elf/libgloss/i386 -L/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/newlib-x86_32/i686-xen-elf/libgloss/libnosys -L/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/newlib-1.16.0/libgloss/i386

#AS = i686-xen-elf-as
AS = `if [ -f ${objroot}/../gas/as-new ] ; \
	then echo ${objroot}/../gas/as-new ; \
	else echo as ; fi`

AR = ar

#LD = ld
LD = `if [ -f ${objroot}/../ld/ld-new ] ; \
	then echo ${objroot}/../ld/ld-new ; \
	else echo ld ; fi`

RANLIB = ranlib

OBJDUMP = `if [ -f ${objroot}/../binutils/objdump ] ; \
	then echo ${objroot}/../binutils/objdump ; \
	else t='$(program_transform_name)'; echo objdump | sed -e $$t ; fi`
OBJCOPY = `if [ -f ${objroot}/../binutils/objcopy ] ; \
	then echo ${objroot}/../binutils/objcopy ; \
	else t='$(program_transform_name)'; echo objcopy | sed -e $$t ; fi`

# object files needed
OBJS = chown.o close.o environ.o errno.o execve.o fork.o fstat.o \
	getpid.o gettod.o isatty.o kill.o link.o lseek.o open.o \
	read.o readlink.o sbrk.o stat.o symlink.o times.o unlink.o \
	wait.o write.o _exit.o

# Object files specific to particular targets.
EVALOBJS = ${OBJS}

GCC_LDFLAGS = `if [ -d ${objroot}/../gcc ] ; \
	then echo -L${objroot}/../gcc ; fi`

OUTPUTS = libnosys.a

NEWLIB_CFLAGS = `if [ -d ${objroot}/newlib ]; then echo -I${objroot}/newlib/targ-include -I${srcroot}/newlib/libc/include; fi`
NEWLIB_LDFLAGS = `if [ -d ${objroot}/newlib ]; then echo -B${objroot}/newlib/ -L${objroot}/newlib/; fi`

INCLUDES = -I. -I$(srcdir)/..
# Note that when building the library, ${MULTILIB} is not the way multilib
# options are passed; they're passed in $(CFLAGS).
CFLAGS_FOR_TARGET = ${MULTILIB} ${INCLUDES} ${NEWLIB_CFLAGS}
LDFLAGS_FOR_TARGET = ${MULTILIB} ${NEWLIB_LDFLAGS}
AR_FLAGS = qc

.c.o:
	$(CC) $(CFLAGS_FOR_TARGET) -O2 $(INCLUDES) -c $(CFLAGS) $<

.C.o:
	$(CC) $(CFLAGS_FOR_TARGET) -O2 $(INCLUDES) -c $(CFLAGS) $<
.s.o:
	$(AS) $(ASFLAGS_FOR_TARGET) $(INCLUDES) $(ASFLAGS) -o $*.o $<

#
# GCC knows to run the preprocessor on .S files before it assembles them.
#
.S.o:
	$(CC) $(CFLAGS_FOR_TARGET) $(INCLUDES) $(CFLAGS) -c $<

#
# this is a bogus target that'll produce an assembler from the
# C source with the right compiler options. this is so we can
# track down code generation or debug symbol bugs.
#
.c.s:
	$(CC) $(CFLAGS_FOR_TARGET) -S $(INCLUDES) $(CFLAGS) $<

all: ${OUTPUTS}

#
# here's where we build the library for each target
#

libnosys.a: $(EVALOBJS)
	${AR} ${ARFLAGS} $@ $(EVALOBJS)
	${RANLIB} $@

doc:	

clean mostlyclean:
	rm -f $(OUTPUTS) *.i *~ *.o *-test *.srec *.dis *.map *.x

distclean maintainer-clean realclean: clean
	rm -f Makefile config.status $(OUTPUTS)

.PHONY: install info install-info clean-info
install:
	@for outputs in ${OUTPUTS}; do\
	 mkdir -p $(DESTDIR)$(tooldir)/lib${MULTISUBDIR}; \
	 $(INSTALL_PROGRAM) $${outputs} $(DESTDIR)$(tooldir)/lib${MULTISUBDIR}; \
	done

info:
install-info:
clean-info:

Makefile: Makefile.in config.status ../../../../newlib-1.16.0/libgloss/libnosys/../config/default.mh
	$(SHELL) config.status

config.status: configure
	$(SHELL) config.status --recheck
