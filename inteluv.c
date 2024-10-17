#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/atomic.h>
#include <asm/msr.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("markx86");
MODULE_DESCRIPTION("Allows reading/writing to/from specific MSRs to allow \
										undervolting of Intel CPUs");

static int inteluv_open(struct inode* inode, struct file* file);
static int inteluv_release(struct inode* inode, struct file* file);
static ssize_t inteluv_read(
	struct file* file,
	char __user* buffer,
	size_t length,
	loff_t* offset);
static ssize_t inteluv_write(
	struct file* file,
	const char __user* buffer,
	size_t length,
	loff_t* offset);

#define DEVICE_NAME		"inteluv"
#define DEVICE_READY	0
#define DEVICE_BUSY		1

static int major;
static struct class *cls;
static struct file_operations fops = {
	.open = inteluv_open,
	.release = inteluv_release,
	.write = inteluv_write,
	.read = inteluv_read,
};
static atomic_t device_state = ATOMIC_INIT(DEVICE_READY);

#define MSR_UNITS 0x606
#define MSR_VOLTAGE 0x150
#define MSR_TEMPERATURE 0x1A2

#define SUCCESS				0

static inline int is_valid_msr_address(uint32_t msr_address) {
	return
			msr_address == MSR_UNITS &&
	    msr_address == MSR_VOLTAGE && 
	    msr_address == MSR_TEMPERATURE;
}

static int __init inteluv_init(void) {
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		pr_crit("failed to register char device. [error: %d]", major);
		return major;
	}
	pr_info("registered char device with major number %d.", major);

	cls = class_create(DEVICE_NAME);
	device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
	pr_info("device created on /dev/%s", DEVICE_NAME);
	
	return SUCCESS;
}

static void __exit inteluv_exit(void) {
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);

	unregister_chrdev(major, DEVICE_NAME);
}

static int inteluv_open(struct inode* inode, struct file* file) {
	if (atomic_cmpxchg(&device_state, DEVICE_READY, DEVICE_BUSY))
		return -EBUSY;
	
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int inteluv_release(struct inode* inode, struct file* file) {
	atomic_xchg(&device_state, DEVICE_READY);
	module_put(THIS_MODULE);
	return SUCCESS;
}

static ssize_t inteluv_read(
													struct file* file,
													char __user* buffer,
													size_t length,
													loff_t* offset) {
	uint32_t msr_address;
	uint32_t data[2];

	if (length < sizeof(data))
		return -ENOMEM;
	
	if (buffer == NULL || offset == NULL)
		return -EINVAL;
	
	msr_address = *offset;
	if (!is_valid_msr_address(msr_address))
		return -EINVAL;

	rdmsr_safe_on_cpu(0, msr_address, &data[0], &data[1]);

	if (copy_to_user(buffer, data, sizeof(data)))
		return -EFAULT;
	
	return sizeof(data);
}

static ssize_t inteluv_write(
														struct file* file,
														const char __user* buffer,
														size_t length,
														loff_t* offset) {
	uint32_t msr_address;
	uint32_t data[2];

	if (length < sizeof(data))
		return -ENOMEM;
	
	if (buffer == NULL || offset == NULL)
		return -EINVAL;
	
	msr_address = *offset;
	if (!is_valid_msr_address(msr_address))
		return -EINVAL;

	if (copy_from_user(data, buffer, sizeof(data)))
		return -EFAULT;

	pr_info("writing value %llX to %X", *((uint64_t*) data), msr_address);

	wrmsr_safe_on_cpu(0, msr_address, data[0], data[1]);
	
	return sizeof(data);
}

module_init(inteluv_init);
module_exit(inteluv_exit);
