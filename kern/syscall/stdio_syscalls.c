#include <kern/unistd.h>
#include <types.h>
#include <cdefs.h>
#include <lib.h>
#include <syscall.h>
#include <vnode.h>
#include <vfs.h>
#include <uio.h>
struct vnode *fd_vect[MAX_FD];

int sys_write(int fd, void *buf, size_t count){
	size_t i;
	int result;
      	struct iovec iov;
      	struct uio ku;
	if(fd==1 || fd==2){
		for(i=0; i<count; i++)
			kprintf("%c", ((char *)buf)[i]);     	
	}
	else{
		uio_kinit(&iov, &ku, buf, count, 0, UIO_WRITE);
     		result = VOP_WRITE(fd_vect[fd], &ku);
      		if (result) {
           		return result;
		}
	}
	return 1;
}

int sys_read(int fd, void *buf, size_t count){
	int result;
      	struct iovec iov;
      	struct uio ku;

	if(fd==0 && count == 1){
		kgets((char *) buf, count);
	}
	else{
  	 	uio_kinit(&iov, &ku, buf, count, 0, UIO_READ);
     		result = VOP_READ(fd_vect[fd], &ku);
      		if (result) {
           		return result;
		}
        }
	return 1;
}

int sys_open(char *path, int openflags, int32_t *result){
	
	struct vnode *v;
	int i, found = 0, ret;
	/* Cerco di aprire il file */
        ret = vfs_open(path, openflags, 0, &v);

       	if (ret) {
        	return ret;
       	}
	
	for(i=3; i<MAX_FD && found!=1; i++){
		if(fd_vect[i] == NULL){
			fd_vect[i] = v;
			found=1;
			*result = i;
		}
	}


	if(found==0){
		return -1;
	}
	
	return ret;
	
}

int sys_close(int index){
	if(fd_vect[index] !=NULL){
		vfs_close(fd_vect[index]);
		fd_vect[index] = NULL;
		return 0;
	}
	return -1;	
}
