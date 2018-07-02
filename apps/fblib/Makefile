BUILDROOT_BASE_DIR ?= ../../patched/buildroot/
TOOLCHAIN_PATH ?= $(BUILDROOT_BASE_DIR)/buildroot-20060918/build_mips_nofpu_onephone/staging_dir/mips32/bin/

CC = $(TOOLCHAIN_PATH)/mips-linux-gcc -std=c99
#CC=gcc -Wall -pedantic -std=c99 -O3

fbgrad: fbgrad.c fblib.h
	${CC} $< -o $@
