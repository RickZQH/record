

usb-hid基础
==============================
 
 
USB设备描述符---向主机传递信息，用于识别设备：功能、哪一类、占用带宽、传输方式、数据量大小
 
 
共有5中描述符：(具有一定的包含关系)

	设备描述符		1
	配置描述符		2
	字符描述符
	接口描述符		3
	端点描述符		4
 
设备描述符
struct _DEVICE_DEscriptOR_STRUCT
{
  BYTE   bLength;    		//设备描述符的字节数大小
  BYTE   bDescriptorType;   //描述符类型编号，为0x01
  WORD  bcdUSB;   			//USB版本号
  BYTE  bDeviceClass;   	//USB分配的设备类代码，0x01~0xfe为标准设备类，0xff为厂商自定义类型，0x00不是在设备描述符中定义的，如HID
  BYTE   bDeviceSubClass;   //usb分配的子类代码，同上，值由USB规定和分配的，HID设备此值为0
  BYTE  bDeviceProtocl;  	//USB分配的设备协议代码，同上HID设备此值为0
  BYTE   bMaxPacketSize0;   //端点0的最大包的大小
  WORD   idVendor;  		//厂商编号
  WORD   idProduct;  		//产品编号
  WORD  bcdDevice;  		//设备出厂编号
  BYTE   iManufacturer;   	//描述厂商字符串的索引
  BYTE   iProduct;   		//描述产品字符串的索引
  BYTE   iSerialNumber;  	//描述设备序列号字符串的索引
  BYTE   bNumConfiguration; //可能的配置数量
}

配置描述符
struct _CONFIGURATION_DEscriptOR_STRUCT
{
  BYTE  bLength;   				//配置描述符的字节数大小
  BYTE   bDescriptorType;   	//描述符类型编号，为0x02
  WORD   wTotalLength;   		//配置所返回的所有数量的大小
  BYTE   bNumInterface;  		//此配置所支持的接口数量
  BYTE   bConfigurationVale;    //Set_Configuration命令需要的参数值
  BYTE   iConfiguration;  		//描述该配置的字符串的索引值
  BYTE  bmAttribute;  			//供电模式的选择
  BYTE   MaxPower;   			//设备从总线提取的最大电流
}

字符描述符 
struct _STRING_DEscriptOR_STRUCT
{
    BYTE bLength; 				//字符串描述符的字节数大小
    BYTE bDescriptorType; 		//描述符类型编号，为0x03
    BYTE SomeDescriptor[36]; 	//UNICODE编码的字符串
} 
 
接口描述符
struct _INTERFACE_DEscriptOR_STRUCT
{
    BYTE bLength; 				//接口描述符的字节数大小
    BYTE bDescriptorType; 		//描述符类型编号，为0x04    
    BYTE bInterfaceNunber; 		//接口的编号    
    BYTE bAlternateSetting;		//备用的接口描述符编号    
    BYTE bNumEndpoints; 		//该接口使用端点数，不包括端点0    
    BYTE bInterfaceClass; 		//接口类型 HID设备此值为0x03    
    BYTE bInterfaceSubClass;	//接口子类型 HID设备此值为0或者1  (1为boot device)  
    BYTE bInterfaceProtocol;	//接口所遵循的协议    
    BYTE iInterface; 			//描述该接口的字符串索引值
}
 
端点描述符
struct _ENDPOIN_DEscriptOR_STRUCT
{
    BYTE bLength; 			//端点描述符的字节数大小
    BYTE bDescriptorType;   //描述符类型编号，为0x05
    BYTE bEndpointAddress; 	//端点地址及输入输出属性
    BYTE bmAttribute; 		//端点的传输类型属性
    WORD wMaxPacketSize; 	//端点收、发的最大包的大小
    BYTE bInterval; 		//主机查询端点的时间间隔
} 


---------------------USB-HID-------------------------------

设备描述符
配置描述符
HID描述符
报告描述符
物理描述符(非必须)



报告描述符：

较为复杂，以item形式排列组合而成，无固定长度，用户可以自定义长度及每一bit的含义。
分为main、global、local，其中main类型分为5种tag


所有的HID设备通过USB的控制管道（默认管道，即端点0）和中断管道与主机通信。
























