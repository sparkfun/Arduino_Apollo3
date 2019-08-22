###################################################################################################
#
# Token generation make targets
#
#         $Date: 2017-02-23 18:57:06 -0600 (Thu, 23 Feb 2017) $
#         $Revision: 11266 $
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
#    CC
#    ROOT_DIR
#    BSP_DIR
#    BIN
#    C_FILES
#    C_FLAGS

# Toolchain
PYTHON     := python
TOK_TO_HDR := $(PYTHON) $(ROOT_DIR)/ws-core/projects/common/token2header.py

# Output
TOK_DIR    := tok
TOKEN_DEF  := $(BIN:.elf=.tokens)
TOKEN_HDR  := $(BIN:.elf=_tokens.h)

#-------------------------------------------------------------------------------
#     Scripts
#-------------------------------------------------------------------------------

ifeq ($(TOKEN),1)
HASH_SCR   := import sys, binascii; \
              print hex(binascii.crc32(sys.argv[1]) & 0xFFFF)
FILE_HASH   = $(shell $(PYTHON) -c "$(HASH_SCR)" $(notdir $(1)))
else
FILE_HASH   =
endif

#--------------------------------------------------------------------------------------------------
#     Objects
#--------------------------------------------------------------------------------------------------

ifeq ($(TOKEN),1)
TOK_FILES  := $(C_FILES:.c=.pp)
TOK_FILES  := $(subst $(ROOT_DIR)/,$(TOK_DIR)/,$(TOK_FILES))
ifneq ($(BSP_DIR),)
TOK_FILES  := $(subst $(BSP_DIR)/,$(TOK_DIR)/,$(TOK_FILES))
endif
endif

#--------------------------------------------------------------------------------------------------
#     Compilation flags
#--------------------------------------------------------------------------------------------------

ifeq ($(TOKEN),1)
C_FLAGS    += -DWSF_TOKEN_ENABLED=TRUE
endif

#--------------------------------------------------------------------------------------------------
#     Targets
#--------------------------------------------------------------------------------------------------

test:
	@echo $(TOK_FILES)

token: $(TOK_FILES)
	@rm -f $(TOKEN_DEF)
	@mkdir -p $(dir $(TOKEN_DEF))
	@find $(TOK_DIR) -name \*.mod -exec cat {} \; >> $(TOKEN_DEF)
	@find $(TOK_DIR) -name \*.pp -exec grep __WSF_TOKEN_DEFINE__ {} \; | cut -d"(" -f2 | cut -d")" -f1 >> $(TOKEN_DEF)
	@$(TOK_TO_HDR) $(TOKEN_DEF) $(TOKEN_HDR)

$(TOK_DIR)/%.pp: $(ROOT_DIR)/%.c
	@echo "+++ Scanning: $<"
	@mkdir -p $(dir $@)
	@$(CC) $(C_FLAGS) -DTOKEN_GENERATION -DMODULE_ID=$(call FILE_HASH,$<) -E -o $@ $<
	@echo "$(call FILE_HASH,$<), 0, $<, , ," >> $(@:.pp=.mod)

ifneq ($(BSP_DIR),)
$(TOK_DIR)/%.pp: $(BSP_DIR)/%.c
	@echo "+++ Scanning: $<"
	@mkdir -p $(dir $@)
	@$(CC) $(C_FLAGS) -DTOKEN_GENERATION -DMODULE_ID=$(call FILE_HASH,$<) -E -o $@ $<
	@echo "$(call FILE_HASH,$<), 0, $<, , ," >> $(@:.pp=.mod)
endif

token.clean:
	@rm -rf $(TOK_DIR)
	@rm -f $(TOKEN_DEF)
	@rm -f $(TOKEN_HDR)

.PHONY: token.clean
