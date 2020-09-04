#ifndef _MB_REGTABLE_H_
#define _MB_REGTABLE_H_

// *****************************************************************************************
// 头文件
#include "mb.h"
// *****************************************************************************************
// 内部变量

//寄存器读写属性
#define SMR_NW_mb  (1<<0)       /* 不可写 */

//寄存器变量属性
#define SMR_U16_mb (0x00<<4)     /* 16位无符号类型 */
#define SMR_S16_mb (0x01<<4)     /* 16位有符号类型 */
#define SMR_U8_mb  (0x02<<4)     /* 8位无符号类型 */
#define SMR_S8_mb  (0x03<<4)     /* 8位有符号类型 */
#define SMR_U32_mb (0x04<<4)     /* 32位无符号类型 */
#define SMR_S32_mb (0x05<<4)     /* 32位有符号类型 */

// 寄存器事件枚举
typedef enum
{
    sMB_RegEventRead,   // 读取事件
    sMB_RegEventWrite,  // 写入事件
}sMB_RegEventEnum_mb;

//寄存器事件回调函数类型
typedef void(*RegEventCallback_mb)(uint16_t regaddr,sMB_RegEventEnum_mb rweevent,void* pdata);


//寄存器映射表
typedef struct
{
    void*    rwptr;         /* 读写变量地址 */
    uint8_t  attribute;     /* 数据属性 */
    int32_t min;            /* 最小值 */
    int32_t max;            /* 最大值 */

    RegEventCallback_mb pCallback;  /* 事件回调函数 */  
    void* pdata;                 /* 事件回调函数的参数 */
}sMB_HoldRegData_mb;

// *****************************************************************************************
// 接口函数

uint16_t HoldRegList_mb_Read(uint16_t usAddress);
eMBErrorCode HoldRegList_mb_Write(uint16_t usAddress,uint16_t dataValue);


#endif

