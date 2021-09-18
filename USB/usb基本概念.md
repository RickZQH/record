

usb1.0 usb1.1  

​	低速  low-speed 1.5Mb/s

​    全速 full-speed   12Mb/s

usb2.0    

​	高速  high-speed 480Mb/s



层级关系：

​	1.设备描述符

​		协议版本号、设备类型、端点0的最大包大小、厂商ID(VID)和产品ID(PID)、设备版本号等等

​	2.配置描述符

​		接口数、配置的编号、供电方式等

​	3.接口描述符

​		接口的编号、接口的端点数、接口所使用的类、子类、协议等

​	4.端点描述符

​		端点号及方端点的传输类型、最大包长度、查询时间间隔

​	5.字符串描述符

​		方便人们阅读的信息非必须



传输模式：

​	控制传输：1.建立过程 ,由usb主机发起始于SETUP令牌包，紧跟DATA0数据包

​						2.数据过程(可选) 

​						3.状态过程，用于确认所有的数据是否正确传输完成



枚举过程：

​	1.检测到usb插入时对设备进行复位，获取设备描述符(仅需前8字节)

​	2.主机对设备进行又一次复位，进入地址设置阶段

​	3.使用新地址再次获取设备描述符（18字节）

​	4.主机获取配置描述符(9字节)

​				a.配置描述符

​				b.接口描述符

​				c.特殊类描述符

​				d.端点描述符

   5.字符描述符



USb的包结构和包的分类

物理

​	传输方式：LSB在前

​	NRZI编码

在usb系统中，主机处于主导地位，设备到主机的数据为输入，主机到设备的数据为输出



USB总线传输数据以包为基本单位，一个包被分成不同的域，根据不同类型的包，所包含的域也不一样。



同步域---包标识符PID---......---包结束符EOP



包标识符---8位，USB协议使用到的只有4位，另外4位用于校验PID

令牌包(01)、数据包(11)、握手包()10、特殊包(00)





| PID类型 | PID名  | PID  | 说明                 |
| ------- | ------ | ---- | -------------------- |
| 令牌类  | OUT    |      | 通知设备将于输出数据 |
|         | IN     |      | 通知设备将于输入数据 |
|         | SOF    |      | 帧起始包             |
|         | SETUP  |      | 将要开始一个控制传输 |
| 数据类  | DATA0  |      |                      |
|         | DATA1  |      |                      |
|         | DATA2* |      |                      |
|         | MDATA* |      |                      |
| 握手类  | ACK    |      | 确认                 |
|         | NAK    |      | 不确认               |
|         | STALL  |      | 挂起                 |
|         | NYET*  |      | 未准备好             |
| 特殊类  | PRE    |      | 前导(令牌包)         |
|         | ERR*   |      | 错误(握手包)         |
|         | SPLIT* |      | 分裂事物(令牌包)     |
|         | PING*  |      | PING测试(令牌包)     |



### uac相关



UAC音频控制接口和UAC音频流接口



```C

struct uac1_control
{
#ifdef RT_USB_DEVICE_COMPOSITE
    struct uiad_descriptor iad_desc;
#endif
    struct uinterface_descriptor interface_desc;
    struct uac1_ac_header_descriptor_2 header_desc;
    struct uac_input_terminal_descriptor usb_out_it_desc;
    struct uac1_output_terminal_descriptor io_out_ot_desc;
    struct uac_feature_unit_descriptor_0 usb_out_it_feature_desc;
    struct uac_input_terminal_descriptor io_in_it_desc;
    struct uac1_output_terminal_descriptor usb_in_ot_desc;
    struct uac_feature_unit_descriptor_0 io_in_it_feature_desc;
};
typedef struct uac1_control *uac1_control_t;

struct uac1_interface_alt
{
    struct uinterface_descriptor interface_desc;
    struct uac1_as_header_descriptor as_header_desc;
    struct uac_format_type_i_discrete_descriptor_6 type_i_desc;
    struct uaudio_endpoint_descriptor endpoint_desc;
    struct uac_iso_endpoint_descriptor iso_endpoint_desc;
};



//创建时申请的内存
{
    uac1_control
    uinterface_descriptor
    uac1_interface_alt
    uinterface_descriptor
    uac1_interface_alt
}

//RT-thread uac描述符
struct uac1_control
{
#ifdef RT_USB_DEVICE_COMPOSITE
    struct uiad_descriptor iad_desc;								//协议接口描述符
#endif
    struct uinterface_descriptor interface_desc;					//接口关联描述符
    struct uac1_ac_header_descriptor_2 header_desc;					//特定音频控制接口头描述符
    struct uac_input_terminal_descriptor usb_out_it_desc;			//输入终端描述符
    struct uac1_output_terminal_descriptor io_out_ot_desc;			//输出终端描述符
    struct uac_feature_unit_descriptor_0 usb_out_it_feature_desc;	//特性单元描述符
    struct uac_input_terminal_descriptor io_in_it_desc;				//输入终端描述符	
    struct uac1_output_terminal_descriptor usb_in_ot_desc;			//输出终端描述符
    struct uac_feature_unit_descriptor_0 io_in_it_feature_desc;		//特性单元描述符
};

//其他描述符
    
    


```



```C
//DCX81 cdc描述符
{
    //协议接口描述符
    //标准接口描述符
    //端点描述符1 in
    //标准接口描述符1
    //端点描述符2 out
    //端点描述符2 in   
}

//uac描述符
{
    //1.接口协议描述符
    //2.标准音频控制接口描述符 接口1
    //3.特殊音频控制接口描述符
    
    //4.耳机usb输入描述符 ID1
    //5.耳机麦克风输入端描述符 ID4    
    //6.耳机功能单元描述符 ID2
    //7.耳机功能单元描述符 ID5
    //8.耳机功能单元描述符 ID7
    //9.耳机混合器单元描述符 ID8
    //10.耳机扬声器输出终端描述符 ID3
    //11.耳机扬声器输出终端描述符 ID6
    
    //12.耳机扬声器输出音频流接口描述符-接口1
    //13.耳机扬声器标准AS接口描述符-接口1备用1
    //14.耳机扬声器类特定的AS通用接口描述符
    //15.耳机扬声器格式类型描述符
    //16.耳机扬声器标准AS音频数据端点
    //17.耳机扬声器类特定的同步音频数据端点描述符
    
    //18.耳机麦克风在音频流接口描述符-接口2
    //19.耳机麦克风标准AS接口描述符-接口2备选1
    //20.耳机麦克风类特定的AS通用接口描述符
    //21.耳机麦克风格式类型描述符
    //22.耳机麦克风标准AS音频数据端点
    //23.耳机麦克风类特定的等时音频数据端点描述符
}


```



问题判断：----重点检查和音频相关传输的描述符









