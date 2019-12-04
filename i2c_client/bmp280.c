#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#define assert(expr)\
	if (!(expr)) {\
		printk(KERN_ERR "Assertion failed! %s,%d,%s,%s\n",\
				__FILE__, __LINE__, __func__, #expr);\
	}

#define SENSOR_NAME "bmp280"
#define SENSOR_I2C_SLAVE_ADDRESS 0x77

#define BMP280_REGISTER_DIG_T1			0x88
#define BMP280_REGISTER_DIG_T2			0x8A
#define BMP280_REGISTER_DIG_T3			0x8C
#define BMP280_REGISTER_DIG_P1			0x8E
#define BMP280_REGISTER_DIG_P2			0x90
#define BMP280_REGISTER_DIG_P3			0x92
#define BMP280_REGISTER_DIG_P4			0x94
#define BMP280_REGISTER_DIG_P5			0x96
#define BMP280_REGISTER_DIG_P6			0x98
#define BMP280_REGISTER_DIG_P7			0x9A
#define BMP280_REGISTER_DIG_P8			0x9C
#define BMP280_REGISTER_DIG_P9			0x9E
#define BMP280_REGISTER_CHIPID			0xD0
#define BMP280_REGISTER_VERSION			0xD1
#define BMP280_REGISTER_SOFTRESET		0xE0
#define BMP280_REGISTER_CAL26			0xE1
#define BMP280_REGISTER_CONTROL			0xF4
#define BMP280_REGISTER_CONFIG			0xF5
#define BMP280_REGISTER_PRESSUREDATA	0xF7
#define BMP280_REGISTER_TEMPDATA		0xFA


#define MODE_CHANGE_DELAY_MS 50


static struct i2c_client *bmp280_i2c_client = NULL;


typedef struct
{
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;
} bmp280_calib_data;

struct bmp280_report_s{
	uint8_t pressure_data[3];
	uint8_t temperature_data[3];
	bmp280_calib_data calib;
};

static bmp280_calib_data bmp280_calib; 

static const unsigned short normal_i2c[2] = {SENSOR_I2C_SLAVE_ADDRESS, I2C_CLIENT_END};

static bool bmp280_i2c_test(struct i2c_client * client)
{
	int retry;

	retry = i2c_smbus_read_byte_data(client,BMP280_REGISTER_CHIPID);
	printk("[test]ID=0x%x\n",retry);

	if(retry == 0x58)
		return true;
	else
		return false;
}

static int bmp280_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	pr_info("%s: addr=0x%x\n",__func__,client->addr);
	strlcpy(info->type, SENSOR_NAME, I2C_NAME_SIZE);
	return 0;
}

uint8_t bmp280_read8(struct i2c_client *client,uint8_t reg)
{
  uint8_t value;
  int ret;
  
  ret = i2c_smbus_read_i2c_block_data(client,reg,1,&value);

  return value;
}

uint16_t bmp280_read16(struct i2c_client *client,uint8_t reg)
{
  uint8_t value[2];
  int ret;

  ret = i2c_smbus_read_i2c_block_data(client,reg,2,value);

  return (value[0] | value[1] << 8);
}


uint16_t bmp280_read16_LE(struct i2c_client *client,uint8_t reg)
{
	return (uint16_t)bmp280_read16(client,reg);
}

int16_t bmp280_readS16_LE(struct i2c_client *client,uint8_t reg)
{
	return (int16_t)bmp280_read16_LE(client,reg);
}


void bmp280_readCoefficients(struct i2c_client *client)
{
    bmp280_calib.dig_T1 = bmp280_read16_LE(client,BMP280_REGISTER_DIG_T1);
    bmp280_calib.dig_T2 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_T2);
    bmp280_calib.dig_T3 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_T3);

    bmp280_calib.dig_P1 = bmp280_read16_LE(client,BMP280_REGISTER_DIG_P1);
    bmp280_calib.dig_P2 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_P2);
    bmp280_calib.dig_P3 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_P3);
    bmp280_calib.dig_P4 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_P4);
    bmp280_calib.dig_P5 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_P5);
    bmp280_calib.dig_P6 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_P6);
    bmp280_calib.dig_P7 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_P7);
    bmp280_calib.dig_P8 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_P8);
    bmp280_calib.dig_P9 = bmp280_readS16_LE(client,BMP280_REGISTER_DIG_P9);

	/*printk("dig_T1:%x(%d) \r\n" , bmp280_calib.dig_T1,bmp280_calib.dig_T1);
	printk("dig_T2:%x(%d) \r\n" , bmp280_calib.dig_T2,bmp280_calib.dig_T2);
	printk("dig_T3:%x(%d) \r\n" , bmp280_calib.dig_T3,bmp280_calib.dig_T3);

	printk("dig_P1:%x(%d) \r\n" , bmp280_calib.dig_P1,bmp280_calib.dig_P1);
	printk("dig_P2:%x(%d) \r\n" , bmp280_calib.dig_P2,bmp280_calib.dig_P2);
	printk("dig_P3:%x(%d) \r\n" , bmp280_calib.dig_P3,bmp280_calib.dig_P3);
	printk("dig_P4:%x(%d) \r\n" , bmp280_calib.dig_P4,bmp280_calib.dig_P4);
	printk("dig_P5:%x(%d) \r\n" , bmp280_calib.dig_P5,bmp280_calib.dig_P5);
	printk("dig_P6:%x(%d) \r\n" , bmp280_calib.dig_P6,bmp280_calib.dig_P6);
	printk("dig_P7:%x(%d) \r\n" , bmp280_calib.dig_P7,bmp280_calib.dig_P7);
	printk("dig_P8:%x(%d) \r\n" , bmp280_calib.dig_P8,bmp280_calib.dig_P8);
	printk("dig_P9:%x(%d) \r\n" , bmp280_calib.dig_P9,bmp280_calib.dig_P9);*/
}

