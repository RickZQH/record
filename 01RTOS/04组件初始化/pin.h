/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-20     Bernard      the first version
 * 2017-10-20      ZYH          add mode open drain and input pull down
 */

#ifndef PIN_H__
#define PIN_H__

#include <rtthread.h>
#include <rtdevice.h>

#ifdef __cplusplus
extern "C" {
#endif

/* pin device and operations for RT-Thread */
struct rt_device_pin
{
    struct rt_device parent;
    const struct rt_pin_ops *ops;
};

//GPIO高低电平宏
#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

//GPIO模式宏
#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

//GPIO中断触发模式宏
#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

//GPIO中断状态宏
#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01


#define PIN_IRQ_PIN_NONE                -1

//GPIO模式结构体
struct rt_device_pin_mode
{
    rt_uint16_t pin;
    rt_uint16_t mode;
};

//GPIO状态结构体
struct rt_device_pin_status
{
    rt_uint16_t pin;
    rt_uint16_t status;
};

//GPIO中断结构体
struct rt_pin_irq_hdr
{
    rt_int16_t        pin;		//GPIO引脚
    rt_uint16_t       mode;		//引脚模式
    void (*hdr)(void *args);	//中断回调函数
    void             *args;		//中断参数
};

//GPIO引脚操作结构体
struct rt_pin_ops
{
    void (*pin_mode)(struct rt_device *device, rt_base_t pin, rt_base_t mode);		
    void (*pin_write)(struct rt_device *device, rt_base_t pin, rt_base_t value);
    int (*pin_read)(struct rt_device *device, rt_base_t pin);

    /* TODO: add GPIO interrupt */
    rt_err_t (*pin_attach_irq)(struct rt_device *device, rt_int32_t pin,
                      rt_uint32_t mode, void (*hdr)(void *args), void *args);
    rt_err_t (*pin_detach_irq)(struct rt_device *device, rt_int32_t pin);
    rt_err_t (*pin_irq_enable)(struct rt_device *device, rt_base_t pin, rt_uint32_t enabled);
};

//设备注册
int rt_device_pin_register(const char *name, const struct rt_pin_ops *ops, void *user_data);

//设置引脚模式
void rt_pin_mode(rt_base_t pin, rt_base_t mode);

//GPIO引脚状态设置
void rt_pin_write(rt_base_t pin, rt_base_t value);

//获取GPIO引脚状态
int  rt_pin_read(rt_base_t pin);

//设置GPIO引脚中断回调函数
rt_err_t rt_pin_attach_irq(rt_int32_t pin, rt_uint32_t mode,
                             void (*hdr)(void *args), void  *args);

//删除GPIO引脚中断回调函数
rt_err_t rt_pin_detach_irq(rt_int32_t pin);

//GPIO中断控制函数
rt_err_t rt_pin_irq_enable(rt_base_t pin, rt_uint32_t enabled);

#ifdef __cplusplus
}
#endif

#endif
