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
    .show = NULL,
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
    u32 buffer = 0x00;

    printk(KERN_ALERT "Initializing Arduino Driver\n");

    struct i2c_adapter* arduino_i2c_adapter = i2c_get_adapter(2);

    struct device_node* i2c2_node = arduino_i2c_adapter->dev.of_node;
    struct device_node* arduino_node = of_find_node_by_name(i2c2_node, "arduino_driver");

    printk(KERN_ALERT "%u\n", i2c2_node);

    of_property_read_u32(arduino_node, "i2c-address", &buffer);

    struct i2c_board_info arduino_i2c_board_info = {
        .type = "arduino_driver",
        .addr = buffer,
    };

    i2c_new_device(arduino_i2c_adapter, &arduino_i2c_board_info);
    i2c_add_driver(&i2c_driver);

    return 0;
}

static void __exit arduino_driver_exit(void)
{
    printk(KERN_ALERT "Exiting Arduino Driver\n");

    i2c_del_driver(&i2c_driver);
    i2c_unregister_device(arduino_i2c_client);
}

static int arduino_driver_probe(struct i2c_client* client, const struct i2c_device_id* id)
{
    printk(KERN_ALERT "Creating File\n");

    driver_create_file(&(i2c_driver.driver), &arduino_attribute);
    
    return 0;
}

static int arduino_driver_remove(struct i2c_client* client)
{
    printk(KERN_ALERT "Deleting File\n");

    driver_remove_file(&(i2c_driver.driver), &arduino_attribute);

    return 0;
}

static ssize_t store_arduino(struct device_driver* device, const char* buffer, size_t size)
{
    printk(KERN_ALERT "Writing To I2C\n");
    i2c_master_send(arduino_i2c_client, buffer, size);
    return size;
}