// *****************************************************************************************
// ͷ�ļ�
#include <stdlib.h>
#include "mb_regtable.h"

// *****************************************************************************************
// �ڲ�����

//ϵͳ�汾��
const uint16_t version   =  2;
static uint16_t fanState =  0;
static uint16_t setRotate =  0;

//����ӳ���
static const sMB_HoldRegData_mb holdRegMapTable_mb[]=
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // ʵʱ����
    {(void*)&version,       SMR_U16_mb|SMR_NW_mb,     0, 65535},      // 0 ???
    {(void*)&fanState,      SMR_U16_mb|SMR_NW_mb,     0, 65535},      // 1 ??????
    {(void*)&setRotate,     SMR_U16_mb,                0, 65535},      // 2 ??????
};


#define usAddrMax  sizeof(holdRegMapTable_mb)/sizeof(holdRegMapTable_mb[0])
const uint8_t gUsAddrMax = usAddrMax;
// *****************************************************************************************
// �ڲ�����

// *****************************************************************************************
// �ӿں���
//�ӻ��Ĵ��������������
//����ֵ����ȡ������
//������Ҫ��ȡ�ļĴ�����ַ
uint16_t HoldRegList_mb_Read(uint16_t usAddress)
{
    const sMB_HoldRegData_mb * holdreg;
    uint8_t attribute = 0;
    uint16_t regdata;

    holdreg = &holdRegMapTable_mb[usAddress];
    
    //�ж�ָ����Ч
    if (holdreg->rwptr)
    {
        // uint16_t ��s16����
        if (SMR_U16_mb == attribute || SMR_S16_mb == attribute)
        {
            regdata = *(uint16_t*)(holdreg->rwptr);
        }
        // u8����
        else if (SMR_U8_mb == attribute)
        {
            regdata = *(uint8_t*)(holdreg->rwptr); /*���ֽ�*/
        }
        // s8����
        else if (SMR_S8_mb == attribute)
        {
            regdata = 0xff00 | *(uint8_t*)(holdreg->rwptr); /*���ֽ�ʹ��0xff���*/
        }
        // u32��s32����
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
    //ָ����Ч0���
    else 
    {
        regdata = 0;
    
    }
    //�ص�����
    if (holdreg->pCallback)
        holdreg->pCallback(usAddress, sMB_RegEventRead, holdreg->pdata);

    return regdata;;
}
//�ӻ��Ĵ�����д��������
//����ֵ��д�����ɹ�����־
//�������Ĵ�����ַ��Ҫд�������
eMBErrorCode HoldRegList_mb_Write(uint16_t usAddress,uint16_t dataValue)
{
    eMBErrorCode eMBCode;
    const sMB_HoldRegData_mb * holdreg;
    uint8_t attribute = 0;
    int32_t datatemp;
    
	//���ݵ�ַ
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
	    //�Ĵ�������д ���ز�������
		if (attribute & SMR_NW_mb)
		{
    		//��׼����ظ�
#if SMB_01_REG_WRITE_ERROR_REP              >0
	    	return MB_EX_ILLEGAL_DATA_VALUE;
#else
			return MB_ENOERR; /* �޴��� */
#endif
		}

		datatemp = (int32_t)(dataValue);

		//�ж����ֵ��Сֵ
		if (datatemp < holdreg->min || datatemp> holdreg->max)
	    {
			//��׼����ظ�
#if SMB_01_REG_WRITE_ERROR_REP              >0
			return MB_ENORES; /* �Ƿ�������ֵ */
#else
			return MB_ENOERR; /* �޴��� */
#endif
		}

		//uint8_t int8_t ���� 
		if (SMR_U8_mb == attribute || SMR_S8_mb == attribute)
			*(uint8_t*)(holdreg->rwptr) = (uint8_t)dataValue;          /* ���ֽ�д�� */
		//uint16_t int16_t ����
		else 
			*(uint16_t*)(holdreg->rwptr) = (uint16_t)dataValue;        /* ��˫�ֽ�д�� */
	}

	// �ص�����
	if (holdreg->pCallback)
		holdreg->pCallback(usAddress, sMB_RegEventWrite, holdreg->pdata);

    eMBCode = MB_ENOERR;
    return eMBCode;
}


