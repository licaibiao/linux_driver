#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#define SENSOR_NAME "fbm320"
#define SENSOR_I2C_SLAVE_ADDRESS 0x6D

static struct i2c_client *fbm320_i2c_client = NULL;

typedef struct
{
	uint16_t C0, C1, C2, C3, C6, C8, C9, C10, C11, C12; 
	uint32_t C4, C5, C7;
} fbm320_calib_data;

struct fbm320_report_s{
	uint8_t mode;
	int32_t pressure;
	int32_t temp;
	fbm320_calib_data calib;
};

static fbm320_calib_data fbm320_calib; 

static const unsigned short normal_i2c[2] = {SENSOR_I2C_SLAVE_ADDRESS, I2C_CLIENT_END};

uint8_t fbm320_read8(struct i2c_client *client,uint8_t reg)
{
	uint8_t value;
	int ret;
  
	ret = i2c_smbus_read_i2c_block_data(client,reg,1,&value);

	return value;
}

uint32_t fbm320_read32_data(struct i2c_client *client)
{
	uint8_t value[3];
	int ret;

	ret = i2c_smbus_read_i2c_block_data(client,0xF6,3,value);

	return ((uint32_t)value[0] << 16) | ((uint32_t)value[1] << 8) | (uint32_t)value[2];
}

int fbm320_write8(struct i2c_client *client,uint8_t reg,uint8_t val)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client,reg,val);

	return ret;
}

static bool fbm320_i2c_test(struct i2c_client * client)
{
	uint8_t id;
	
	id = fbm320_read8(client,0x6B);
	printk("==%s== %x\r\n",__func__,id);

	return true;
}

static int fbm320_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	pr_info("%s: addr=0x%x\n",__func__,client->addr);
	strlcpy(info->type, SENSOR_NAME, I2C_NAME_SIZE);
	return 0;
}


void fbm320_readCoefficients(struct i2c_client *client)
{
	uint8_t i;
	uint16_t R[10]={0};
	
	for(i=0; i<9; i++)
		R[i] = ((uint8_t)fbm320_read8(client, 0xAA + (i*2)) << 8) | fbm320_read8(client, 0xAB + (i*2));

	R[9] = ((uint8_t)fbm320_read8(client, 0xA4) << 8) | fbm320_read8(client, 0xF1);
	
	
	/*	Use R0~R9 calculate C0~C12 of FBM320-02	*/
	fbm320_calib.C0 = R[0] >> 4;
	fbm320_calib.C1 = ((R[1] & 0xFF00) >> 5) | (R[2] & 7);
	fbm320_calib.C2 = ((R[1] & 0xFF) << 1) | (R[4] & 1);
	fbm320_calib.C3 = R[2] >> 3;
	fbm320_calib.C4 = ((uint32_t)R[3] << 2) | (R[0] & 3);
	fbm320_calib.C5 = R[4] >> 1;
	fbm320_calib.C6 = R[5] >> 3;
	fbm320_calib.C7 = ((uint32_t)R[6] << 3) | (R[5] & 7);
	fbm320_calib.C8 = R[7] >> 3;
	fbm320_calib.C9 = R[8] >> 2;
	fbm320_calib.C10 = ((R[9] & 0xFF00) >> 6) | (R[8] & 3);
	fbm320_calib.C11 = R[9] & 0xFF;
	fbm320_calib.C12 = ((R[0] & 0x0C) << 1) | (R[7] & 7);

#if 0
	printk("C0:%x(%d) \r\n" , fbm320_calib.C0,fbm320_calib.C0);
	printk("C1:%x(%d) \r\n" , fbm320_calib.C1,fbm320_calib.C1);
	printk("C2:%x(%d) \r\n" , fbm320_calib.C2,fbm320_calib.C2);
	printk("C3:%x(%d) \r\n" , fbm320_calib.C3,fbm320_calib.C3);
	printk("C4:%x(%d) \r\n" , fbm320_calib.C4,fbm320_calib.C4);
	printk("C5:%x(%d) \r\n" , fbm320_calib.C5,fbm320_calib.C5);
	printk("C6:%x(%d) \r\n" , fbm320_calib.C6,fbm320_calib.C6);
	printk("C7:%x(%d) \r\n" , fbm320_calib.C7,fbm320_calib.C7);
	printk("C8:%x(%d) \r\n" , fbm320_calib.C8,fbm320_calib.C8);
	printk("C9:%x(%d) \r\n" , fbm320_calib.C9,fbm320_calib.C9);
	printk("C10:%x(%d) \r\n" , fbm320_calib.C10,fbm320_calib.C10);
	printk("C11:%x(%d) \r\n" , fbm320_calib.C11,fbm320_calib.C11);
	printk("C12:%x(%d) \r\n" , fbm320_calib.C12,fbm320_calib.C12);
#endif
}

