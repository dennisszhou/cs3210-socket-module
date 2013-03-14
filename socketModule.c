#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

#include <linux/net.h>
#include <linux/in.h>
#include <linux/netpoll.h>

MODULE_LICENSE("GPL");

#define MODULE_NAME		"[socketModule] "
#define LOG_SIZE		4*PAGE_SIZE
#define MESSAGE_SIZE	1024
#define INADDR_SEND		((unsigned long int) 0x803D6868) //128.61.104.104
#define INADDR_LOCAL	((unsigned long int) 0xc0a80a55) //192.168.10.85

static struct socket *sock;
static struct proc_dir_entry *proc_entry;
static char *log_buffer;

static struct netpoll *np = NULL;
static struct netpoll np_t;

ssize_t log_write(struct file *filp, const char __user *buffer, unsigned long len, void *data) {

	char message[100];
	int plen;

	np_t.name = "LRNG";
	strlcpy(np_t.dev_name, "eth0", IFNAMSIZ);
	np_t.local_ip = INADDR_LOCAL;
	np_t.remote_ip = INADDR_SEND;
	np_t.local_port = 12000;
	np_t.remote_port = 12000;
	memset(np_t.remote_mac, 0xff, ETH_ALEN);
	netpoll_print_options(&np_t);
	netpoll_setup(&np_t);
	np = &np_t;

	sprintf(message, "%d", 42);
	plen = strlen(message);
	netpoll_send_udp(np, message, plen);

	if(copy_from_user(&log_buffer[0], buffer, len)) {
		return -EFAULT;
	}

	return len;
}

int log_read(char *page, char **start, off_t offset, int count, int *eof, void *data) {
	return 0;
}

int init_socket_module(void) {
	int ret = 0;
	ret = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock);

	if(ret < 0)
		printk(KERN_DEBUG "Can't create socket. Error: %d\n", ret);


	//ProcFS
	log_buffer = (char *) vmalloc(LOG_SIZE);

	if(!log_buffer) 
		ret = -ENOMEM;
	else {
		memset(log_buffer, 0, LOG_SIZE);

		proc_entry = create_proc_entry("monitorlog", 0644, NULL);

		if(proc_entry == NULL) {
			ret = -ENOMEM;
			vfree(log_buffer);
			printk(KERN_INFO MODULE_NAME "Could not create proc entry\n");
		}
		else {
			proc_entry->read_proc = log_read;
			proc_entry->write_proc = log_write;
			printk(KERN_INFO MODULE_NAME "Loaded Module.\n");
		}
	}

	return ret;
}

void cleanup_socket_module(void) {
	remove_proc_entry("monitorlog", NULL);
	vfree(log_buffer);
	printk(KERN_INFO "Removing Socket Module");
}

module_init(init_socket_module);
module_exit(cleanup_socket_module);
