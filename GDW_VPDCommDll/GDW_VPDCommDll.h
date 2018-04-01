#ifndef GDW_VPDCOMMDLL
#define GDW_VPDCOMMDLL

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 GDW_VPDCOMMDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// GDW_VPDCOMMDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef GDW_VPDCOMMDLL_EXPORTS
#define GDW_VPDCOMMDLL_API __declspec(dllexport)
#else
#define GDW_VPDCOMMDLL_API __declspec(dllimport)
#endif



/**********************************************
函数说明
初始化系统，连接处理单元。连接前保证配置文件中IP地址和端口号设置正确，
必须首先调用本函数返回0，并且收到网络连接成功消息后方可进行以后的操作。
参数			说明
HWND hWnd,	     [IN]接收消息的窗口的句柄，不允许为NULL
UINT uiDataMsgNo， [IN]当前数据准备好的消息号，必须大于0x400，否则默认为0x401。
UINT uiReConnectMsg，[IN]连接成功消息号，必须大于0x401，否则默认为0x402.
UINT uiDisconnectMsg，[IN]网络故障消息号，必须大于0x402，否则默认为0x403.
返回值			说明
0       		初始化成功；
1				初始化失败(原因：InitDll失败.)
2				初始化失败(原因：输入参数有误,hWnd = NULL).
3				初始化失败(原因：启动SDK失败).
4				初始化失败(原因：与VPD网络不通).
5				初始化失败(原因：打开通道1失败).
6				初始化失败(原因：打开通道2失败).
7				初始化失败(原因：设置回调函数失败).
8				初始化失败(原因：向VPD发送初始命令失败).
9               初始化失败(原因：启动心跳检测线程失败)
*****************************************************/
GDW_VPDCOMMDLL_API int WINAPI  GDW_Connect(HWND hWnd,
    UINT uiDataMsgNo = 0x401,
    UINT uiReConnectMsg = 0x402,
    UINT uiDisconnectMsg = 0x403);

/************************************************
函数说明
断开与处理单元的连接，关闭系统。必须与GDW_Connect成对出现,配合使用.
参数			说明
返回值			说明
0       		断开成功；
************************************************/
GDW_VPDCOMMDLL_API int WINAPI  GDW_Disconnect();


/************************************************
函数说明
向处理单元发送抓拍命令。
参数			说明
返回值			说明
0				抓拍成功
1				抓拍失败(原因：未连接)
2				抓拍失败(原因：向VPD发送命令失败)
3				抓拍失败(原因：向VPD发送命令失败)
4				抓拍失败(原因：网络故障)
5				抓拍失败(原因：等待数据超时)
************************************************/
GDW_VPDCOMMDLL_API int WINAPI  GDW_Capture();




/********************************************************
函数说明
收到当前数据准备好消息后，接收当前的车辆信息数据。
备注：
接收到当前数据准备好消息后调用，各参数所需内存在上端程序中申请。
车牌号至少需分配20个字节。
二值图至少分配280个字节。
车牌JPEG图至少分配5*1024个字节。
近景车辆JPEG图至少分配100*1024个字节。
远景车辆JPEG图至少分配100*1024个字节。
参数			说明
char* pchPlate，[OUT]车牌号码
char* pchTime，[OUT]车辆抓拍时间，格式为：yyyyMMddHHmmss
BYTE* pByteBinImage，[OUT]车牌二值图
BYTE* pBytePlateJpeg，[OUT]车牌JPEG图
UINT &dwPlateJpegSize，[OUT]车牌JPEG图的大小
BYTE* pByteCarJpeg，[OUT]近景车辆JPEG图
UINT &dwCarJpegSize，[OUT]近景车辆JPEG图的大小
BYTE* pByteFarJpeg，[OUT]远景车辆JPEG图
UINT &dwFarJpegSize，[OUT]远景车辆JPEG图的大小
返回值			说明
0       		获取数据成功.完整数据。
1				获取数据成功.只有车牌数据。
2				获取数据失败(原因：未连接)
3				获取数据失败(原因：参数输入有误,含有NULL指针).
4				获取数据失败(原因：网络数据未准备好).
5   			获取数据失败(原因：串口取数据失败).
6				获取数据失败(原因：串口数据未准备好).
7				获取数据失败(原因：网络和串口都不可用).
********************************************************************/
GDW_VPDCOMMDLL_API int WINAPI  GDW_GetVehicleInfo(char* pchPlate,
    char* pchTime,
    BYTE* pByteBinImage,
    BYTE* pBytePlateJpeg,
    UINT &dwPlateJpegSize,
    BYTE* pByteCarJpeg,
    UINT &dwCarJpegSize,
    BYTE* pByteFarJpeg,
    UINT &dwFarJpegSize);

/*******************************************************************
函数说明
收到当前数据准备好消息后，接收当前的车辆信息数据。
备注：
接收到当前数据准备好消息后调用，各参数所需内存在上端程序中申请。
车牌号至少需分配20个字节。
二值图至少分配280个字节。
车牌JPEG图至少分配5*1024个字节。
近景车辆JPEG图至少分配100*1024个字节。
参数			说明
char* pchPlate，[OUT]车牌号码
char* pchTime，[OUT]车辆抓拍时间，格式为：yyyyMMddHHmmss
BYTE* pByteBinImage，[OUT]车牌二值图
BYTE* pBytePlateJpeg，[OUT]车牌JPEG图
UINT &dwPlateJpegSize，[OUT]车牌JPEG图的大小
BYTE* pByteCarJpeg，[OUT]近景车辆JPEG图
UINT &dwCarJpegSize，[OUT]近景车辆JPEG图的大小
BYTE* pByteFarJpeg，[OUT]远景车辆JPEG图
UINT &dwFarJpegSize，[OUT]远景车辆JPEG图的大小
返回值			说明
0       		获取数据成功.完整数据。
1				获取数据成功.只有车牌数据。
2				获取数据失败(原因：未连接)
3				获取数据失败(原因：参数输入有误,含有NULL指针).
4				获取数据失败(原因：网络数据未准备好).
5   			获取数据失败(原因：串口取数据失败).
6				获取数据失败(原因：串口数据未准备好).
7				获取数据失败(原因：网络和串口都不可用).

********************************************************************/
GDW_VPDCOMMDLL_API int WINAPI  GDW_GetVehicleInfo2AD(char* pchPlate,
    char* pchTime,
    BYTE* pByteBinImage,
    BYTE* pBytePlateJpeg,
    UINT &dwPlateJpegSize,
    BYTE* pByteCarJpeg,
    UINT &dwCarJpegSize);

/*******************************************************************
函数说明
向处理单元发送校时命令。
参数			说明
char* pchTime，[IN]当前时间，格式为：yyyyMMddHHmmss，该参数不允许为空。
返回值			说明
0       		校时成功.
1				校时失败(原因：未连接).
2				校时失败(原因：参数为NULL).
3				校时失败(原因：year不在(**00-**99)之间).
4				校时失败(原因：month不在(01-12)之间).
5				校时失败(原因：day不在(01-31)之间).
6				校时失败(原因：hour不在(00-23)之间).
7				校时失败(原因：minite不在(00-59)之间).
8				校时失败(原因：second不在(00-59)之间).
9				校时失败(原因：发送校时命令失败).
10				校时失败(原因：网络不通,串口不支持该操作).
11				校时失败(原因：网络不通).
********************************************************************/
GDW_VPDCOMMDLL_API int WINAPI  GDW_AdjustTime(char* pchTime);

#endif