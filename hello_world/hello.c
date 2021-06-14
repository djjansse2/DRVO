#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define MAX_DEVICES 1

MODULE_LICENSE("Dual BSD/GPL");

static int hello_open(struct inode *inode, struct file *file);
static int hello_release(struct inode *inode, struct file *file);
static ssize_t hello_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t hello_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = hello_read,
    .write = hello_write,
    .open = hello_open,
    .release = hello_release
};

struct hello_device_data {
    struct cdev cdev;
};

static int dev_major = 0;
static struct class *hellodev_class = NULL;
static struct hello_device_data hellodev_data[MAX_DEVICES];

static int hellodev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init hello_init(void)
{
    int err, i;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, MAX_DEVICES, "hellodev");

    dev_major = MAJOR(dev);

    hellodev_class = class_create(THIS_MODULE, "hellodev");
    hellodev_class->dev_uevent = hellodev_uevent;

    for(i = 0; i < MAX_DEVICES; i++)
    {
        cdev_init(&hellodev_data[i].cdev, &fops);
        hellodev_data[i].cdev.owner = THIS_MODULE;

        cdev_add(&hellodev_data[i].cdev, MKDEV(dev_major, i), 1);

        device_create(hellodev_class, NULL, MKDEV(dev_major, i), NULL, "hellodev-%d", i);
    }

    return 0;
}

static void __exit hello_exit(void)
{
    int i;
    for (i = 0; i < MAX_DEVICES; i++)
    {
        device_destroy(hellodev_class, MKDEV(dev_major, i));
    }

    class_unregister(hellodev_class);
    class_destroy(hellodev_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
}

static int hello_open(struct inode *inode, struct file *file)
{
    printk("HELLO: Device open\n");
    return 0;
}

static int hello_release(struct inode *inode, struct file *file)
{
    printk("HELLO: Device close\n");
    return 0;
}

static ssize_t hello_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    uint8_t *data = "Hello from the kernel world!\n";
    size_t datalen = strlen(data);

    printk("Reading device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count > datalen)
    {
        count = datalen;
    }

    if (copy_to_user(buf, data, count))
    {
        return -EFAULT;
    }

    return count;
}

static ssize_t hello_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    size_t maxdatalen = 30, ncopied;
    uint8_t databuf[maxdatalen];

    printk("Writing device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count < maxdatalen)
    {
        maxdatalen = count;
    }

    ncopied = copy_from_user(databuf, buf, maxdatalen);

    if (ncopied == 0)
    {
        printk("Copied %zd bytes from the user\n", maxdatalen);
    }
    else
    {
        printk("Couldn't copy %zd bytes from the user\n", ncopied);
    }

    databuf[maxdatalen] = 0;

    printk("Data from the user %s\n", databuf);

    return count;
}

module_init(hello_init);
module_exit(hello_exit);