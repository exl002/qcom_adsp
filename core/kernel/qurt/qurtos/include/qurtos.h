#ifndef   QURTOS_H
#define   QURTOS_H

#include <qurt_defs_internal.h>
#include <qurt_trap_constants.h>
#include <error.h>

/*
||  qurtos.h
||
||  QuRT OS has both a kernel portion which runs in monitor
||   mode and a qurtos portion which runs in guest-OS mode.
||
||  This include file contains macros, types, definitions,
||   declarations, etc., which are the external APIs to the
||   qurtos portion of QuRT.
||
||  These APIs are more stable than the ones in qurtos_internal.h,
||   but they are still subject to change and not intended for
||   direct use by users.
*/

#define qurtos_assert( cond ) \
    do{ \
        if(!(cond)){ \
            qurtos_printf("FATAL ERROR in QuRT OS: %s:%d\n", __FILE__, __LINE__ ); \
            qurt_exception_raise_shutdown_fatal(); \
        } \
    }while(0)

// OSTHREAD_STACK_SIZE is no longer used.
//  Instead, add something like one of the following to the kernel
//  section of the XML configuration file:
//     <qurtos_stack_size value = "1984"/>    # Default for CONFIG_MP
//     <qurtos_stack_size value = "448"/>     # Default for non CONFIG_MP
//     <qurtos_stack_size value = "8192"/>    # Oversized invocation stacks

struct qurtos_thread_info;

int qurtos_thread_create(int client_handle, struct QURT_ugp_ptr *pUgp, void (*pfnStart)(void *), struct qurtos_thread_info *pInfo);
void qurtos_thread_exit(int client_handle, int status);
int qurtos_thread_join(int client_handle, int target_thread, int *status);
int qurtos_spawn_flags(int client_handle, const char *name, int flags);
int qurtos_wait(int client_handle, int *status);

int qurtos_printf(const char* format, ...);

#if defined(QURT_TRAP_KERNEL_WRITEWORD)
extern inline void macro_qurtos_kernel_writewords(unsigned *p, unsigned val, unsigned cnt)
{
   __asm__ volatile("{\n"
                    "  r0 = %0\n"
                    "  r1 = %1\n"
                    "  r2 = %2\n"
                    "}\n"
                    "  trap0(#%3)" ::
                    "r" (p), "r" (val), "r" (cnt), "i" (QURT_TRAP_KERNEL_WRITEWORD) :
                    "r0", "r1", "r2",
                    "r6", "r7", "r8", "r9",
                    "r10", "r11", "r12", "r13",
                    "r14", "r15", "memory",
                    "lc0", "sa0",
                    "p0", "p1", "p2", "p3");
}
#define qurtos_kernel_writeword(p,n) macro_qurtos_kernel_writewords((unsigned *)(p), (unsigned)(n), 1)
#define qurtos_kernel_writewords(p,n,c) macro_qurtos_kernel_writewords((unsigned *)(p), (unsigned)(n), (unsigned)(c))
#endif

#endif // QURTOS_H