static int bmp280_init_client(struct i2c_client *client)
{
	uint8_t reg_val;
	
	//printk("==%s==\r\n",__func__);
	i2c_smbus_read_i2c_block_data(client,BMP280_REGISTER_CONTROL,1,&reg_val);
	//printk("==%s== control:%x \r\n",__func__,reg_val);
	i2c_smbus_read_i2c_block_data(client,BMP280_REGISTER_CONFIG,1,&reg_val);
	//printk("==%s== config:%x \r\n",__func__,reg_val);

	bmp280_readCoefficients(client);
	i2c_smbus_write_byte_data(client,BMP280_REGISTER_CONTROL,0x3F);
	i2c_smbus_read_i2c_block_data(client,BMP280_REGISTER_CONTROL,1,&reg_val);
	printk("==%s== done:%x\r\n",__func__,reg_val);

	return 0;
}

static int bmp280_measure(struct i2c_client *client,struct bmp280_report_s *pf)
{
	char buf[6];

	pf->calib.dig_T1 = bmp280_calib.dig_T1;
	pf->calib.dig_T2 = bmp280_calib.dig_T2;
	pf->calib.dig_T3 = bmp280_calib.dig_T3;

	pf->calib.dig_P1 = bmp280_calib.dig_P1;
	pf->calib.dig_P2 = bmp280_calib.dig_P2;
	pf->calib.dig_P3 = bmp280_calib.dig_P3;
	pf->calib.dig_P4 = bmp280_calib.dig_P4;
	pf->calib.dig_P5 = bmp280_calib.dig_P5;
	pf->calib.dig_P6 = bmp280_calib.dig_P6;
	pf->calib.dig_P7 = bmp280_calib.dig_P7;
	pf->calib.dig_P8 = bmp280_calib.dig_P8;
	pf->calib.dig_P9 = bmp280_calib.dig_P9;

	i2c_smbus_read_i2c_block_data(client,BMP280_REGISTER_PRESSUREDATA,6,buf);

	memcpy(pf->pressure_data,&buf[0],3);
	memcpy(pf->temperature_data,&buf[3],3);

	return 0;
}

int bmp280_open(struct inode *inode, struct file *filp)
{
	printk("==%s==\r\n",__func__);
	if(bmp280_i2c_test(bmp280_i2c_client) == false){
		return -1;
	}
	bmp280_init_client(bmp280_i2c_client);
	return 0; 
}

int bmp280_release(struct inode *inode, struct file *filp)
{
	printk("==%s==\r\n",__func__);
	return 0;
}

static ssize_t bmp280_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	struct bmp280_report_s data;
	//printk("==%s==\r\n",__func__);
	bmp280_measure(bmp280_i2c_client,&data);
	copy_to_user(buf,(void *)&data,sizeof(data));
	return 1;
}

static const struct file_operations bmp280_fops =
{
	.owner = THIS_MODULE,
	.read = bmp280_read,
	.open = bmp280_open,
	.release = bmp280_release,
};

static int bmp280_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	printk("==%s==\r\n",__func__);
	if(bmp280_i2c_client == NULL){
		bmp280_i2c_client = client;
	}

	if(bmp280_i2c_client == NULL){
		printk("==%s== failed\r\n",__func__);
	}

	return 0;
}

static int bmp280_remove(struct i2c_client *client)
{
	return 0;
}


static const struct i2c_device_id bmp280_id[] = {
	{ SENSOR_NAME, 1 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, bmp280_id);

static struct i2c_driver bmp280_driver = {
	.class = I2C_CLASS_HWMON,
	.driver = {
		.name	= SENSOR_NAME,
		.owner	= THIS_MODULE,
		//.of_match_table = "allwinner,sun50i-gsensor-para",
	},
	.probe	= bmp280_probe,
	.remove	= bmp280_remove,
	.id_table = bmp280_id,
	.detect = bmp280_detect,
	.address_list	= normal_i2c,
};


static int __init bmp280_init(void)
{
	int ret = -1;
	int result;
	dev_t devno;
	struct cdev * cdev; 
	struct device *device;
	struct class *cls;

	printk("==%s==\r\n",__func__);

	ret = i2c_add_driver(&bmp280_driver);
	if (ret < 0) {
		printk(KERN_INFO "add bmp280 i2c driver failed\n");
		return -ENODEV;
	}

	result = alloc_chrdev_region(&devno, 0, 1, "bmp280");
	if (result < 0)
		return result;

	cdev = cdev_alloc();  
	cdev_init(cdev, &bmp280_fops);
	cdev->owner = THIS_MODULE;
	result = cdev_add(cdev,devno,1);

    cls = class_create(THIS_MODULE, "bmp280");
    if(IS_ERR(cls)){
		ret = PTR_ERR(cls);
		printk("==%s== class_create failed:%d\r\n",__func__,ret);
		
    }
	
	device = device_create(cls,NULL,devno,NULL,"bmp280");
	if(IS_ERR(device)){
		ret = PTR_ERR(device);
		printk("==%s== device_create failed:%d\r\n",__func__,ret);
	}

	printk("==%s== done:%d\r\n",__func__,result);

	return ret;
}

static void __exit bmp280_exit(void)
{
	printk(KERN_INFO "remove bmp280 i2c driver.\n");
	i2c_del_driver(&bmp280_driver);
}

module_init(bmp280_init);
module_exit(bmp280_exit);

MODULE_DESCRIPTION("bmp280 3-Axis Orientation/Motion Detection Sensor driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");

