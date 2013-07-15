
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/if.h>
#include <linux/netdevice.h>
#include <linux/cache.h>
#include <linux/fs.h>

#define KernPrint	printk
#define NVRAMDRV_MAJOR	124
#define IGW_DECLARE_RWLOCK_EXTERN(l) extern rwlock_t l
IGW_DECLARE_RWLOCK_EXTERN(nvram_software_lock);
IGW_DECLARE_RWLOCK_EXTERN(nvram_isecpdri_lock);

MODULE_LICENSE("GPL");

ssize_t nvramdrv_ioctl(struct inode * inode, struct file * file,
	unsigned int cmd, unsigned long buf)
{
    int temp = 0;
    return temp;
}

int nvramdrv_open(struct inode *inode, struct file *filp)
{
    return 0;
}

int nvramdrv_release(struct inode *inode, struct file *filp)
{
    return 0;
}


struct file_operations nvramdrv_fops = {
    .unlocked_ioctl = nvramdrv_ioctl,
    .open = nvramdrv_open,
    .release = nvramdrv_release,
};

int __init nvramdrv_init(void)
{
    int result;

    result = register_chrdev(NVRAMDRV_MAJOR, "nvramdrv", &nvramdrv_fops);
    if (result < 0)
    {
        KernPrint(KERN_CRIT "nvramdrv: can't get major 124\n");
        return result;
    }

    printk("nvramdrv install successfully\r\n");
    return result;
}


void __exit nvramdrv_exit(void)
{

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
      if (unregister_chrdev(NVRAMDRV_MAJOR,"nvramdrv")<0)
      {
        KernPrint("<1>isecpdri: cleanup module failed\n");
      }
#else
        unregister_chrdev(NVRAMDRV_MAJOR,"nvramdrv");
#endif
}

module_init(nvramdrv_init);
module_exit(nvramdrv_exit);
