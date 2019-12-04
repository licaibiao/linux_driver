#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#define SENSOR_NAME "awp100"
#define SENSOR_I2C_SLAVE_ADDRESS 0x6C

static struct i2c_client *awp100_i2c_client = NULL;

struct awp100_report_s{
	int32_t pressure;
	int32_t temp;
};

static const unsigned short normal_i2c[2] = {SENSOR_I2C_SLAVE_ADDRESS, I2C_CLIENT_END};

uint8_t awp100_read8(struct i2c_client *client,uint8_t reg)
{
	uint8_t value;
	int ret;
  
	ret = i2c_smbus_read_i2c_block_data(client,reg,1,&value);

	return value;
}

int awp100_read_data(struct i2c_client *client,int32_t * press,int32_t * temp)
{
	uint8_t val[5];
	int ret;

	ret = i2c_smbus_read_i2c_block_data(client,0x06,5,val);

	*press = (val[0] << 16) | (val[1] << 8) | (val[2] << 0);
	*temp = (val[3] << 8) | (val[4] << 0);

	//printk("%d %d \r\n" , *press , *temp);

	return ret;
}

int awp100_write8(struct i2c_client *client,uint8_t reg,uint8_t val)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client,reg,val);

	return ret;
}

static bool awp100_i2c_test(struct i2c_client * client)
{
	uint8_t id;
	
	id = awp100_read8(client,0x01);
	printk("==%s== %x\r\n",__func__,id);

	return true;
}

static int awp100_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	pr_info("%s: addr=0x%x\n",__func__,client->addr);
	strlcpy(info->type, SENSOR_NAME, I2C_NAME_SIZE);
	return 0;
}

static int awp100_init_client(struct i2c_client *client)
{	
	awp100_write8(client,0x30,0x0A);	
	
	printk("==%s== done\r\n",__func__);

	return 0;
}

static int awp100_measure(struct i2c_client *client,struct awp100_report_s *pf)
{
	int32_t press;
	int32_t temp;

	awp100_read_data(client,&press,&temp);
	awp100_write8(client,0x30,0x0A);	

	pf->pressure = press;
	pf->temp = temp;

	return 0;
}

int awp100_open(struct inode *inode, struct file *filp)
{
	printk("==%s==\r\n",__func__);
	if(awp100_i2c_test(awp100_i2c_client) == false){
		return -1;
	}
	awp100_init_client(awp100_i2c_client);
	return 0; 
}

int awp100_release(struct inode *inode, struct file *filp)
{
	printk("==%s==\r\n",__func__);
	return 0;
}

static ssize_t awp100_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	struct awp100_report_s data;
	
	//printk("==%s==\r\n",__func__);
	copy_from_user((void *)&data,buf,sizeof(data));
	awp100_measure(awp100_i2c_client,&data);
	copy_to_user(buf,(void *)&data,sizeof(data));
	
	return 1;
}

static const struct file_operations awp100_fops =
{
	.owner = THIS_MODULE,
	.read = awp100_read,
	.open = awp100_open,
	.release = awp100_release,
};

static int awp100_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	printk("==%s==\r\n",__func__);
	if(awp100_i2c_client == NULL){
		awp100_i2c_client = client;
	}

	if(awp100_i2c_client == NULL){
		printk("==%s== failed\r\n",__func__);
	}

	return 0;
}

static int awp100_remove(struct i2c_client *client)
{
	return 0;
}


static const struct i2c_device_id awp100_id[] = {
	{ SENSOR_NAME, 1 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, awp100_id);

static struct i2c_driver awp100_driver = {
	.class = I2C_CLASS_HWMON,
	.driver = {
		.name	= SENSOR_NAME,
		.owner	= THIS_MODULE,
		//.of_match_table = "allwinner,sun50i-gsensor-para",
	},
	.probe	= awp100_probe,
	.remove	= awp100_remove,
	.id_table = awp100_id,
	.detect = awp100_detect,
	.address_list	= normal_i2c,
};


static int __init awp100_init(void)
{
	int ret = -1;
	int result;
	dev_t devno;
	struct cdev * cdev; 
	struct device *device;
	struct class *cls;

	printk("==%s==\r\n",__func__);

	ret = i2c_add_driver(&awp100_driver);
	if (ret < 0) {
		printk(KERN_INFO "add awp100 i2c driver failed\n");
		return -ENODEV;
	}

	result = alloc_chrdev_region(&devno, 0, 1, "awp100");
	if (result < 0)
		return result;

	cdev = cdev_alloc();  
	cdev_init(cdev, &awp100_fops);
	cdev->owner = THIS_MODULE;
	result = cdev_add(cdev,devno,1);

    cls = class_create(THIS_MODULE, "awp100");
    if(IS_ERR(cls)){
		ret = PTR_ERR(cls);
		printk("==%s== class_create failed:%d\r\n",__func__,ret);
		
    }
	
	device = device_create(cls,NULL,devno,NULL,"awp100");
	if(IS_ERR(device)){
		ret = PTR_ERR(device);
		printk("==%s== device_create failed:%d\r\n",__func__,ret);
	}

	printk("==%s== done:%d\r\n",__func__,result);

	return ret;
}

static void __exit awp100_exit(void)
{
	printk(KERN_INFO "remove awp100 i2c driver.\n");
	i2c_del_driver(&awp100_driver);
}

module_init(awp100_init);
module_exit(awp100_exit);

MODULE_DESCRIPTION("awp100 Barometer Sensor driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");

