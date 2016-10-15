#ifndef   QURTOS_INTERNAL_ASM_H
#define   QURTOS_INTERNAL_ASM_H

/*
||  qurtos_internal_asm.h
||
||  QuRT OS has both a kernel portion which runs in monitor
||   mode and a qurtos portion which runs in guest-OS mode.
||
||  This include file contains macros, types, definitions,
||   declarations, etc., which are specific to the qurtos
||   portion of QuRT as needed by assembly language files.
||
||  Nothing in this file is intended to be a public API and
||   everything in this file is to be considered subject to
||   change at any time.
*/

// Get auto generated include files from build/$TARGET/common/include
#include <qurt_trap_constants.h>

// Get those include files which are pulled from libs/qurt/include/public 
#include <qurt_consts.h>

// Get those include files which are pulled from libs/qurt/include
#include <qurt_sections.h>

// Get the kernel include files from kernel/include
#include <qurtos_constants.h>

#endif // QURTOS_INTERNAL_ASM_H
