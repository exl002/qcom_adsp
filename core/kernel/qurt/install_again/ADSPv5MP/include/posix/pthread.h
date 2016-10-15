#ifndef QURT_PTHREAD_H
#define QURT_PTHREAD_H  

/*==========================================================================
 * FILE:         mqueue.h
 *
 * SERVICES:     POSIX pthread API interface
 *
 * DESCRIPTION:  POSIX pthread API interface based upon POSIX 1003.1-2004
 *
 *               Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
 *               All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/posix/include/public/pthread.h#6 $ $DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 *
 *  when       who     what, where, why
 *  --------   ---     -------------------------------------------------------
 *  10/13/08   cz      Initial version.
 *==========================================================================*/

#include <sys/types.h>
#include <sys/sched.h>  /* For struct sched_param */
#include <sys/errno.h>  /* error values */
#include <qurt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <common/time.h>

/* the range of the set supported by the kernel data type used to represent CPU sets. */
#define CONFIG_NR_CPUS QURT_THREAD_CFG_BITMASK_ALL

#define UNIMPLEMENTED(FUNC, RETURNTYPE, ARGS)    static inline RETURNTYPE FUNC ARGS { qurt_printf("Unimplemented: %s... exiting\n", __FUNCTION__); exit(1); }

/** \details 
 * This provides POSIX thread API. 
 *
 */

/** \defgroup pthread POSIX pthread API */
/** \ingroup pthread */
/** @{ */

/** Compare Two Threads. 
 * Please refer to POSIX standard for details.  
 */
static inline int pthread_equal(pthread_t t1, pthread_t t2)
{
    return t1 == t2;
}

/** Create Thread. 
 * Please refer to POSIX standard for details.  
 */
int pthread_create(pthread_t * tid, const pthread_attr_t * attr, void *(*start)(void *), void *arg);

/** Terminate Calling Thread. 
 * Please refer to POSIX standard for details.  
 */
void pthread_exit(void *value_ptr);

/** Wait for thread termination.
 * Please refer to POSIX standard for details.
 * @param thread    [in]  the thread to be joined
 * @param value_ptr [out] the pointer of the exit status
 */
int pthread_join(pthread_t thread, void **value_ptr);
pthread_t pthread_self(void);
int pthread_cancel(pthread_t thread);
static inline void pthread_yield(void)
{
    return;
}

int pthread_kill(pthread_t thread, int sig);

int pthread_getschedparam(pthread_t thread, int *restrict policy, struct sched_param *restrict param);
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
int pthread_setschedprio(pthread_t thread, int prio);

/* Attribute functions */
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setschedparam(pthread_attr_t *restrict attr, const sched_param *restrict param);
int pthread_attr_getschedparam(const pthread_attr_t *restrict attr, sched_param *restrict param);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
int pthread_attr_setstackaddr(pthread_attr_t *attr, void * stackaddr);
int pthread_attr_getstackaddr(const pthread_attr_t *attr, void ** stackaddr);

/* Qualcomm additions to pthread get/set attribute functions */
int pthread_attr_setthreadname(pthread_attr_t *attr, const char * name);
int pthread_attr_getthreadname(const pthread_attr_t *attr, char * name, int size);
int pthread_attr_settimetestid(pthread_attr_t *attr, unsigned int tid);
int pthread_attr_gettimetestid(const pthread_attr_t *attr, unsigned int* tid);

/* Mutexes */
int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

/* For Mutex with type PTHREAD_MUTEX_NORMAL, Priority Inheritance is not 
 * supported even PTHREAD_PRIO_INHERIT is defined since QURT does not support
 * this kind of Mutex */
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict, int *restrict);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *restrict, int *restrict);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict, int *restrict);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);

/* Spinlocks */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);

/* Condition variables */
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr, int *restrict pshared);
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

/* Barriers */
int pthread_barrier_init(pthread_barrier_t *restrict barrier, const pthread_barrierattr_t *restrict attr, unsigned count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_wait(pthread_barrier_t *barrier);
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *restrict attr, int *restrict pshared);

/** please refer to POSIX standard document 
 */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);

/** set CPU affinity attribute in thread attributes object.

 * @param attr       [in] pthread attributes 
 * @param cpusetsize [in] The argument cpusetsize is the length (in bytes) 
                          of the buffer pointed to by cpuset. Typically, 
                          this argument would be specified as 
                          sizeof(cpu_set_t).
 * @param cpuset     [in] This data set is a bitset where each bit represents 
                          a CPU (hw thread). How the system's CPUs are mapped 
                          to bits in the bitset is system dependent. 
                          For QURT kernel, Bit 0 is corresponding to hw 
                          thread 0, and so on. If the corresponding bit is 
                          set to 1, then the software thread is eligible to 
                          run this hw thread.  0x3f means it can run any hw
                          threads 0x0 also means it can run on any hw threads.
   @return On success, this function returns 0; on error, it returns a 
           non-zero error number.
           EINVAL - cpuset specified a CPU that was outside the set supported 
                    by the kernel.  (The kernel configuration option 
                    CONFIG_NR_CPUS defines the range of the set supported by 
                    the kernel data type used to represent CPU sets.)
 * @note This function is non-standard GNU extensions; hence the suffix "_np"
         (non-portable) in the names. 
 */
int pthread_attr_setaffinity_np(pthread_attr_t *attr, size_t cpusetsize, const cpu_set_t *cpuset);

/** get CPU affinity attribute in thread attributes object.
 * @param attr       [in] pthread attributes 
 * @param cpusetsize [in] The argument cpusetsize is the length (in bytes) 
                          of the buffer pointed to by cpuset. Typically, 
                          this argument would be specified as 
                          sizeof(cpu_set_t).
 * @param cpuset    [out] This data set is a bitset where each bit represents 
                          a CPU (hw thread). How the system's CPUs are mapped 
                          to bits in the bitset is system dependent. 
                          For QURT kernel, Bit 0 is corresponding to hw 
                          thread 0, and so on. If the corresponding bit is 
                          set to 1, then the software thread is eligible to 
                          run this hw thread.  0x3f means it can run any hw
                          threads 0x0 also means it can run on any hw threads.
   @return On success, this function returns 0; on error, it returns a 
           non-zero error number.
           EINVAL - cpusetsize is smaller than the size of the affinity mask 
                    used by the kernel.
 * @note   This function is non-standard GNU extensions; hence the suffix "_np"
           (non-portable) in the names. 
 */
int pthread_attr_getaffinity_np(pthread_attr_t *attr, size_t cpusetsize, cpu_set_t *cpuset);

/* TLS */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_key_delete(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);
int pthread_getattr_np(pthread_t thread, pthread_attr_t * restrict attr); 	 	

/** @} */

/* Calling non-pthread calls this function to create pthred tcb w/o creating actual thread */
int pthread_fake(pthread_t * restrict thread, const pthread_attr_t * restrict attr);
int pthread_fake_destroy(pthread_t thread);

UNIMPLEMENTED(pthread_attr_getdetachstate, int, (const pthread_attr_t * attr, int state))
UNIMPLEMENTED(pthread_attr_setdetachstate, int, (const pthread_attr_t * attr, int state))

UNIMPLEMENTED(pthread_detach, int, (pthread_t id))

UNIMPLEMENTED(pthread_cond_timedwait, int, (pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec *time))

#endif /* QURT_PTHREAD_H */

