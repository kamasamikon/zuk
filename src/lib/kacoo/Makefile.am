moddir = $(prefix)/zuk/lib

mod_LTLIBRARIES = libkacoo.la
libkacoo_la_SOURCES = \
					 lib/base64/base64.c lib/hget/hget.c \
					 lib/md5sum/md5sum.c lib/xmldoc/xmldoc.c lib/ziplib/gziplib.c \
					 src/kdbg.c src/kim.c src/kini.c src/kmem.c src/ksal.c \
					 src/kstr.c src/ksyn.c src/ktmr.c src/ktsk.c src/sdlist.c

libkacoo_la_CFLAGS = -Wall -g
libkacoo_la_CFLAGS += -I./inc -I./lib/base64 -I./lib/hget -I./lib/inc -I./lib/md5sum -I./lib/xmldoc -I./lib/ziplib
libkacoo_la_CFLAGS += -D__UNIX__
