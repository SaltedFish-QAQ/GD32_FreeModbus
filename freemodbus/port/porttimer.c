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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "gd32f3x0.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    // return FALSE;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER13);

    timer_deinit(TIMER13);

    timer_struct_para_init(&timer_initpara);

    timer_initpara.prescaler         = 4199;
    // timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = usTim1Timerout50us - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER13, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER13, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER13);

    //��ʱ��2�жϣ�����֡���ճ�ʱ
    nvic_irq_enable(TIMER13_IRQn,0,2);
	
	return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    timer_counter_value_config(TIMER13,0);//���㶨ʱ��
    timer_interrupt_flag_clear(TIMER13,TIMER_INT_UP);//�����ʱ�������ж�
    timer_interrupt_enable(TIMER13,TIMER_INT_UP);//ʹ�ܶ�ʱ�������ж�
    timer_enable(TIMER13);//ʹ�ܶ�ʱ��
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
    timer_counter_value_config(TIMER13,0);//���㶨ʱ��
    timer_interrupt_flag_clear(TIMER13,TIMER_INT_UP);//�����ʱ�������ж�
    timer_interrupt_disable(TIMER13,TIMER_INT_UP);//���ö�ʱ�������ж�
    timer_disable(TIMER13);//ʧ�ܶ�ʱ��
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}
//��ʱ������ж�
/*******************************************************************************************
����:     void TIM6_IRQHandler(void)
˵��:     ��ʱ��ʱ������ж� ��Ҫ���ж��е��� sMB_TimerExpired() ֪ͨmodbus
����:

����ֵ:  void
********************************************************************************************/
void TIMER13_IRQHandler(void)
{
    if(RESET != timer_interrupt_flag_get(TIMER13, TIMER_INT_UP))
    {
        /* clear timer update interrupt bit */
        timer_interrupt_flag_clear(TIMER13, TIMER_INT_UP);
        /* clear count number*/
        timer_counter_value_config(TIMER13,0);
        /* use this function to call pxMBPortCBTimerExpired( ) */
        prvvTIMERExpiredISR();
    }
}
