#
# Copyright (c) 2010 Atmel Corporation. All rights reserved.
#
# \asf_license_start
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

#Build Directory
BUILD_DIR = Debug/

# Path to top level ASF directory relative to this project directory.
PRJ_PATH =

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atxmega128a3u

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = Debug/karman-avionics.elf

# C source files located from the top-level source directory
CSRCS = \
src/ASF/common/boards/user_board/init.c \
src/ASF/common/services/clock/xmega/sysclk.c \
src/ASF/common/services/hugemem/avr8/avr8_hugemem.c \
src/ASF/common/services/ioport/xmega/ioport_compat.c \
src/ASF/common/services/sleepmgr/xmega/sleepmgr.c \
src/ASF/xmega/drivers/dma/dma.c \
src/ASF/common/services/usb/class/cdc/device/udi_cdc_desc.c \
src/ASF/common/services/usb/class/cdc/device/udi_cdc.c \
src/ASF/common/services/usb/udc/udc.c \
src/ASF/xmega/drivers/nvm/nvm.c \
src/ASF/xmega/drivers/spi/spi.c \
src/ASF/xmega/drivers/tc/tc.c \
src/ASF/xmega/drivers/usart/usart.c \
src/drivers/ms5607-02ba03.c \
src/drivers/n25q_512.c \
src/ASF/xmega/drivers/usb/usb_device.c \
src/framework/Scheduler.c \
src/framework/Tasks.c \
src/framework/Timer.c \
src/main.c \
src/tasks/Background.c \
src/tasks/Pyrotechnics.c \
src/tasks/RadioTask.c \
src/tasks/SensorTask.c \
src/tasks/Spi_bg_task.c \
src/utils/CC2500_regvalues.c \
src/utils/FlashMem.c \
src/utils/Spi_service.c \
src/utils/USBUtils.c \
src/tasks/USBTask.c

UNUSED_CSRCS = \
src/ASF/common/utils/stdio/read.c \
src/ASF/common/utils/stdio/write.c \
src/ASF/common/utils/unit_test/suite.c \
src/ASF/xmega/drivers/twi/twim.c \


# Assembler source files located from the top-level source directory
ASSRCS = \
        src/ASF/xmega/drivers/cpu/ccp.s \
		src/ASF/xmega/drivers/nvm/nvm_asm.s

# Include path located from the top-level source directory
INC_PATH =	\
src \
src/ASF \
src/ASF/common \
src/ASF/common/boards \
src/ASF/common/boards/user_board \
src/ASF/common/services \
src/ASF/common/services/clock \
src/ASF/common/services/clock/xmega \
src/ASF/common/services/gpio \
src/ASF/common/services/gpio/xmega_gpio \
src/ASF/common/services/hugemem \
src/ASF/common/services/hugemem/avr8 \
src/ASF/common/services/hugemem/generic \
src/ASF/common/services/ioport \
src/ASF/common/services/ioport/xmega \
src/ASF/common/services/sleepmgr \
src/ASF/common/services/sleepmgr/xmega \
src/ASF/common/services/hugemem \
src/ASF/common/services/hugemem/avr8 \
src/ASF/common/services/hugemem/generic \
src/ASF/common/services/usb \
src/ASF/common/services/usb/class/cdc \
src/ASF/common/services/usb/class/cdc/device \
src/ASF/common/services/usb/udc \
src/ASF/common/utils \
src/ASF/common/utils/interrupt \
src/ASF/common/utils/make \
src/ASF/common/utils/stdio \
src/ASF/common/utils/unit_test \
src/ASF/xmega \
src/ASF/xmega/drivers \
src/ASF/xmega/drivers/cpu \
src/ASF/xmega/drivers/nvm \
src/ASF/xmega/drivers/dma \
src/ASF/xmega/drivers/pmic \
src/ASF/xmega/drivers/sleep \
src/ASF/xmega/drivers/spi \
src/ASF/xmega/drivers/tc \
src/ASF/xmega/drivers/twi \
src/ASF/xmega/drivers/usart \
src/ASF/xmega/drivers/usb \
src/ASF/xmega/drivers/dma \
src/ASF/xmega/utils \
src/ASF/xmega/utils/assembler \
src/ASF/xmega/utils/bit_handling \
src/ASF/xmega/utils/preprocessor \
src/config \
src/framework \
src/drivers \
src/utils    \
src/tasks

# Library paths from the top-level source directory
LIB_PATH =

# Libraries to link with the project
LIBS =

# Additional options for debugging. By default the common Makefile.in will
# add -gdwarf-2.
DBGFLAGS =

# Optimization settings
OPTIMIZATION = -Os

# Extra flags used when creating an EEPROM Intel HEX file. By default the
# common Makefile.in will add -j .eeprom
# --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0.
EEPROMFLAGS =

# Extra flags used when creating an Intel HEX file. By default the common
# Makefile.in will add -R .eeprom -R .usb_descriptor_table.
FLASHFLAGS =

# Extra flags to use when archiving.
ARFLAGS =

# Extra flags to use when assembling.
ASFLAGS = -DBOARD=USER_BOARD

# Extra flags to use when compiling.
CFLAGS =

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
       -DDEBUG \
       -DBOARD=USER_BOARD \
       -DIOPORT_XMEGA_COMPAT

UNUSED_CPPFLAGS = \
       -DTEST_SUITE_DEFINE_ASSERT_MACRO \
       -D_ASSERT_ENABLE_ \
       -DLINUX_DEV

# Extra flags to use when linking
LDFLAGS =  \
       -Wl,--section-start=.BOOT=0x20000

# Pre- and post-build commands
PREBUILD_CMD =
POSTBUILD_CMD =
