#ifndef QURTOS_KTRAPS_H
#define QURTOS_KTRAPS_H

/* Define macros so that we can pass anywhere from 0 to 6 arguments to
   a TRAP with the same macro, and so that all of the arguments are
   passed in registers... */

#define _qmcast_0()
#define _qmcast_1(a) (int)(a)
#define _qmcast_2(a,b) (int)(a),(int)(b)
#define _qmcast_3(a,b,c) (int)(a),(int)(b),(int)(c)
#define _qmcast_4(a,b,c,d) (int)(a),(int)(b),(int)(c),(int)(d)
#define _qmcast_5(a,b,c,d,e) (int)(a),(int)(b),(int)(c),(int)(d),(int)(e)
#define _qmcast_6(a,b,c,d,e,f) (int)(a),(int)(b),(int)(c),(int)(d),(int)(e),(int)(f)
#define _qmpaste_(a,b) a##b
#define _qmpaste(a,b) _qmpaste_(a,b)
#define _qmcnt_(a,b,c,d,e,f,g,cnt,...) cnt
#define _qmcnt(...) _qmcnt_(__VA_ARGS__,6,5,4,3,2,1,0)
#define qmtrapcall(f,...) \
   _qmpaste(f,_qmcnt(f,##__VA_ARGS__))(_qmpaste(_qmcast_,_qmcnt(0,##__VA_ARGS__))(__VA_ARGS__))
#define qmtrapfunc(s) \
   extern int s##0(void); \
   extern int s##1(int); \
   extern int s##2(int,int); \
   extern int s##3(int,int,int); \
   extern int s##4(int,int,int,int); \
   extern int s##5(int,int,int,int,int); \
   extern int s##6(int,int,int,int,int,int)

qmtrapfunc(qurtos_osthread_trap);

static inline QURTK_thread_context *qurtos_thread_alloc(int config)
{
   return (void *)qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_ALLOC, config);
}

static inline QURTK_thread_context *qurtos_thread_install(QURTK_thread_context *tcb_to_copy, struct thread_start_info *pInfo)
{
   return (void *)qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_INSTALL, tcb_to_copy, pInfo);
}

static inline int qurtos_thread_remove(QURTK_thread_context *tcb_to_remove, int status, unsigned int joiner_tid)
{
   return qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_REMOVE, tcb_to_remove, status, joiner_tid);
}

static inline int qurtos_thread_free(QURTK_thread_context *pTcb)
{
   return qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_FREE, pTcb);
}

static inline unsigned qurtos_thread_reap(void)
{
   return qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_REAP);
}

static inline QURTK_thread_context *qurtos_thread_get_tcb(void)
{
   return (void *)qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_CURRTCB);
}

static inline QURTK_thread_context *qurtos_thread_lookup(int id)
{
   return (void *)qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_LOOKUP, id);
}

static inline int qurtos_enable_reaper_error_handling(void)
{
   return qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_REAPER_ERR_ENABLE);
}

static inline int qurtos_thread_precious(void)
{
   return qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_PRECIOUS);
}

static inline int qurtos_thread_wake_reaper(void)
{
   return qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_WAKE_REAPER);
}

static inline int qurtos_thread_set_reaper(void)
{
   return qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_SET_REAPER);
}

static inline int qurtos_thread_set_int_info(int element_num, unsigned int asid)
{
   return qmtrapcall(qurtos_osthread_trap, QURTOS_OSTHREAD_INT_INFO, element_num, asid);
}


#endif /* QURTOS_KTRAPS_H */
