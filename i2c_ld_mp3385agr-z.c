#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/delay.h>

/* Register */
#define REG_00 0x00
#define REG_01 0x01
#define REG_05 0x05

/* Mode */
#define AN_DIM_MODE 0xf1 /* Analog dimming mode */
#define OC_3A 0x01       /* Set Over Current to 3A */
#define REC_MODE 0x05    /* Recoverable mode */

struct mp3385agrz {
	u32 addr;
    struct i2c_client *client;
};

static int mp3385agrz_write_regs(struct mp3385agrz *mp3385agrz, u8 reg, u8 val)
{
    int ret;

    ret = i2c_smbus_write_byte_data(mp3385agrz->client, reg, val);
    if (ret) {
		dev_err(&mp3385agrz->client->dev,
			"register write to 0x%02X failed (error %d)",
			reg, ret);
	}

    return ret;
}


static int mp3385agrz_init_regs(struct mp3385agrz *mp3385agrz)
{
    int ret = 0;
    /* Set - Analog Dimming mode to reduce glitch */
	ret = mp3385agrz_write_regs(mp3385agrz, REG_00, AN_DIM_MODE);
    /* Set - Over current to 3A */
    ret = mp3385agrz_write_regs(mp3385agrz, REG_05, OC_3A);
    /* Set - Recoverable mode current limit */
    ret = mp3385agrz_write_regs(mp3385agrz, REG_01, REC_MODE);

    return ret;
}

static const struct i2c_device_id mp3385agrz_id[] = {
	{ "mp3385agrz", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, adc128_id);

#if defined(CONFIG_OF)
static const struct of_device_id mp3385agrz_of_match[] = {
	{ .compatible = "mp3385agrz", },
	{},
};
MODULE_DEVICE_TABLE(of, mp3385agrz_of_match);
#endif

static int mp3385agrz_i2c_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
    struct device *dev = &client->dev;
    struct device *mp3385agrz_dev;
    struct mp3385agrz *mp3385agrz;
    int ret;
    u32 reg;
    static u8 test;

    mp3385agrz = devm_kzalloc(dev, sizeof(struct mp3385agrz), GFP_KERNEL);
	if (!mp3385agrz)
		return -ENOMEM;

    ret = of_property_read_u32(dev->of_node, "reg", &reg);
    printk("KERN_ERR, DEBUG FUNC = %s, LINE = %d, reg = %x \n", __func__, __LINE__, reg);
    if(ret){
        printk("KERN_ERR, Can't read reg property \n");
    }

    mp3385agrz->addr = reg;
    mp3385agrz->client = client;

    i2c_set_clientdata(mp3385agrz->client, mp3385agrz);

    ret = mp3385agrz_init_regs(mp3385agrz);

    return 0;
}


static int mp3385agrz_i2c_remove(struct i2c_client *client)
{
	struct mp3385agrz *mp3385agrz = i2c_get_clientdata(client);



	return 0;
}


/* machine i2c codec control layer */
static struct i2c_driver mp3385agrz_i2c_led_driver = {
	.driver = {
		.name = "mp3385agrz_led_driver",
		.of_match_table = of_match_ptr(mp3385agrz_of_match),
	},
	.probe	= mp3385agrz_i2c_probe,
    .id_table	= mp3385agrz_id,
};


static int __init mp3385agrz_init(void)
{
	return i2c_add_driver(&mp3385agrz_i2c_led_driver);
}


static void __exit mp3385agrz_exit(void)
{
	i2c_del_driver(&mp3385agrz_i2c_led_driver);
}

late_initcall(mp3385agrz_init);
module_exit(mp3385agrz_exit);
// module_i2c_driver(mp3385agrz_i2c_led_driver);
MODULE_DESCRIPTION("MP3385AGR-Z Led Driver");
MODULE_AUTHOR("Tommaso Merciai");
MODULE_LICENSE("GPL");
