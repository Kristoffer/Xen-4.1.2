QEMU_ROOT ?= ..
XEN_ROOT ?= $(QEMU_ROOT)/../xen-unstable.hg

include ../config-host.mak

TARGET_ARCH=i386
TARGET_PATH:=$(SRC_PATH)/$(TARGET_DIRS)
CONFIG_SOFTMMU=yes

TARGET_CPPFLAGS += -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include -D__MINIOS__ -DHAVE_LIBC -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/posix -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../tools/xenstore  -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/x86 -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/x86/x86_64 -U __linux__ -U __FreeBSD__ -U __sun__ -nostdinc -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../extras/mini-os/include/posix -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/cross-root-x86_64/x86_64-xen-elf/include -isystem /usr/lib/gcc/x86_64-linux-gnu/4.6/include -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/lwip-x86_64/src/include -isystem /usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/lwip-x86_64/src/include/ipv4 -I/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/include -I/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/../xen/include -DCONFIG_STUBDOM -D__XEN_TOOLS__
TARGET_CFLAGS= -mno-red-zone -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls  -m64 -mno-red-zone -fno-reorder-blocks -fno-asynchronous-unwind-tables -m64 -g -fno-strict-aliasing -std=gnu99 -Wall -Wstrict-prototypes -Wno-unused-value -Wdeclaration-after-statement -Wno-unused-but-set-variable  -fno-stack-protector -fno-exceptions
TARGET_LDFLAGS= -nostdlib -L/usr/src/xen_build_from_xen_source/xen-4.1.2/stubdom/cross-root-x86_64/x86_64-xen-elf/lib

bindir = ${LIBEXEC}
