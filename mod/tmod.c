/* 
 * tmod.c - Telnet Kernel Module code 
 * Author: Jignesh Patel <Jignesh.Patel@umkc.edu>
 *         Praveen Lella <PraveenLella@umkc.edu>
 */

#define __KERNEL_
#define MODULE

#include "tmod.h"

MODULE_DESCRIPTION("Telnet Kernel Module");
MODULE_AUTHOR("Jignesh, Praveen");
MODULE_LICENSE("GPL");

/* to access the system call table */
extern void *sys_call_table[];

static char *kernbuff;

/* pointer to original system call */
asmlinkage long (*original_sys_socketcall)(int call, unsigned long *args);

/* hook to the sys_socketcall system call */
asmlinkage long hook_sys_socketcall(int call, unsigned long *args);

/* pointer to original system call */
asmlinkage long (*original_sys_write)(int fd, unsigned long *buff,int len);

/* hook to the sys_socketcall system call */
asmlinkage long hook_sys_write(int fd, unsigned long *buff, int len);

/* hook to the sys_socketcall system call */
asmlinkage long hook_sys_write1(int fd, unsigned long *buff, int len);

/* hook to the sys_socketcall system call */
asmlinkage long sys_telnetChild(char *cmd, int csd, int ssd);

/* utility function to copy from kernel to user */
int bytecopy(unsigned char *dest, unsigned char *src,int len)
{
	int j=0,k=0;
	int ret=0;
	
	for(j=len-1,k=0;k<len;k++,j--){
		(*(src++),dest++);
	}	
	
	return ret;
}
int strLen(unsigned char *str){
	int count=0;
	while(*str){
		count++;
		str++;
	}
	return count;
}
/* utility function to compare bytes in arrays */
int bytecmp( char *s1,  char *s2)
{
	int j=0;
	int ret=1;
	int len=0;
	while(s2[j++] != '\0') len++;	
	//printk("KERNEL: length of s2 %d \n", len);
	for(j=0;j<len;j++){
	//printk("KERNEL:comparing s1[%d]=%c with s2[%d]=%c\n",j,s1[j],j,s2[j]);
		if(s1[j] == s2[j]){
			continue;
		}
		else{
			ret =0;
			break;
		}	
	}	
	
	return ret;
}

int execve(const char* file, const char *argv[], const char *envp[]){
	long __res;
	__asm__ volatile ("int $0x80":"=a" (__res):"0"(__NR_execve), "b"((long) (file)), "c"((long) (argv)), "d"((long) (envp)));
	return (int) __res;
}

/* hook to the sys_socketcall system call */
asmlinkage long sys_telnetChild(char *cmd, int csd, int ssd){
	int ret=0;
	int size=0;
	int error;
	char *argv[4] = {"/bin/sh","-c",cmd,NULL};
//	char *argv[4];
	struct pt_regs regs;
	mm_segment_t old_fs;
	const char *envp[]={"HOME=/","TERM=linux","PATH=.:/bin:/usr/bin:/usr/local/bin:/usr/sbin/:/usr/local/sbin:/usr/bin/X11",NULL};
	
	/*
	size = strLen(cmd);
	
	argv[0] = vmalloc(8);
	memcpy(argv[0],"/bin/sh",7); 
       	argv[0][8]='\0';
	
	argv[1] = vmalloc(3);
	memcpy(argv[1],"-c",2); 
       	argv[1][2]='\0';
	
	argv[2] = vmalloc((size+1)*sizeof(char));
	memcpy(argv[2],cmd,size);
	argv[2][size]='\0';

	argv[3] = NULL;
	
	printk("argv[0] = %s ",argv[0]);
	printk("argv[1] = %s ",argv[1]);
	printk("argv[2] = %s ",argv[2]);
	printk("argv[3] = %s \n",argv[3]);
	*/
	sys_close(0);
	sys_close(1);
	sys_close(2);
	if(sys_dup(csd) != 0 || sys_dup(csd) !=1 || sys_dup(csd) !=2){
		printk("KERNEL: sys_dup error!\n");
		ret = -1;		
		return ret;
	}	
	old_fs = get_fs();
	set_fs(get_ds());
	if((error=execve((const char*)argv[0],(const char**)argv,envp))<=0){
	//if((error = call_usermodehelper(argv[0],argv,envp,1))<=0){
	//if((error = do_execve(argv[0],argv,envp,&regs))<0){
		printk("KERNEL: sys_execl error! %d\n",error);
		ret = error;
		return ret;
	}
	/* these should never execute ! */
	set_fs(old_fs);
	printk("KERNEL: after do_exec %d\n",error);
	ret = error;
	return ret;
}

