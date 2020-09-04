#ifndef _MB_REGTABLE_H_
#define _MB_REGTABLE_H_

// *****************************************************************************************
// ͷ�ļ�
#include "mb.h"
// *****************************************************************************************
// �ڲ�����

//�Ĵ�����д����
#define SMR_NW_mb  (1<<0)       /* ����д */

//�Ĵ�����������
#define SMR_U16_mb (0x00<<4)     /* 16λ�޷������� */
#define SMR_S16_mb (0x01<<4)     /* 16λ�з������� */
#define SMR_U8_mb  (0x02<<4)     /* 8λ�޷������� */
#define SMR_S8_mb  (0x03<<4)     /* 8λ�з������� */
#define SMR_U32_mb (0x04<<4)     /* 32λ�޷������� */
#define SMR_S32_mb (0x05<<4)     /* 32λ�з������� */

// �Ĵ����¼�ö��
typedef enum
{
    sMB_RegEventRead,   // ��ȡ�¼�
    sMB_RegEventWrite,  // д���¼�
}sMB_RegEventEnum_mb;

//�Ĵ����¼��ص���������
typedef void(*RegEventCallback_mb)(uint16_t regaddr,sMB_RegEventEnum_mb rweevent,void* pdata);


//�Ĵ���ӳ���
typedef struct
{
    void*    rwptr;         /* ��д������ַ */
    uint8_t  attribute;     /* �������� */
    int32_t min;            /* ��Сֵ */
    int32_t max;            /* ���ֵ */

    RegEventCallback_mb pCallback;  /* �¼��ص����� */  
    void* pdata;                 /* �¼��ص������Ĳ��� */
}sMB_HoldRegData_mb;

// *****************************************************************************************
// �ӿں���

uint16_t HoldRegList_mb_Read(uint16_t usAddress);
eMBErrorCode HoldRegList_mb_Write(uint16_t usAddress,uint16_t dataValue);


#endif

