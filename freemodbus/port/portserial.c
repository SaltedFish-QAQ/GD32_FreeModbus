/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"
#include "gd32f3x0.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    if(TRUE == xRxEnable)
    {
        usart_interrupt_enable(USART1,USART_INT_RBNE);//ʹ�ܽ����ж�
    }
    else
    {
        usart_interrupt_disable(USART1,USART_INT_RBNE);//ʧ�ܽ����ж�
    }
    
    if(TRUE == xTxEnable)
    {
        usart_interrupt_enable(USART1,USART_INT_TBE);//ʹ�ܷ����ж�
    }
    else
    {
        usart_interrupt_disable(USART1,USART_INT_TBE);//ʧ�ܷ����ж�
    }
    
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
//    return FALSE;
    (void)ucPORT;
    (void)ucDataBits;
	(void)eParity;

    //ʹ��ʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART1);

    //���Ӷ˿ڵ�����tx��rx
    gpio_af_set(GPIOA,GPIO_AF_1,GPIO_PIN_2);//connect port to USARTx_TX
    gpio_af_set(GPIOA,GPIO_AF_1,GPIO_PIN_3);//connect port to USARTX_RX

    //����gpio�ܽ�
    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    //���ڳ�ʼ������
    usart_deinit(USART1);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_baudrate_set(USART1, ulBaudRate);

    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);

    //�����ж�
    nvic_irq_enable(USART1_IRQn, 0, 0);

    usart_enable(USART1); 
	
	return TRUE;
}
BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    usart_data_transmit(USART1,ucByte);
	return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = usart_data_receive(USART1);
	return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}
//USART2�ж�
/*******************************************************************************************
����:     void USART2_IRQHandler(void)
˵��:     �����ж� �ڷ����ж��е��� sMB_TxSerialData() �����ж��е��� sMB_RxSerialData()
����:

����ֵ:  void
********************************************************************************************/
void USART1_IRQHandler(void)
{
    //�жϽ��ջ��Ƿ����ж�
    if (RESET != usart_interrupt_flag_get(USART1,USART_INT_FLAG_RBNE))
    {
        //��������ж�
        prvvUARTRxISR();
        usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE_ORERR);//orerrλ��rbneλһͬ��һ��rbneλ�Զ�����
    }
    if(RESET != usart_interrupt_flag_get(USART1,USART_INT_FLAG_TBE))
    {
        //��������ж�
        prvvUARTTxReadyISR();
        usart_interrupt_flag_clear(USART1,USART_INT_FLAG_TC);//tbeλ�Զ�����
    }
}