static int fbm320_init_client(struct i2c_client *client)
{	
	fbm320_readCoefficients(client);
	fbm320_write8(client,0xF4,0x2E);	
	
	printk("==%s== done\r\n",__func__);

	return 0;
}

static int fbm320_measure(struct i2c_client *client,struct fbm320_report_s *pf)
{
	pf->calib.C0 = fbm320_calib.C0;
	pf->calib.C1 = fbm320_calib.C1;
	pf->calib.C2 = fbm320_calib.C2;
	pf->calib.C3 = fbm320_calib.C3;
	pf->calib.C4 = fbm320_calib.C4;
	pf->calib.C5 = fbm320_calib.C5;
	pf->calib.C6 = fbm320_calib.C6;
	pf->calib.C7 = fbm320_calib.C7;
	pf->calib.C8 = fbm320_calib.C8;
	pf->calib.C9 = fbm320_calib.C9;
	pf->calib.C10 = fbm320_calib.C10;
	pf->calib.C11 = fbm320_calib.C11;
	pf->calib.C12 = fbm320_calib.C12;

	if(pf->mode){
		pf->pressure = fbm320_read32_data(client);
		fbm320_write8(client,0xF4,0x2E);	
		//printk("fbm320_measure pressure:%d \r\n",pf->pressure);
	}else{
		pf->temp = fbm320_read32_data(client);
		fbm320_write8(client,0xF4,0xF4);	
		//printk("fbm320_measure temp:%d \r\n",pf->temp);
	}

	return 0;
}

int fbm320_open(struct inode *inode, struct file *filp)
{
	printk("==%s==\r\n",__func__);
	if(fbm320_i2c_test(fbm320_i2c_client) == false){
		return -1;
	}
	fbm320_init_client(fbm320_i2c_client);
	return 0; 
}

int fbm320_release(struct inode *inode, struct file *filp)
{
	printk("==%s==\r\n",__func__);
	return 0;
}

static ssize_t fbm320_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	struct fbm320_report_s data;
	//printk("==%s==\r\n",__func__);
	copy_from_user((void *)&data,buf,sizeof(data));
	fbm320_measure(fbm320_i2c_client,&data);
	copy_to_user(buf,(void *)&data,sizeof(data));
	return 1;
}

static const struct file_operations fbm320_fops =
{
	.owner = THIS_MODULE,
	.read = fbm320_read,
	.open = fbm320_open,
	.release = fbm320_release,
};

static int fbm320_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	printk("==%s==\r\n",__func__);
	if(fbm320_i2c_client == NULL){
		fbm320_i2c_client = client;
	}

	if(fbm320_i2c_client == NULL){
		printk("==%s== failed\r\n",__func__);
	}

	return 0;
}

static int fbm320_remove(struct i2c_client *client)
{
	return 0;
}


static const struct i2c_device_id fbm320_id[] = {
	{ SENSOR_NAME, 1 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, fbm320_id);

static struct i2c_driver fbm320_driver = {
	.class = I2C_CLASS_HWMON,
	.driver = {
		.name	= SENSOR_NAME,
		.owner	= THIS_MODULE,
		//.of_match_table = "allwinner,sun50i-gsensor-para",
	},
	.probe	= fbm320_probe,
	.remove	= fbm320_remove,
	.id_table = fbm320_id,
	.detect = fbm320_detect,
	.address_list	= normal_i2c,
};


static int __init fbm320_init(void)
{
	int ret = -1;
	int result;
	dev_t devno;
	struct cdev * cdev; 
	struct device *device;
	struct class *cls;

	printk("==%s==\r\n",__func__);

	ret = i2c_add_driver(&fbm320_driver);
	if (ret < 0) {
		printk(KERN_INFO "add fbm320 i2c driver failed\n");
		return -ENODEV;
	}

	result = alloc_chrdev_region(&devno, 0, 1, "fbm320");
	if (result < 0)
		return result;

	cdev = cdev_alloc();  
	cdev_init(cdev, &fbm320_fops);
	cdev->owner = THIS_MODULE;
	result = cdev_add(cdev,devno,1);

    cls = class_create(THIS_MODULE, "fbm320");
    if(IS_ERR(cls)){
		ret = PTR_ERR(cls);
		printk("==%s== class_create failed:%d\r\n",__func__,ret);
		
    }
	
	device = device_create(cls,NULL,devno,NULL,"fbm320");
	if(IS_ERR(device)){
		ret = PTR_ERR(device);
		printk("==%s== device_create failed:%d\r\n",__func__,ret);
	}

	printk("==%s== done:%d\r\n",__func__,result);

	return ret;
}

static void __exit fbm320_exit(void)
{
	printk(KERN_INFO "remove fbm320 i2c driver.\n");
	i2c_del_driver(&fbm320_driver);
}

module_init(fbm320_init);
module_exit(fbm320_exit);

MODULE_DESCRIPTION("fbm320 3-Axis Orientation/Motion Detection Sensor driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");

