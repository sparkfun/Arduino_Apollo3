#******************************************************************************
#
# Makefile - Rules for building the libraries, examples and docs.
#
# Copyright (c) 2019, Ambiq Micro
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
# 
# Third party software included in this distribution is subject to the
# additional license terms as defined in the /docs/licenses directory.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# This is part of revision 2.1.0 of the AmbiqSuite Development Package.
#
#******************************************************************************

# ARTEMIS_BIN2BOARD is the command to run the combined Ambiq tools (bin to ota blob, 
# ota blob to wired blob, wired update). You'll need to determine how to engage it
# for your own system. (You might use pyinstaller to create a binary executable, or)
# call it via python3
# Examples:
# (via python) 										ARTEMIS_BIN2BOARD=python "path_to_tool/ambiq_bin2board.py"
# (via executable [windows]) 						ARTEMIS_BIN2BOARD="path_to_tool/ambiq_bin2board_installed.exe"
# (via executable [*nix]) 							ARTEMIS_BIN2BOARD=./path_to_tool/ambiq_bin2board_installed
# (via executable on in the default search path) 	ARTEMIS_BIN2BOARD=ambiq_bin2board_installed
ARTEMIS_SVL=
# COM_PORT is the serial port to use for uploading. For example COM#### on Windows or /dev/cu.usbserial-#### on *nix
COM_PORT=
ifeq ($(ARTEMIS_SVL),)
        $(warning warning: you have not defined ARTEMIS_SVL so bootloading will not work)
endif
ifeq ($(COM_PORT),)
        $(warning warning: you have not defined COM_PORT so bootloading will not work)
endif

# SDKPATH				 =C:/Users/owen.lyke/AppData/Roaming/AmbiqSDK/AmbiqSuite-Rel2.1.0# Note that if you copy/paste a windows file path here you need to change backslashes to forward slashes
SDKPATH				 =
ifeq ($(SDKPATH),)
	SDKPATH			=../../../../..
        $(warning warning: you have not defined SDK_PATH so will continue assuming that the SDK root is at $(SDKPATH))
	
endif

BOARDPATH			 =${SDKPATH}/boards/artemis# Set this as the path to the Artemis BSP (Board Support Package) directory. Initially assumes that you put the BSP under the 'boards' directory in the SDK


USER_INCLUDE_DIRS	 = -I../src/svl_ringbuf
USER_INCLUDE_DIRS	+= -I../src/svl_packet
USER_INCLUDE_DIRS	+= -I../src/svl_uart
USER_INCLUDE_DIRS	+= -I../src/svl_utils

USER_SOURCEDIRS		 = ../src
USER_SOURCEDIRS		+= ../src/svl_ringbuf
USER_SOURCEDIRS		+= ../src/svl_packet
USER_SOURCEDIRS		+= ../src/svl_uart
USER_SOURCEDIRS		+= ../src/svl_utils

USER_MAIN_SRC		 = main.c
USER_SOURCE_FILES	 = svl_ringbuf.c
USER_SOURCE_FILES	+= svl_packet.c
USER_SOURCE_FILES	+= svl_uart.c
USER_SOURCE_FILES	+= svl_utils.c


#### Names ####
TARGET := artemis_svl
COMPILERNAME := gcc
PROJECT := artemis_svl_gcc
CONFIG := bin

SHELL:=/bin/bash
#### Setup ####

TOOLCHAIN ?= arm-none-eabi
PART = apollo3
CPU = cortex-m4
FPU = fpv4-sp-d16
# Default to FPU hardware calling convention.  However, some customers and/or
# applications may need the software calling convention.
#FABI = softfp
FABI = hard

LINKER_FILE := ./artemis_svl_gcc.ld
STARTUP_FILE := ./startup_$(COMPILERNAME).c

#### Required Executables ####
CC = $(TOOLCHAIN)-gcc
GCC = $(TOOLCHAIN)-gcc
CPP = $(TOOLCHAIN)-cpp
LD = $(TOOLCHAIN)-ld
CP = $(TOOLCHAIN)-objcopy
OD = $(TOOLCHAIN)-objdump
RD = $(TOOLCHAIN)-readelf
AR = $(TOOLCHAIN)-ar
SIZE = $(TOOLCHAIN)-size
RM = $(shell which rm 2>/dev/null)

EXECUTABLES = CC LD CP OD AR RD SIZE GCC
K := $(foreach exec,$(EXECUTABLES),\
        $(if $(shell which $($(exec)) 2>/dev/null),,\
        $(info $(exec) not found on PATH ($($(exec))).)$(exec)))
$(if $(strip $(value K)),$(info Required Program(s) $(strip $(value K)) not found))

