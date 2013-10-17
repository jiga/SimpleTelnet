/* 	
 * 	Telnet kernel module header includes
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/net.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <sys/syscall.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/wrapper.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/smp_lock.h>
#include <asm/atomic.h>
#include <linux/sys.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/errno.h>
#include <asm/errno.h>
#include <linux/slab.h>
#include <linux/string.h>

#define __NR_sys_telnetChild	253
#define __KERNEL_SYSCALLS__
