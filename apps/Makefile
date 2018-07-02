
BUILDROOT_BASE_DIR ?= ../patched/buildroot/
TOOLCHAIN_PATH ?= $(BUILDROOT_BASE_DIR)/buildroot-20060918/build_mips_nofpu_onephone/staging_dir/mips32/bin/

INC_DIR := $(BUILDROOT_BASE_DIR)/buildroot-20060918/build_mips_nofpu_onephone/staging_dir/mips32/include
CFLAGS += -I $(INC_DIR)

CC = $(TOOLCHAIN_PATH)/mips-linux-gcc

hello:
