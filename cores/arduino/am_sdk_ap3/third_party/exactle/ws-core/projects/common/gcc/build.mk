###################################################################################################
#
# Generic build make targets
# 
#         $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
#         $Revision: 10805 $
#  
# Copyright (c) 2013-2017 ARM Ltd., all rights reserved.
# ARM Ltd. confidential and proprietary.
#
# IMPORTANT.  Your use of this file is governed by a Software License Agreement
# ("Agreement") that must be accepted in order to download or otherwise receive a
# copy of this file.  You may not use or copy this file for any purpose other than
# as described in the Agreement.  If you do not agree to all of the terms of the
# Agreement do not use this file and delete all copies in your possession or control;
# if you do not have a copy of the Agreement, you must contact ARM Ltd. prior
# to any use, copying or further distribution of this software.
#
###################################################################################################

#--------------------------------------------------------------------------------------------------
#     Project
#--------------------------------------------------------------------------------------------------

# Parent makefile must export the following variables
#    CROSS_COMPILE
#    ROOT_DIR
#    LIBS
#    DEBUG
#    BIN
#    MAC_PRJ
#    STACK_PRJ
#    CPU
#    CFG_STACK
#    CFG_MAC

# Toolchain
CC         := $(CROSS_COMPILE)gcc
AR         := $(CROSS_COMPILE)ar
LD         := $(CROSS_COMPILE)gcc
DEP        := $(CROSS_COMPILE)gcc
OBJDUMP	   := $(CROSS_COMPILE)objdump
OBJCOPY	   := $(CROSS_COMPILE)objcopy
SIZE	   := $(CROSS_COMPILE)size

#--------------------------------------------------------------------------------------------------
#     Sources
#--------------------------------------------------------------------------------------------------

include sources*.mk

# Object file list
OBJ_FILES  := $(C_FILES:.c=.o)
OBJ_FILES  := $(subst $(ROOT_DIR)/,$(INT_DIR)/,$(OBJ_FILES))
OBJ_FILES  := $(subst ./,$(INT_DIR)/,$(OBJ_FILES))
DEP_FILES  := $(OBJ_FILES:.o=.d)

#--------------------------------------------------------------------------------------------------
#     Compilation flags
#--------------------------------------------------------------------------------------------------

# Hardware configuration
CFG_MAC    += 
CFG_DEV    += 

# Compiler flags
C_FLAGS    := -std=c99
C_FLAGS    += -Wall #-pedantic #-Werror
C_FLAGS    += -mcpu=$(CPU) -mthumb -mlittle-endian
C_FLAGS    += $(addprefix -I,$(INC_DIRS))
C_FLAGS    += $(addprefix -D,$(CFG_STACK))
C_FLAGS    += $(addprefix -D,$(CFG_MAC))
C_FLAGS    += $(addprefix -D,$(CFG_DEV))
C_FLAGS    += -DWSF_BUF_ALLOC_FAIL_ASSERT=FALSE
ifeq ($(DEBUG),1)
C_FLAGS    += -O0 -g
C_FLAGS    += -DWSF_ASSERT_ENABLED=TRUE
#C_FLAGS    += -DWSF_BUF_FREE_CHECK=TRUE
C_FLAGS    += -DWSF_BUF_STATS=TRUE
else
C_FLAGS    += -Os -g
endif

# Linker flags
LD_FLAGS   := -T$(ROOT_DIR)/projects/generic/board/lm3s6965.ld
LD_FLAGS   += -mthumb -mcpu=$(CPU)
LD_FLAGS   += -Wl,-Map=$(BIN:.elf=.map)
LD_FLAGS   += -Wl,--gc-sections

# Dependency flags
DEP_FLAGS  := $(C_FLAGS) -MM -MF

#--------------------------------------------------------------------------------------------------
#     Targets
#--------------------------------------------------------------------------------------------------

all: $(LIBS) $(BIN) gentokens
	@$(OBJCOPY) -O binary $(BIN) $(BIN:.elf=.bin)
	@$(OBJCOPY) -O ihex $(BIN) $(BIN:.elf=.hex)
	@$(OBJDUMP) -t $(BIN) > $(BIN:.elf=.sym)
	@echo "+++ Object summary: $(INT_DIR)"
	@-find $(INT_DIR) -name *.o | xargs $(SIZE) -t
	@echo "+++ Binary summary: $(BIN)"
	@-$(SIZE) $(BIN)

$(BIN): $(OBJ_FILES) $(LIBS)
	@echo "+++ Linking: $@"
	@mkdir -p $(BIN_DIR)
	@$(LD) -o $(BIN) $(LD_FLAGS) $(OBJ_FILES) $(LIBS)
	@$(OBJDUMP) -t $(BIN) > $(BIN:.elf=.sym)

$(STACK_LIB):
	@$(MAKE) -C $(STACK_PRJ) CROSS_COMPILE=$(CROSS_COMPILE) CPU=$(CPU) CFG_STACK="$(CFG_STACK)"

$(MAC_LIB):
	@$(MAKE) -C $(MAC_PRJ) CROSS_COMPILE=$(CROSS_COMPILE) CPU=$(CPU) CFG_MAC="$(CFG_MAC)"

$(INT_DIR)/%.o: $(ROOT_DIR)/%.c
	@echo "+++ Compiling: $<"
	@mkdir -p $(dir $@)
	@$(CC) $(C_FLAGS) -DMODULE_ID=$(call FILE_HASH,$<) -c -o $@ $<
	@$(if $(DEP),$(DEP) $(DEP_FLAGS) $(subst .o,.d,$@) -MP -MT $@ $<,)

ifeq ($(TOKEN),1)
gentokens: token
	@-$(if $(STACK_PRJ),$(MAKE) -C $(STACK_PRJ) CPU=$(CPU) token)
	@-$(if $(MAC_PRJ),  $(MAKE) -C $(MAC_PRJ)   CPU=$(CPU) token)
	@echo "+++ Generating token defintion file: $(TOKEN_FILE)"
	@-$(if $(STACK_LIB),cat $(STACK_LIB:.a=.tokens) >> $(TOKEN_FILE))
	@-$(if $(MAC_LIB),  cat $(MAC_LIB:.a=.tokens)   >> $(TOKEN_FILE))
else
gentokens:
endif

ifeq ($(TOKEN),1)
clean: token.clean
else
clean:
endif
	@-$(if $(STACK_PRJ),$(MAKE) -C $(STACK_PRJ) CPU=$(CPU) clean)
	@-$(if $(MAC_PRJ),  $(MAKE) -C $(MAC_PRJ)   CPU=$(CPU) clean)
	@rm -rf $(INT_DIR)
	@rm -rf $(BIN_DIR)

ifeq ($(TOKEN),1)
include $(ROOT_DIR)/ws-core/projects/common/gcc/token.mk
endif
-include $(DEP_FILES)

.PHONY: all $(LIBS) gentokens clean
