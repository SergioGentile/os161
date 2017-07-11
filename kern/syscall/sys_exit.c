#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <lib.h>
#include <uio.h>
#include <syscall.h>
#include <vnode.h>
#include <vfs.h>
#include <current.h>
#include <synch.h>
#include <proc.h>

int _exit(int code){
	curthread->exit_code = code;
	curproc->p_exit_code = code;
	V(curproc->sem);
	thread_exit();	
	return 0;
}
