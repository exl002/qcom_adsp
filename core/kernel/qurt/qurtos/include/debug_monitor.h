#ifndef _DEBUG_MONITOR_H_
#define _DEBUG_MONITOR_H_

#include <qurtk_stddef.h>

/**
    Interface to inform the debug monitor that a process was created.
    
    @param [in] pid -- Process ID of the newly created process
    @param [in] tid -- Thread ID of the process. This is typically the
                       primordial thread or "root" thread of the newly
                       created process.
    @return None.
*/
/*extern*/ void debug_monitor_process_create_hook(const char *process_name, pid_t pid, uint32_t tid);/* __attribute__((weak));*/

/**
    Interface to inform the debug monitor that a process was stopped.
    
    @param [in] pid -- Process ID of the process being stopped.
    
    @return None.
*/
/*extern*/ void debug_monitor_process_destroy_hook(pid_t pid);/* __attribute__((weak));*/

/**
    Interface to inform the debug monitor that a process ran into an exception.
    
    @param [in] pid -- Process ID of the process that has run into an exception.
    
    @return None.
*/
/*extern*/ void debug_monitor_report_process_exception(pid_t pid);/* __attribute__((weak));*/

/**
    Interface to enable/disable the debug monitor functionality. 
    Once enabled, the service may not be restarted until the target is reset.
    Once disabled, the service may not be restarted until the target is reset.
    
    @param [in] on_off -- Integer denoting the operation 1 implies enable and 0 implies disable
    @param [in] p_cfg_data -- Pointer to config data needed by the DM.
    @param [in] cfg_size -- Size of the config data needed by the DM.
    @return
        0 -- Success
        -1 -- Error
*/
/*extern*/ int debug_monitor_ctrl(int on_off, void *p_cfg_data, size_t cfg_size);/* __attribute__((weak));*/

#endif //_DEBUG_MONITOR_H_
