#include <common.h>
#include <command.h>
#include <asm/io.h>


#define DEVICE_ID_REG   0x44E10600

/* GPIO addresses for AM335x */
#define GPIO1_BASE 0x4804C000
#define GPIO_OE 0x134          /* Output Enable Register */
#define GPIO_SETDATAOUT 0x194  /* Set Data Output Register */
#define GPIO_CLEARDATAOUT 0x190 /* Clear Data Output Register */

/* BBB LED GPIO pins */
#define USR0_LED_BIT  (1<<21)  /* GPIO1_21 - USR0 LED */
#define USR1_LED_BIT  (1<<22)  /* GPIO1_22 - USR1 LED */
#define USR2_LED_BIT  (1<<23)  /* GPIO1_23 - USR2 LED */
#define USR3_LED_BIT  (1<<24)  /* GPIO1_24 - USR3 LED */

static void gpio_init_led(unsigned int led_bit)
{
    unsigned int reg_val;
    
    /* Set GPIO pin as output by clearing the OE bit (0 = output, 1 = input) */
    reg_val = readl((void *)(GPIO1_BASE + GPIO_OE));
    reg_val &= ~led_bit;  /* Clear the bit to set as output */
    writel(reg_val, (void *)(GPIO1_BASE + GPIO_OE));
}

static int do_bbb_info(struct cmd_tbl *cmdtp, int flag, int argc,
               char *const argv[])
{
    unsigned int dev_id;

    printf("BBB Info: This is a BeagleBone Black board running U-Boot!\n");
    dev_id = readl((void *)DEVICE_ID_REG);
    printf("Device ID Register: 0x%08X\n", dev_id);

    if(argc > 1)
    {
        if(strcmp(argv[1], "led_on") == 0)
        {
            /* Initialize GPIO1_23 as output */
            gpio_init_led(USR2_LED_BIT);
            
            /* Turn on USR2 LED */
            writel(USR2_LED_BIT, (void *)(GPIO1_BASE + GPIO_SETDATAOUT));
            printf("USR2 LED turned ON\n");
        }
        else if(strcmp(argv[1], "led_off") == 0)
        {
            /* Turn off USR2 LED */
            writel(USR2_LED_BIT, (void *)(GPIO1_BASE + GPIO_CLEARDATAOUT));
            printf("USR2 LED turned OFF\n");
        }
        else if(strcmp(argv[1], "led_all_on") == 0)
        {
            /* Turn on all user LEDs */
            gpio_init_led(USR0_LED_BIT);
            gpio_init_led(USR1_LED_BIT);
            gpio_init_led(USR2_LED_BIT);
            gpio_init_led(USR3_LED_BIT);
            
            unsigned int all_leds = USR0_LED_BIT | USR1_LED_BIT | USR2_LED_BIT | USR3_LED_BIT;
            writel(all_leds, (void *)(GPIO1_BASE + GPIO_SETDATAOUT));
            printf("All user LEDs turned ON\n");
        }
        else if(strcmp(argv[1], "led_all_off") == 0)
        {
            /* Turn off all user LEDs */
            unsigned int all_leds = USR0_LED_BIT | USR1_LED_BIT | USR2_LED_BIT | USR3_LED_BIT;
            writel(all_leds, (void *)(GPIO1_BASE + GPIO_CLEARDATAOUT));
            printf("All user LEDs turned OFF\n");
        }
        else
        {
            printf("Unknown argument: %s\n", argv[1]);
            printf("Usage: bbb_info [led_on|led_off|led_all_on|led_all_off]\n");
            return CMD_RET_USAGE;
        }
    }
    return 0;
}


U_BOOT_CMD(
    bbb_info,   CONFIG_SYS_MAXARGS,  1,  do_bbb_info,
    "Display BeagleBone Black specific information and control LEDs",
    "bbb_info [args...]\n"
    "    - Displays information about the BeagleBone Black board device ID.\n"
    "    - led_on     : Turn on USR2 LED\n"
    "    - led_off    : Turn off USR2 LED\n"
    "    - led_all_on : Turn on all USR LEDs (USR0, USR1, USR2, USR3)\n"
    "    - led_all_off: Turn off all USR LEDs"
);