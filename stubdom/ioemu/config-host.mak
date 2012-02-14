CONFIG_STUBDOM=yes
prefix=/usr
bindir=${prefix}/bin
mandir=$(MANDIR)
datadir=$(SHAREDIR)/qemu
docdir=$(SHAREDIR)/doc/qemu
MAKE=make
INSTALL=install
CC=gcc
HOST_CC=gcc
AR=ar
STRIP=strip -s -R .comment -R .note
OS_CFLAGS=
OS_LDFLAGS=
ARCH_CFLAGS=-m32
ARCH_LDFLAGS=-m32
EXESUF=
AIOLIBS=
ARCH=i386
CONFIG_GDBSTUB=
CONFIG_OSS=
CONFIG_OSS_LIB=
VERSION=0.10.2
SRC_PATH=/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../tools/ioemu-qemu-xen
TARGET_DIRS=i386-stubdom
BUILD_DOCS=
CONFIG_AIO=
TOOLS=
QEMU_ROOT ?= /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/ioemu
XEN_ROOT=/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/..
include $(XEN_ROOT)/tools/Rules.mk
SUBDIR_RULES=subdir-$(TARGET_DIRS)
subdir-$(TARGET_DIRS): libqemu_common.a
include $(QEMU_ROOT)/xen-hooks.mak
HOST_USB=stub

HOST_LDFLAGS= $(LDFLAGS) -g -Wl,--warn-common
HOST_CFLAGS= $(CFLAGS)
CPPFLAGS += -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include -D__MINIOS__ -DHAVE_LIBC -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/posix -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../tools/xenstore  -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/x86 -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/x86/x86_64 -U __linux__ -U __FreeBSD__ -U __sun__ -nostdinc -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/posix -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/cross-root-x86_64/x86_64-xen-elf/include -isystem /usr/lib/gcc/x86_64-linux-gnu/4.6/include -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/lwip-x86_64/src/include -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/lwip-x86_64/src/include/ipv4 -I/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/include -I/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../xen/include -DCONFIG_STUBDOM -MF .$(@F).d 
CFLAGS= -mno-red-zone -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls  -m64 -mno-red-zone -fno-reorder-blocks -fno-asynchronous-unwind-tables -m64 -g -fno-strict-aliasing -std=gnu99 -Wall -Wstrict-prototypes -Wno-unused-value -Wdeclaration-after-statement -Wno-unused-but-set-variable  -fno-stack-protector -fno-exceptions
LDFLAGS= -nostdlib -L/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/cross-root-x86_64/x86_64-xen-elf/lib

SUBDIR_RULES=subdir-$(TARGET_DIRS)
subdir-$(TARGET_DIRS): libqemu_common.a
include $(QEMU_ROOT)/xen-hooks.mak

