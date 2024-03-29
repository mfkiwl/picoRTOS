# DEVKIT-MPC5748G test board
ELF_RAM := devkit-mpg5748g-SMP.elf
ELF_FLASH := devkit-mpg5748g-SMP_flash.elf

CC := gcc
CROSS := powerpc-eabivle-

# directories
PICORTOS_DIR := $(abspath ../..)
BUILD_DIR := build

# static anaylzers
SPLINTFLAGS := -D__PPC_EABI__

# SDK (you need NXP's SDK for that one)
SDK_DIR := /c/NXP/S32DS_Power_v2.1/S32DS/software/S32_SDK_S32PA_RTM_3.0.0
DEVICES_DIR := $(SDK_DIR)/platform/devices
NXP_BUILDTOOLS_DIR := $(dir $(shell which $(CROSS)$(CC)))../..
SYSROOT := $(NXP_BUILDTOOLS_DIR)/e200_ewl2

DEFINE += -DCPU_MPC5748G -DTURN_ON_CPU0
INCLUDE += -I$(SYSROOT)/EWL_C/include -I$(SYSROOT)/EWL_C/include/pa
INCLUDE += -I$(DEVICES_DIR) -I$(DEVICES_DIR)/common
INCLUDE += -I$(DEVICES_DIR) -I. -I$(PICORTOS_DIR) 
INCLUDE += -I$(PICORTOS_DIR)/arch/include
INCLUDE += -I$(PICORTOS_DIR)/arch/ppc/e200/e200z4

CFLAGS += -Wall -Wextra -std=c99 $(INCLUDE) -O0 -g3 $(DEFINE)
CFLAGS += -fmessage-length=0 -msdata=eabi -mlra -funsigned-bitfields
CFLAGS += -ffunction-sections -fdata-sections -fno-common -mcpu=e200z4
CFLAGS += -specs=ewl_c9x.specs -mbig -mvle -mregnames -mhard-float
CFLAGS += --sysroot=$(SYSROOT) -DDEV_ERROR_DETECT

SFLAGS += -x assembler-with-cpp -g3 $(DEFINE) $(INCLUDE)
SFLAGS += -g3 -mcpu=e200z4 -specs=ewl_c9x.specs -mbig -mvle -mregnames
SFLAGS += --sysroot=$(SYSROOT) 
# enable caches
SFLAGS += -DI_CACHE -DICACHE_ENABLE=1 -DD_CACHE -DDCACHE_ENABLE=1

LDRAM := $(DEVICES_DIR)/MPC5748G/linker/gcc/MPC5748G_ram.ld
LDFLASH := $(DEVICES_DIR)/MPC5748G/linker/gcc/MPC5748G_Flash.ld
LDFLAGS += -Xlinker --gc-sections -mcpu=e200z4 -specs=ewl_c9x.specs -mhard-float
LDFLAGS += --sysroot=$(SYSROOT)

SRC_C_FILES += $(DEVICES_DIR)/startup.c
SRC_C_FILES += $(DEVICES_DIR)/MPC5748G/startup/system_MPC5748G.c
SRC_S_FILES += $(DEVICES_DIR)/MPC5748G/startup/gcc/startup_MPC5748G.S
SRC_S_FILES += $(DEVICES_DIR)/MPC5748G/startup/gcc/interrupt_vectors.S

# picoRTOS
SRC_C_FILES += $(PICORTOS_DIR)/picoRTOS-SMP.c
SRC_C_FILES += $(PICORTOS_DIR)/ipc/picoRTOS_mutex.c
SRC_C_FILES += $(PICORTOS_DIR)/ipc/picoRTOS_cond.c
SRC_C_FILES += $(PICORTOS_DIR)/arch/ppc/e200/e200z4/picoRTOS_port.c
SRC_C_FILES += $(PICORTOS_DIR)/arch/ppc/e200/e200z4/picoRTOS-SMP_port.c
SRC_S_FILES += $(PICORTOS_DIR)/arch/ppc/e200/e200z4/picoRTOS_portasm.S

# demo
SRC_C_FILES += blink.c

OBJ_FILES := $(SRC_C_FILES:%.c=$(BUILD_DIR)/%.o)
OBJ_FILES += $(SRC_S_FILES:%.S=$(BUILD_DIR)/%.o)

all: $(ELF_RAM) $(ELF_FLASH)

$(ELF_RAM): $(OBJ_FILES)
	-mkdir -p $(@D)
	$(CROSS)$(CC) $^ -T$(LDRAM) $(LDFLAGS) -o $@

$(ELF_FLASH): $(OBJ_FILES)
	-mkdir -p $(@D)
	$(CROSS)$(CC) $^ -T$(LDFLASH) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.c
	-mkdir -p $(@D)
	$(CROSS)$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	-mkdir -p $(@D)
	$(CROSS)$(CC) $(SFLAGS) -c $< -o $@

splint:
	splint $(SPLINTFLAGS) $(DEFINE) $(INCLUDE) $(SPLINT_C_FILES)

uncrustify:
	uncrustify -c $(PICORTOS_DIR)/uncrustify.cfg \
	  $(INDENT_FILES) --no-backup

clean:
	-rm -rf $(BUILD_DIR)
	-rm -f $(ELF_RAM)

.PHONY: clean splint uncrustify
