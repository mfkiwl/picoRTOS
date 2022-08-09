# LAUNCHXL-F28069M board
OUT_RAM := launchxl-f28379d.out
OUT_FLASH := launchxl-f28379d_Flash.out

CC := cl2000

# directories
PICORTOS_DIR := ../..
BUILD_DIR := build

# SDK (you need TI C2000Ware SDK for this one)
SDK_DIR := /c/ti/C2000Ware_4_00_00_00
DEVICES_DIR := $(SDK_DIR)/device_support/f2837xd/common

DEFINE := -D__TMS320C28XX__
INCLUDE := -I. -I$(PICORTOS_DIR)
INCLUDE += -I$(PICORTOS_DIR)/arch/c2000/c28x
INCLUDE += -I$(PICORTOS_DIR)/arch/include

TOOLCHAIN_DIR := $(dir $(shell which $(CC)))..
INCLUDE += -I$(TOOLCHAIN_DIR)/include

CFLAGS += -v28 -ml -mt -O0 $(DEFINE) $(INCLUDE)
CFLAGS += --float_support=fpu64 --preproc_with_compile --abi=eabi
CFLAGS += --preproc_dependency="$(basename $(<F)).d_raw"

SFLAGS := $(CFLAGS)

LDRAM := -l2837xD_RAM_lnk_cpu1.cmd
LDFLASH := --define=_FLASH --define=CPU1
LDFLASH += -l2837xD_FLASH_lnk_cpu1.cmd

LDFLAGS := -v28 -ml -mt -O0 -g --abi=eabi -z --stack_size=0x100 --float_support=fpu64
LDFLAGS += $(INCLUDE) --warn_sections --reread_libs --priority
LDFLAGS += -i"$(DEVICES_DIR)/cmd" --disable_auto_rts --rom_model
LDFLAGS += --search_path="$(TOOLCHAIN_DIR)/lib" -lrts2800_fpu64_eabi.lib

# picoRTOS
SRC_C_FILES += $(PICORTOS_DIR)/picoRTOS.c
SRC_C_FILES += $(PICORTOS_DIR)/arch/c2000/c28x/picoRTOS_port.c
SRC_S_FILES += $(PICORTOS_DIR)/arch/c2000/c28x/picoRTOS_portasm.asm

# IPCs extension
SRC_C_FILES += $(PICORTOS_DIR)/ipc/picoRTOS_futex.c

# demo
SRC_C_FILES += blink.c

OBJ_FILES := $(SRC_C_FILES:%.c=%.o)
OBJ_FILES += $(SRC_S_FILES:%.asm=%.o)

all: $(OUT_RAM) $(OUT_FLASH)

$(OUT_RAM): $(OBJ_FILES)
	-mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDRAM) -o $@

$(OUT_FLASH): $(OBJ_FILES)
	-mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDFLASH) -o $@

%.o: %.c
	-mkdir -p $(@D)
	echo $(SRC_C_FILES)
	$(CC) $(CFLAGS) -c $< --output_file=$@

%.o: %.asm
	-mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< --output_file=$@

splint:
	splint $(SPLINTFLAGS) $(DEFINE) $(INCLUDE) $(SPLINT_C_FILES)

uncrustify:
	uncrustify -c $(PICORTOS_DIR)/uncrustify.cfg \
	  $(INDENT_FILES) --no-backup

clean:
	-rm -rf $(BUILD_DIR)
	-rm -f $(OUT_RAM) $(OUT_FLASH)
	-rm -f $(OBJ_FILES) *.d_raw

.PHONY: clean splint uncrustify