ifneq ($(strip $(value K)),)
all clean:
	$(info Tools $(TOOLCHAIN)-$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

DEFINES = -DPART_$(PART)
DEFINES+= -DAM_PART_APOLLO3
DEFINES+= -DAM_PACKAGE_BGA

# Includes from the SDK
# INCLUDES+= -I../../../../.. # This one doesn't make much sense to me
INCLUDES = -I${SDKPATH}/CMSIS/AmbiqMicro/Include
INCLUDES+= -I${SDKPATH}/mcu/apollo3
INCLUDES+= -I${SDKPATH}/CMSIS/ARM/Include
INCLUDES+= -I${SDKPATH}/utils
INCLUDES+= -I${SDKPATH}/devices

# Includes for the board you've chosen
INCLUDES+= -I../src # This is where the source for the particular project is
INCLUDES+= -I${BOARDPATH}/bsp

INCLUDES+= ${USER_INCLUDE_DIRS}

VPATH = ${SDKPATH}/devices
VPATH+= ${SDKPATH}/utils
VPATH+= ${USER_SOURCEDIRS}

SRC = ${USER_MAIN_SRC}
SRC += am_devices_led.c
SRC += am_util_delay.c
SRC += am_util_faultisr.c
SRC += am_util_stdio.c
SRC += am_util_id.c
SRC += startup_gcc.c
SRC += ${USER_SOURCE_FILES}

CSRC = $(filter %.c,$(SRC))
ASRC = $(filter %.s,$(SRC))

OBJS = $(CSRC:%.c=$(CONFIG)/%.o)
OBJS+= $(ASRC:%.s=$(CONFIG)/%.o)

DEPS = $(CSRC:%.c=$(CONFIG)/%.d)
DEPS+= $(ASRC:%.s=$(CONFIG)/%.d)

LIBS = ../../../../../mcu/apollo3/hal/gcc/bin/libam_hal.a
LIBS += ../../../bsp/gcc/bin/libam_bsp.a

CFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CFLAGS+= -ffunction-sections -fdata-sections
CFLAGS+= -MMD -MP -std=c99 -Wall -g
# CFLAGS+= -O3
CFLAGS+= -O0
CFLAGS+= $(DEFINES)
CFLAGS+= $(INCLUDES)
CFLAGS+= 

LFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
LFLAGS+= -nostartfiles -static
LFLAGS+= -Wl,--gc-sections,--entry,Reset_Handler,-Map,$(CONFIG)/$(TARGET).map
LFLAGS+= -Wl,--start-group -lm -lc -lgcc $(LIBS) -Wl,--end-group
LFLAGS+= -specs=nano.specs -specs=nosys.specs

# Additional user specified CFLAGS
CFLAGS+=$(EXTRA_CFLAGS)

CPFLAGS = -Obinary

ODFLAGS = -S

#### Rules ####
all: directories $(CONFIG)/$(TARGET).bin

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@

$(CONFIG)/%.o: %.c $(CONFIG)/%.d
	@echo " Compiling $(COMPILERNAME) $<" ;\
	$(CC) -c $(CFLAGS) $< -o $@

$(CONFIG)/%.o: %.s $(CONFIG)/%.d
	@echo " Assembling $(COMPILERNAME) $<" ;\
	$(CC) -c $(CFLAGS) $< -o $@

$(CONFIG)/$(TARGET).axf: $(OBJS) $(LIBS)
	@echo " Linking $(COMPILERNAME) $@" ;\
	$(CC) -Wl,-T,$(LINKER_FILE) -o $@ $(OBJS) $(LFLAGS)

$(CONFIG)/$(TARGET).bin: $(CONFIG)/$(TARGET).axf
	@echo " Copying $(COMPILERNAME) $@..." ;\
	$(CP) $(CPFLAGS) $< $@ ;\
	$(OD) $(ODFLAGS) $< > $(CONFIG)/$(TARGET).lst

clean:
	@echo "Cleaning..." ;\
	$(RM) -f $(OBJS) $(DEPS) \
	    $(CONFIG)/$(TARGET).bin $(CONFIG)/$(TARGET).axf \
	    $(CONFIG)/$(TARGET).lst $(CONFIG)/$(TARGET).map

bootload: all
	C:\Users\owen.lyke\AppData\Local\Arduino15\packages\SparkFun\hardware\apollo3\1.0.2/tools/ambiq/windows/ambiq_bin2board.exe --bin C:\Users\owen.lyke\AppData\Roaming\AmbiqSDK\AmbiqSuite-Rel2.2.0\boards\artemis\examples\!artemis_svl\gcc\bin\artemis_svl.bin --load-address-blob 0x20000 --magic-num 0xCB -o C:\Users\owen.lyke\AppData\Roaming\AmbiqSDK\AmbiqSuite-Rel2.2.0\boards\artemis\examples\!artemis_svl\gcc\bin\artemis_svl --version 0x0 --load-address-wired 0xC000 -i 6 --options 0x1 -b 115200 -port COM4 -r 2 -v 

$(CONFIG)/%.d: ;

../../../../../mcu/apollo3/hal/gcc/bin/libam_hal.a:
	$(MAKE) -C ../../../../../mcu/apollo3/hal

../../../bsp/gcc/bin/libam_bsp.a:
	$(MAKE) -C ../../../bsp

# Automatically include any generated dependencies
-include $(DEPS)
endif
.PHONY: all clean directories
