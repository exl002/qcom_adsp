#ifndef SHMEM_H
#define SHMEM_H
unsigned int *qurtos_shmem_open(const char *name, int oflag, int mode);
int qurtos_shmem_release(unsigned int *shmem_handle);
unsigned int qurtos_shmem_mmap(int client_handle, unsigned int *shmem_handle, unsigned int size, unsigned int vaddr /*not being used*/);
int qurtos_shmem_munmap(int client_handle, unsigned int *shmem_handle); 

#endif
