#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/init.h>

MODULE_LICENSE("Dual BSD/GPL");

static int __init arduino_driver_init(void);
static void __exit arduino_driver_exit(void);

static int arduino_driver_probe(struct i2c_client*, const struct i2c_device_id*);
static int arduino_driver_remove(struct i2c_client*);

static ssize_t store_arduino(struct device_driver* device, const char* buffer, size_t size);
static ssize_t show_arduino(struct device_driver* device, char* buffer);

static struct i2c_client *arduino_i2c_client = NULL;

static const struct i2c_device_id arduino_device_idtable[] = {
    {"arduino_driver", 0},
    {}
};

MODULE_DEVICE_TABLE(i2c, arduino_device_idtable);

static const struct of_device_id arduino_ids[] = {
    {
        .compatible = "arduino_driver",
    },
    {}
};

static struct i2c_driver i2c_driver = {
    .driver = {
        .name = "arduino_driver",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(arduino_ids),
    },
    .id_table = arduino_device_idtable,
    .probe = arduino_driver_probe,
    .remove = arduino_driver_remove,
};

struct driver_attribute arduino_attribute = {
    .show = show_arduino,
    .store = store_arduino,
    .attr = {
        .name = "arduino",
        .mode = 00666
    }
};

module_init(arduino_driver_init);
module_exit(arduino_driver_exit);

static int __init arduino_driver_init(void)
{
    printk(KERN_ALERT "Initializing Arduino Driver\n");

    i2c_add_driver(&i2c_driver);

    return 0;
}

static void __exit arduino_driver_exit(void)
{
    printk(KERN_ALERT "Exiting Arduino Driver\n");

    i2c_del_driver(&i2c_driver);
}

static int arduino_driver_probe(struct i2c_client* client, const struct i2c_device_id* id)
{
    arduino_i2c_client = client;
    printk(KERN_ALERT "Creating File\n");
    return driver_create_file(&(i2c_driver.driver), &arduino_attribute);
}

static int arduino_driver_remove(struct i2c_client* client)
{
    printk(KERN_ALERT "Deleting File\n");

    driver_remove_file(&(i2c_driver.driver), &arduino_attribute);

    return 0;
}

static ssize_t store_arduino(struct device_driver* device, const char* buffer, size_t size)
{
    if (arduino_i2c_client == NULL)
        return -1;
        
    printk(KERN_ALERT "Writing To I2C\n");
    i2c_master_send(arduino_i2c_client, buffer, size);
    return size;
}

static ssize_t show_arduino(struct device_driver* device, char* buffer)
{
    if (arduino_i2c_client == NULL)
        return -1;
    
    printk(KERN_ALERT "Reading From I2C\n");
    i2c_master_recv(arduino_i2c_client, buffer, 13);
    printk(KERN_ALERT "Read: %s\n", buffer);
    return 0;
}