/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <lib.h>
#include <vm.h>
#include <mainbus.h>


vaddr_t firstfree;   /* first free virtual address; set by start.S */

static paddr_t firstpaddr;  /* address of first free physical page */
static paddr_t lastpaddr;   /* one past end of last free physical page */

static int addr_vector[RAM_SIZE];
//static int counter_npages;
/*
 * Called very early in system boot to figure out how much physical
 * RAM is available.
 */
void
ram_bootstrap(void)
{
	size_t ramsize;
	int i;
	/* Get size of RAM. */
	ramsize = mainbus_ramsize();

	/*
	 * This is the same as the last physical address, as long as
	 * we have less than 512 megabytes of memory. If we had more,
	 * we wouldn't be able to access it all through kseg0 and
	 * everything would get a lot more complicated. This is not a
	 * case we are going to worry about.
	 */
	if (ramsize > 512*1024*1024) {
		ramsize = 512*1024*1024;
	}

	lastpaddr = ramsize;

	/*
	 * Get first free virtual address from where start.S saved it.
	 * Convert to physical address.
	 */
	firstpaddr = firstfree - MIPS_KSEG0;
	
//***MIO***
	
	firstpaddr_const = firstpaddr;
	for(i=0; i<RAM_SIZE; i++){
		addr_vector[i] = 0;
	}
	//counter_npages = 1;
//****
	kprintf("%uk physical memory available\n",
		(lastpaddr-firstpaddr)/1024);
}

/*
 * This function is for allocating physical memory prior to VM
 * initialization.
 *
 * The pages it hands back will not be reported to the VM system when
 * the VM system calls ram_getsize(). If it's desired to free up these
 * pages later on after bootup is complete, some mechanism for adding
 * them to the VM system's page management must be implemented.
 * Alternatively, one can do enough VM initialization early so that
 * this function is never needed.
 *
 * Note: while the error return value of 0 is a legal physical address,
 * it's not a legal *allocatable* physical address, because it's the
 * page with the exception handlers on it.
 *
 * This function should not be called once the VM system is initialized,
 * so it is not synchronized.
 */


//**Versione originale**
/*paddr_t ram_stealmem(unsigned long npages)
{
	size_t size;
	int i;
	paddr_t paddr;

	size = npages * PAGE_SIZE;

	if (firstpaddr + size > lastpaddr) {
		return 0;
	}
	
	paddr = firstpaddr;
	firstpaddr += size;

	
	
	return paddr;
}*/
//**Versione mia **
paddr_t ram_stealmem(unsigned long npages)
{
	size_t size;
	int i, j, pos = 0, trovato = 0;

	size = npages * PAGE_SIZE;

	/*if (firstpaddr + size > lastpaddr) {
		return 0;
	}*/
	//kprintf("Alloco.\n");
	//Questo metodo porta frammentazione, in realtà potrei vedere se c'è qualcosa libero in mezzo
	
		
//Cerco posizione libera
	for(i=0; i<RAM_SIZE && (trovato<(int)npages); i++){
		pos=i;
		trovato = 0;
		for(j=i; j<(i + (int)npages) && j<RAM_SIZE; j++){
			if(addr_vector[j]==0)
				trovato++;
		}
		
	}
//se si verifica questa condizione, vuol dire che non c'è memoria disponibile
	if(trovato<(int)npages){
		return 0;
	}

	//kprintf("trovato: %d\n", pos);

	for(i=pos; i<pos + (int)npages; i++){
		addr_vector[i] = 1;	
	}
	/*for(i=0;i<RAM_SIZE; i++){
		kprintf("%d ", addr_vector[i]);
	}
	kprintf("\n");*/
	//kprintf("Alloco %d pagine, da %d a %d\n", (int)npages, pos, pos + (int)npages);
	firstpaddr += size;
	//Ritorna l'indirizzo di partenza
	return (paddr_t)pos*PAGE_SIZE+firstpaddr_const;
}




//Aggiunta da me 
int ram_freemem(paddr_t addr, unsigned long npages){
	int i;	
	
	if(npages<=0) return -1;
	//kprintf("Dealloco %d pagine, da %d a %d\n", (int)npages, (addr - firstpaddr_const)/PAGE_SIZE, (addr - firstpaddr_const)/PAGE_SIZE + (int)npages);
	for(i=(addr - firstpaddr_const)/PAGE_SIZE; i<(int)(addr - firstpaddr_const)/PAGE_SIZE + (int)npages; i++){
		if(addr_vector[i] ==0){		
			kprintf("Errore: Memoria non allocata. Non posso deallocarla.\n");
			return -2;
		}
		else{
			addr_vector[i] = 0;
		}
	}

	/*for(i=0; i<RAM_SIZE; i++)
		kprintf("%d ", addr_vector[i]);
	kprintf("\n");	*/

	return 0;

}
/*
int ram_freemem(paddr_t addr){
	
	int idPage, pos;
	kprintf("Dealloco.\n");
	
	pos = (addr - firstpaddr_const)/PAGE_SIZE;
	if(pos>=RAM_SIZE) return -4;
	
	idPage = addr_vector[pos];
	if(addr > lastpaddr) return -3;	

	if(idPage==0){
		kprintf("Errore: memoria non allocata.\n");
		return -2;
	} 

	while(addr_vector[pos]==idPage){
		addr_vector[pos]=0;
		pos = pos + 1;
	}

	return 0;

}*/

/*
 * This function is intended to be called by the VM system when it
 * initializes in order to find out what memory it has available to
 * manage. Physical memory begins at physical address 0 and ends with
 * the address returned by this function. We assume that physical
 * memory is contiguous. This is not universally true, but is true on
 * the MIPS platforms we intend to run on.
 *
 * lastpaddr is constant once set by ram_bootstrap(), so this function
 * need not be synchronized.
 *
 * It is recommended, however, that this function be used only to
 * initialize the VM system, after which the VM system should take
 * charge of knowing what memory exists.
 */
paddr_t
ram_getsize(void)
{
	return lastpaddr;
}

/*
 * This function is intended to be called by the VM system when it
 * initializes in order to find out what memory it has available to
 * manage.
 *
 * It can only be called once, and once called ram_stealmem() will
 * no longer work, as that would invalidate the result it returned
 * and lead to multiple things using the same memory.
 *
 * This function should not be called once the VM system is initialized,
 * so it is not synchronized.
 */
paddr_t
ram_getfirstfree(void)
{

//In quanto ho modificato l'allocatore, questa funzione ha poco senso.
	/*paddr_t ret;

	ret = firstpaddr;
	firstpaddr = lastpaddr = 0;
	return ret;*/

	int i;
	for(i=0; i<RAM_SIZE; i++)
		if(addr_vector[i]==0)
			return i*PAGE_SIZE+firstpaddr_const;
	return (paddr_t)0;
	
}
