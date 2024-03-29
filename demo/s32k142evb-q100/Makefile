# S32K142 test board
ELF_RAM := s32k142evb-q100.elf
ELF_FLASH := s32k142evb-q100_Flash.elf

CC := gcc
CROSS := arm-none-eabi-

# directories
PICORTOS_DIR := $(abspath ../..)
BUILD_DIR := build

# SDK (you need NXP's SDK for that one)
#SDK_DIR := $(S32K116_SDK_3.0.0_PATH)
SDK_DIR := /c/NXP/S32DS_ARM_v2.2/S32DS/software/S32SDK_S32K1xx_RTM_3.0.0
DEVICES_DIR := $(SDK_DIR)/platform/devices
NXP_BUILDTOOLS_DIR := $(dir $(shell which $(CROSS)$(CC)))..
SYSROOT := $(NXP_BUILDTOOLS_DIR)/arm-none-eabi/newlib

DEFINE := -DCPU_S32K142 -DI_CACHE -DICACHE_ENABLE=1
INCLUDE := -I$(DEVICES_DIR) -I.
INCLUDE += -I$(PICORTOS_DIR) -I$(PICORTOS_DIR)/arch/include
INCLUDE += -I$(PICORTOS_DIR)/arch/arm/cm4f

CFLAGS += -Wall -Wextra -std=c99 $(INCLUDE) -O1 -g3 $(DEFINE)
CFLAGS += -fmessage-length=0 -ffunction-sections -fdata-sections
CFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -specs=nano.specs -specs=nosys.specs
CFLAGS += --sysroot=$(SYSROOT) -DDEV_ERROR_DETECT

SFLAGS += $(DEFINE) $(INCLUDE) -g3
SFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
SFLAGS += -specs=nano.specs -specs=nosys.specs --sysroot=$(SYSROOT)

LDRAM := $(DEVICES_DIR)/S32K142/linker/gcc/S32K142_32_ram.ld
LDFLASH := $(DEVICES_DIR)/S32K142/linker/gcc/S32K142_32_Flash.ld
LDFLAGS += -Xlinker --gc-sections -n
LDFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -specs=nano.specs -specs=nosys.specs --sysroot=$(SYSROOT)

SRC_C_FILES += $(DEVICES_DIR)/startup.c
SRC_C_FILES += $(DEVICES_DIR)/S32K142/startup/system_S32K142.c
SRC_S_FILES += $(DEVICES_DIR)/S32K142/startup/gcc/startup_S32K142.S

# picoRTOS
SRC_C_FILES += $(PICORTOS_DIR)/picoRTOS.c
SRC_C_FILES += $(PICORTOS_DIR)/arch/arm/cm4f/picoRTOS_port.c
SRC_S_FILES += $(PICORTOS_DIR)/arch/arm/cm4f/picoRTOS_portasm.S

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
	-rm -f $(ELF_RAM) $(ELF_FLASH)

.PHONY: clean splint uncrustify
