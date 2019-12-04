#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include "hiir.h"
#include "hiir_codedef.h"

#define REMOTE_KEY0       0xff00ff00
#define REMOTE_KEY1       0xfa05ff00
#define REMOTE_KEY2       0xf30cff00
#define REMOTE_KEY3       0xf10eff00
#define REMOTE_KEY4       0xfd02ff00
#define REMOTE_KEY5       0xfe01ff00
#define REMOTE_KEY6       0xf609ff00
#define REMOTE_KEY7       0xf20dff00
#define REMOTE_KEY8       0xf609ff00
#define REMOTE_KEY9       0xf20dff00

typedef unsigned short int U8;

typedef struct
{
    char *name;
    unsigned int irkey_value;
}IRKEY_ADAPT;


static IRKEY_ADAPT g_irkey_adapt_array[] =
{
    /*irkey_name*/ /*irkey_value*/
    {"REMOTE_KEY0   ", REMOTE_KEY0,/*1*/         },
    {"REMOTE_KEY1   ", REMOTE_KEY1,/*2*/         },
    {"REMOTE_KEY2   ", REMOTE_KEY2,/*3*/         },
    {"REMOTE_KEY3   ", REMOTE_KEY3,/*4*/         },
    {"REMOTE_KEY4   ", REMOTE_KEY4,/*5*/         },
    {"REMOTE_KEY5   ", REMOTE_KEY5,/*6*/         },
    {"REMOTE_KEY6   ", REMOTE_KEY6,/*7*/         },
    {"REMOTE_KEY7   ", REMOTE_KEY7,/*8*/         },
    {"REMOTE_KEY8   ", REMOTE_KEY8,/*9*/         },
    {"REMOTE_KEY9   ", REMOTE_KEY9,/*-/--*/      },
};


static int g_irkey_adapt_count = sizeof(g_irkey_adapt_array) / sizeof(IRKEY_ADAPT);
static int key0_down = 0;
static int key1_down = 0;
static int key2_down = 0;
static int key3_down = 0;
static int key4_down = 0;
static int key5_down = 0;
static int key6_down = 0;
static int key7_down = 0;
static int key8_down = 0;
static int key9_down = 0;


static void huawei_report_irkey(irkey_info_s rcv_irkey_info)
{
    int i = 0;
    for(i = 0; i<g_irkey_adapt_count; i++)
    {
        if( (rcv_irkey_info.irkey_datah == 0) &&
            (rcv_irkey_info.irkey_datal == g_irkey_adapt_array[i].irkey_value) )
        {
        	printf("keyvalue=H/L 0x%x/0x%x\n",rcv_irkey_info.irkey_datah,rcv_irkey_info.irkey_datal);
            break;
        }
    }
    if(i>=g_irkey_adapt_count)
    {
        printf("Error. get a invalid code. irkey_datah=0x%.8x,irkey_datal=0x%.8x.\n", 
               (int)rcv_irkey_info.irkey_datah, (int)rcv_irkey_info.irkey_datal);
    }
    else
    {
        printf("RECEIVE ---> %s\t", g_irkey_adapt_array[i].name);
        if(rcv_irkey_info.irkey_state_code == 1)
        {
            printf("KEYUP...");
        }
        printf("\n");
        
        if((rcv_irkey_info.irkey_datah == 0) && 
           (rcv_irkey_info.irkey_state_code == 0) &&
           (rcv_irkey_info.irkey_datal == REMOTE_KEY0)) /*POWER*/
        {
            key0_down = 1;
        }
        else if((rcv_irkey_info.irkey_datah == 0) && 
                (rcv_irkey_info.irkey_state_code == 0) &&
                (rcv_irkey_info.irkey_datal == REMOTE_KEY1)) /*MUTE*/
        {
            key1_down = 1;
        }
        else if((rcv_irkey_info.irkey_datah == 0) && 
                (rcv_irkey_info.irkey_state_code == 0) &&
                (rcv_irkey_info.irkey_datal == REMOTE_KEY2)) /*F1*/
        {
            key2_down = 1;
        }
        else if((rcv_irkey_info.irkey_datah == 0) && 
                (rcv_irkey_info.irkey_state_code == 0) &&
                (rcv_irkey_info.irkey_datal == REMOTE_KEY3)) /*F2*/
        {
            key3_down = 1;
        }
        else if((rcv_irkey_info.irkey_datah == 0) && 
                (rcv_irkey_info.irkey_state_code == 0) &&
                (rcv_irkey_info.irkey_datal == REMOTE_KEY4)) /*F3*/
        {
            key4_down = 1;
        }
        else if((rcv_irkey_info.irkey_datah == 0) && 
                (rcv_irkey_info.irkey_state_code == 0) &&
                (rcv_irkey_info.irkey_datal == REMOTE_KEY5)) /*F4*/
        {
            key5_down = 1;
        }
    }
}

void Hi_IR_FUNC_TEST_001()
{
    int fp, res, i, count;
    int delay = 0;
    irkey_info_s rcv_irkey_info[4];
    
    key0_down = 0;
    key1_down = 0;
    key2_down = 0;
    key3_down = 0;
    key4_down = 0;
    key5_down = 0;
	key6_down = 0;
	key7_down = 0;
	key8_down = 0;
	key9_down = 0;

    printf("Hi_IR_FUNC_TEST_001 start...\n");
    printf("REMOTE codetype ...NEC with simple repeat code - uPD6121G\n");
    
    if( -1 == (fp = open("/dev/"HIIR_DEVICE_NAME, O_RDWR) ) )
    {
        printf("ERROR:can not open %s device. read return %d\n", HIIR_DEVICE_NAME, fp);
        return;
    }

    ioctl(fp, IR_IOC_SET_ENABLE_KEYUP, 1);

    printf("REMOTE_POWER key to finish the test...\n");
    printf("REMOTE_MUTE  key to set repkey delay time...\n");
    while(1)
    {
        res = read(fp, rcv_irkey_info, sizeof(rcv_irkey_info));
        count = res / sizeof(irkey_info_s);
        if( (res > 0) && (res<=sizeof(rcv_irkey_info)) )
        {
            for(i=0;i<count;i++)
            {
                huawei_report_irkey(rcv_irkey_info[i]);
            }
        }
        else
        {
            printf("Hi_IR_FUNC_TEST_001 Error. read irkey device error. result=%d.\n", res);
        }
    }

    close(fp);
}

int  main(void)
{
	Hi_IR_FUNC_TEST_001();
}