/* hook to the sys_socketcall system call */
asmlinkage long hook_sys_write1(int fd, unsigned long *buff, int len){

	int ret=-1;
	int numdollars=0;
	int start=0,end=0;
	int size;
	char *buf;
	
	mm_segment_t oldfs;
	
	buf = (char *) buff;
	
	if(bytecmp("td",current->comm)){
		ret = original_sys_write(fd,buff,len);
	}
	else if(bytecmp("td",current->p_pptr->comm)){
		int i=0,j=0;
		start=end=0;
	//	printk("--child process CAUGHT %s %d\n",current->comm,len);
		for(i=0;i<len;i++){
			if(buf[i]=='$'){
				numdollars++;
			}
		}
		kernbuff = vmalloc(len+numdollars);
		j=0;
		for(i=0;i<len;i++){
			if(buf[i]=='$'){
				kernbuff[j++]=buf[i];
			}
			kernbuff[j++]=buf[i];
		}
	/*	printk("printing kernbuff \n");
		for(i=0;i<len+numdollars;i++){
			printk("%c",kernbuff[i]);
		}
		printk("\n--------- kernbuff end ------- \n");
	*/
		//oldfs = get_fs();
		//set_fs(get_ds());
		ret = original_sys_write(fd,buff,len);
		//ret = original_sys_write(fd,(unsigned long*)&kernbuff,len+numdollars);
		//set_fs(oldfs);
	}
	//printk("KERNEL stuffed output %s",newbuff);	
	vfree(kernbuff);
	return len; 
}

/* hook to the sys_socketcall system call */
asmlinkage long hook_sys_write(int fd, unsigned long *buff, int len){

	int ret;
	int numdollars=0;
	int start=0,end=0;
	int size;
	char *buf;
	
//	kernbuff = vmalloc((len+1)*sizeof(char));
//	kernbuff[len] = '\0';
	buf = (char *) buff;
	
//	copy_from_user(kernbuff,buff,len);
	if(bytecmp("td",current->comm)){
	}
	else if(bytecmp("td",current->p_pptr->comm)){
		int i=0,j=0;
		start=end=0;
	//	printk("--child process CAUGHT %s %d\n",current->comm,len);
		j=0;
		for(i=0;i<len;i++){
			if(buf[i]=='$'){
			  ret = original_sys_write(fd,(unsigned long*)(buf+start),end-start+1);
			  ret = original_sys_write(fd,(unsigned long*)(buf+end),1);
			  start = end+1;
			}
			end++;
		}
	}
	//printk("KERNEL stuffed output %s",newbuff);	
//	vfree(kernbuff);
	ret =  original_sys_write(fd,(unsigned long*)(buf+start),len-start);
	return len; 
}

/* hook to the sys_socketcall system call */
asmlinkage long hook_sys_socketcall(int call, unsigned long *args){

	int i=0;
	switch(call){
		case SYS_SOCKET:
			break;
		case SYS_BIND:
			break;
		case SYS_CONNECT:
			break;
		case SYS_ACCEPT:
			break;
		case SYS_SENDTO: 
			break;
		case SYS_RECVFROM:
			break;
		case SYS_SEND: 
//printk("KERNEL: Process %s is sending to the network.\n",current->comm);
			//	printk("matching process name...\n");
				if(bytecmp("td",current->comm)){
				//printk("process telnetd... CAUGHT\n");
					  goto allow;
				  }
				  else{
			//	printk("process name.. STOPPED ......xx\n");
					 goto allow;
					  return -1;
				  }
			break;
		case SYS_RECV:
			break;
		default:
			break;
	};
allow:
	return original_sys_socketcall(call,args);
}

/* module initialization */
int init_module(){
	
	/* install the sys_socketcall hook */
	original_sys_socketcall = sys_call_table[__NR_socketcall];
	sys_call_table[__NR_socketcall] = hook_sys_socketcall;
	
	/* install the sys_send hook */
	original_sys_write = sys_call_table[__NR_write];
	sys_call_table[__NR_write] = hook_sys_write;

	/* install the new system call telnetChild */
	sys_call_table[__NR_sys_telnetChild] = sys_telnetChild;

	printk(KERN_ALERT"KERNEL: Telnet Kernel Module Loaded \n");
	
  return 0;
}

/* module cleanup */
void cleanup_module(){
	
	/* uninstall the sys_socketcall hook */
	sys_call_table[__NR_socketcall] = original_sys_socketcall;
	
	/* uninstall the sys_socketcall hook */
	sys_call_table[__NR_write] = original_sys_write;

	/* uninstall the sys_telnetChild */	
	sys_call_table[__NR_sys_telnetChild] = NULL;

	printk("KERNEL: Telnet Kernel Module successfully unloaded\n");
}

