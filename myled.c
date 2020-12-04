// SPDX-License-Identifier: GPL-3.0
/*
 *(c) 2020 Ina Syuichiro and Ueda Ryuichi
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>

MODULE_AUTHOR("Ina Syuichiro and Ueda Ryuichi");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;
static volatile u32 *gpio_base_2 = NULL;

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
        int i, cycle_count;
        int time = 2000;
        int split_time = 10; //分割時間
        int Lit_time = 0, Off_time; //点灯時間　消灯時間
        int duty_ratio = 0; // デューティ比
        char c;
        if(copy_from_user(&c, buf, sizeof(char)))
                return -EFAULT;

//      printk(KERN_INFO "receive %c\n", c);

        //gpio_base は赤色のLED
        //gpio_base_2は黄色のLED
    
        if(c == '0'){ //両方の明かりを消す
                gpio_base[10] = 1 << 25;
                gpio_base_2[10] = 1 << 18;
        }else if(c == '1'){ //両方の明かりをつける
                gpio_base[7] = 1 << 25;
                gpio_base_2[7] = 1 << 18;
        }else if(c == '2'){
                cycle_count = time/split_time;
                // 1秒で点灯して、1秒で消灯する
                for(i=0;i<3;i++){
                        for(;duty_ratio <=100;){
                                Lit_time = (split_time*duty_ratio)/100;
                                Off_time = split_time - Lit_time;
                                gpio_base[7] = 1 << 25;
                                gpio_base_2[10] = 1 << 18;
                                mdelay(Lit_time);
                                gpio_base[10] = 1 << 25;
                                gpio_base_2[7] = 1 << 18;
                                mdelay(Off_time);
                                duty_ratio++;
                        }
                        for(;duty_ratio >= 0;){
                                Lit_time = (split_time*duty_ratio)/100;
                                Off_time = split_time - Lit_time;
                                gpio_base[7] = 1 << 25;
                                gpio_base_2[10] = 1 << 18;
                                mdelay(Lit_time);
                                gpio_base[10] = 1 << 25;
                                gpio_base_2[7] = 1 << 18;
                                mdelay(Off_time);
                                duty_ratio--;
                        }
                }
                gpio_base_2[10] = 1 << 18;
        }

        return 1;
}
static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
        int size = 0;
        char sushi[] = {0xF0,0x9F,0x8D,0xA3,0x0A};
        if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){
                printk(KERN_ERR "sushi : copy_to_user failed\n");
                return -EFAULT;
        }
        size += sizeof(sushi);
        return size;
}


static struct file_operations led_fops = {
        .owner = THIS_MODULE,
        .write = led_write,
        .read = sushi_read
};

static int __init init_mod(void)
{
        int retval;

        retval = alloc_chrdev_region(&dev, 0, 1, "myled");
        if(retval < 0){
                printk(KERN_INFO "alloc_chrdev_region failed.\n");
                return retval;
        }
        printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));

        cdev_init(&cdv, &led_fops);
        retval = cdev_add(&cdv, dev, 1);
        if(retval < 0){
                printk(KERN_ERR "cdev_add failed. major:%d, minor:%d\n",MAJOR(dev),MINOR(dev));
                                return retval;
        }

        cls = class_create(THIS_MODULE, "myled");
        if(IS_ERR(cls)){
                printk(KERN_ERR "class_create failed.");
                return PTR_ERR(cls);
        }
        device_create(cls, NULL, dev, NULL, "myled%d",MINOR(dev));

        gpio_base = ioremap_nocache(0xfe200000,0xA0);
        gpio_base_2 = ioremap_nocache(0xfe200000,0xA0);

        //GPIO 25ピンの出力設定
        const u32 led = 25;
        const u32 index = led/10;
        const u32 shift = (led%10)*3;
        const u32 mask = ~(0x7 << shift);
        gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift);

        //GPIO 18ピンの出力設定 
        const u32 led_2 = 18;
        const u32 index_2 = led_2/10;
        const u32 shift_2 = (led_2%10)*3;
        const u32 mask_2 = ~(0x7 << shift_2);
        gpio_base_2[index_2] = (gpio_base_2[index_2] & mask_2) | (0x1 << shift_2);

        return 0;
}

static void __exit cleanup_mod(void)
{
        cdev_del(&cdv);
        device_destroy(cls,dev);
        class_destroy(cls);
        cdev_del(&cdv);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "%s is unloaded major:%d\n", __FILE__,MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);
