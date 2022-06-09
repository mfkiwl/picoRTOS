# Release notes

## picoRTOS v1.4.1
### What's new ?

ARM context switching assembly code has been simplified a lot, removing as many useless
instructions as possible.

A major bug has been fixed on Cortex-M4, FPU was not configured properly and led to
crashes if used.

## picoRTOS v1.4.0
### What's new ?

One new architecture has been added:
 - Cortex-M4F (ARM)

The corresponding blink demo has been added to the demo directory
at demo/s32k142evb-q100 and the static analysis is performed by the
makefile.

## picoRTOS v1.3.2
### What's new ?

On NXP MPC574xC/G, the e200z2 can now be used as an auxiliary core and a
major bug has been fixed (auxilary core crashed when trying to access the data section).
The picoRTOS_port.h file has been removed (for better or for worse).

The ARM CM0+ family (including RP2040) have been refactored and duplicated asm code
has been removed.

The CM3 port has been slightly improved and IPCs have been added to the Arduino Due demo.

On c28x, the assembly code has been refactored to be compatible with the eabi option
(required if you want to use fpu64).

A /*@unused@*/ splint tag has been added to the trylock functions to notify they
may or may not be called externally.

## picoRTOS v1.3.1
### What's new ?

IPC support has been added to RP2040 (+ demo).
Clear guidelines have been added to the CONTRIBUTING.md file.

## picoRTOS v1.3
### What's new ?

One new SMP architecture has been added:
 - RP2040 (Raspberry Pico)

On this architecture, pwm0 acts as the periodic interrupt timer as the
irq is shared by the 2 cores' NVICs and allows their synchronization.

The minimal stack on ARM has been increased by 4 bytes as some crashes occurred
in -Os mode on severely RAM limited targets (like NXP S32k116)

Adding -DNDEBUG in CFLAGS has proven to cause issues with ARM's VTABLE alignment
so the systematic copy of the VTABLE to RAM has been made optional

## picoRTOS v1.2
### What's new ?

API was improved, picoRTOS now provides the following call:
 - picoRTOS_self (returns the calling task its priority/id)

Ports now can implement two new interfaces related to atomic operations:
 - arch_test_and_set
 - arch_compare_and_swap

The following IPCs were added under the ipc/ directory:
 - spinlocks (require arch_test_and_set)
 - futexes (require arch_test_and_set)
 - re-entrant mutexes (require arch_compare_and_swap)
 - conditions (require mutexes)

Mutexes and conditions are very POSIX-lookalike, for some strange reason :)

Three Arduino architectures and their relative demos are provided:
 - ARM Cortex-M3 (Arduino Due)
 - ATmega2560 (Arduino Mega 2560)
 - ATmega328P (Arduino Uno)

Refactoring of PPC targets to make inclusion in existing projects easier

Usual share of bugfixes on existing supported architectures:
 - Better stack management on PPC and Cortex-M0+
 - Bad use of reservation bit on PPC e200

## picoRTOS v1.1
### What's new ?

Two missing targets and their demo code were added:
 - Linux / phtreads simulator
 - Windows simulator

## picoRTOS v1.0
### What's new ?

Everything. picoRTOS finally goes public, free of charge and expurgated of any problematic
intellectual property. To be on the safe side, this is a very stripped down version, huge
improvements might be added along the way as we discover more non-litigous features. 

The following architectures are supported :
 - TI C2000 / c28x (single core)
 - ARM Cortex-M0+ (single core)
 - PowerPC e200 series (single core and multicore SMP)

Some demo code is provided for these 3 architectures under demo/
