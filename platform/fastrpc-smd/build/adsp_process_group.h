#ifndef _ADSP_PROCESS_GROUP_H
#define _ADSP_PROCESS_GROUP_H
#include "AEEStdDef.h"
#ifndef __QAIC_HEADER
#define __QAIC_HEADER(ff) ff
#endif //__QAIC_HEADER

#ifndef __QAIC_HEADER_EXPORT
#define __QAIC_HEADER_EXPORT
#endif // __QAIC_HEADER_EXPORT

#ifndef __QAIC_HEADER_ATTRIBUTE
#define __QAIC_HEADER_ATTRIBUTE
#endif // __QAIC_HEADER_ATTRIBUTE

#ifndef __QAIC_IMPL
#define __QAIC_IMPL(ff) ff
#endif //__QAIC_IMPL

#ifndef __QAIC_IMPL_EXPORT
#define __QAIC_IMPL_EXPORT
#endif // __QAIC_IMPL_EXPORT

#ifndef __QAIC_IMPL_ATTRIBUTE
#define __QAIC_IMPL_ATTRIBUTE
#endif // __QAIC_IMPL_ATTRIBUTE
#ifdef __cplusplus
extern "C" {
#endif
#define _const_adsp_process_group_handle 1
typedef struct mmap_page mmap_page;
struct mmap_page {
   uint32 phy_addr;
   uint32 page_size;
};
__QAIC_HEADER_EXPORT int __QAIC_HEADER(adsp_process_group_create)(int pgid) __QAIC_HEADER_ATTRIBUTE;
__QAIC_HEADER_EXPORT int __QAIC_HEADER(adsp_process_group_destroy)(int pgid) __QAIC_HEADER_ATTRIBUTE;
__QAIC_HEADER_EXPORT int __QAIC_HEADER(adsp_process_group_mmap)(int pgid, uint32 flags, uint32 vaddrin, const mmap_page* pages, int pagesLen, unsigned int* vaddrout) __QAIC_HEADER_ATTRIBUTE;
__QAIC_HEADER_EXPORT int __QAIC_HEADER(adsp_process_group_munmap)(int pgid, uint32 vaddrin, int len) __QAIC_HEADER_ATTRIBUTE;
#ifdef __cplusplus
}
#endif
#endif //_ADSP_PROCESS_GROUP_H
