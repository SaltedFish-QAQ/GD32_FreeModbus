// *****************************************************************************************
// 头文件
#include <stdlib.h>
#include "mb_regtable.h"

// *****************************************************************************************
// 内部变量

//系统版本号
const uint16_t version   =  2;
static uint16_t fanState =  0;
static uint16_t setRotate =  0;

//变量映射表
static const sMB_HoldRegData_mb holdRegMapTable_mb[]=
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // 实时数据
    {(void*)&version,       SMR_U16_mb|SMR_NW_mb,     0, 65535},      // 0 ???
    {(void*)&fanState,      SMR_U16_mb|SMR_NW_mb,     0, 65535},      // 1 ??????
    {(void*)&setRotate,     SMR_U16_mb,                0, 65535},      // 2 ??????
};


#define usAddrMax  sizeof(holdRegMapTable_mb)/sizeof(holdRegMapTable_mb[0])
const uint8_t gUsAddrMax = usAddrMax;
// *****************************************************************************************
// 内部函数

// *****************************************************************************************
// 接口函数
//从机寄存器表读操作函数
//返回值：读取的数据
//参数：要读取的寄存器地址
uint16_t HoldRegList_mb_Read(uint16_t usAddress)
{
    const sMB_HoldRegData_mb * holdreg;
    uint8_t attribute = 0;
    uint16_t regdata;

    holdreg = &holdRegMapTable_mb[usAddress];
    
    //判断指针有效
    if (holdreg->rwptr)
    {
        // uint16_t 和s16类型
        if (SMR_U16_mb == attribute || SMR_S16_mb == attribute)
        {
            regdata = *(uint16_t*)(holdreg->rwptr);
        }
        // u8类型
        else if (SMR_U8_mb == attribute)
        {
            regdata = *(uint8_t*)(holdreg->rwptr); /*低字节*/
        }
        // s8类型
        else if (SMR_S8_mb == attribute)
        {
            regdata = 0xff00 | *(uint8_t*)(holdreg->rwptr); /*高字节使用0xff填充*/
        }
        // u32和s32类型
        else if (SMR_U32_mb == attribute || SMR_S32_mb == attribute)
        {
            regdata = *(uint16_t*)holdreg->rwptr;
        }
        else
        {
            regdata = 0;
        }
    }
    else if (SMR_U32_mb == attribute || SMR_S32_mb == attribute)
    {
        regdata = *((uint16_t*)holdreg[-1].rwptr + 1);
    }
    //指针无效0填充
    else 
    {
        regdata = 0;
    
    }
    //回调函数
    if (holdreg->pCallback)
        holdreg->pCallback(usAddress, sMB_RegEventRead, holdreg->pdata);

    return regdata;;
}
//从机寄存器表写操作函数
//返回值：写操作成功与否标志
//参数：寄存器地址，要写入的数据
eMBErrorCode HoldRegList_mb_Write(uint16_t usAddress,uint16_t dataValue)
{
    eMBErrorCode eMBCode;
    const sMB_HoldRegData_mb * holdreg;
    uint8_t attribute = 0;
    int32_t datatemp;
    
	//数据地址
	holdreg = &holdRegMapTable_mb[usAddress];
	attribute = holdreg->attribute & 0xf0;

	if (SMR_U32_mb == attribute || SMR_S32_mb == attribute)
	{
		if (holdreg->rwptr != NULL)
			*(uint16_t*)holdreg->rwptr = dataValue;
		else
			*((uint16_t*)holdreg[-1].rwptr + 1) = dataValue;
	}
	else if (holdreg->rwptr)
	{
	    //寄存器不可写 返回操作错误
		if (attribute & SMR_NW_mb)
		{
    		//标准错误回复
#if SMB_01_REG_WRITE_ERROR_REP              >0
	    	return MB_EX_ILLEGAL_DATA_VALUE;
#else
			return MB_ENOERR; /* 无错误 */
#endif
		}

		datatemp = (int32_t)(dataValue);

		//判断最大值最小值
		if (datatemp < holdreg->min || datatemp> holdreg->max)
	    {
			//标准错误回复
#if SMB_01_REG_WRITE_ERROR_REP              >0
			return MB_ENORES; /* 非法的数据值 */
#else
			return MB_ENOERR; /* 无错误 */
#endif
		}

		//uint8_t int8_t 类型 
		if (SMR_U8_mb == attribute || SMR_S8_mb == attribute)
			*(uint8_t*)(holdreg->rwptr) = (uint8_t)dataValue;          /* 按字节写入 */
		//uint16_t int16_t 类型
		else 
			*(uint16_t*)(holdreg->rwptr) = (uint16_t)dataValue;        /* 按双字节写入 */
	}

	// 回调函数
	if (holdreg->pCallback)
		holdreg->pCallback(usAddress, sMB_RegEventWrite, holdreg->pdata);

    eMBCode = MB_ENOERR;
    return eMBCode;
}


