#include <types.h>
#include <kern/errno.h>
#include <kern/reboot.h>
#include <kern/unistd.h>
#include <limits.h>
#include <lib.h>
#include <test.h>
#include "opt-sfs.h"
#include "opt-net.h"
#include "opt-hello.h"

#if OPT_HELLO
	void hello(void);
#endif
void hello(void){
	kprintf("HELLO!!!\n");
}
