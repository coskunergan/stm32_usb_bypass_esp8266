/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Virtual Com Port Demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BOOT  LED2// Wifi Boot Pin
#define RESET LED3// Wifi Reset Pin
#define BUZZER LED4// Wifi Reset Pin
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern uint8_t  USART_Rx_Buffer[];
extern uint32_t USART_Rx_ptr_in;
uint8_t reset_state = 0, temp;
extern uint8_t USB_Rx_Buffer[];
typedef  void (*pFunction)(void);
pFunction Jump_To_DFU;
uint32_t JumpAddress;
/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#define Boot_Key_Adr 0x2000BFF8
#define Boot_DFU_Adr 0x1FF00000
#define Boot_Key		 0xDEADBEEF
void Start_DFU(void)
{
    SYSCFG_USBPuCmd(DISABLE);
    SYSCFG_DeInit();
    for(uint8_t i = 0; i < 15; i++)// blink led
    {
        for(volatile uint32_t i = 0; i < 100000; i++);
        STM_EVAL_LEDToggle(LED1);
        STM_EVAL_LEDToggle(BUZZER);

    }
    *((unsigned long *)Boot_Key_Adr) = Boot_Key; // 48KB STM32L152D
    NVIC_SystemReset();
}
/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

int main(void)
{
    if(*(uint32_t *)Boot_Key_Adr == Boot_Key)
    {
        *(uint32_t *)Boot_Key_Adr = 0;
        JumpAddress = *(__IO uint32_t *)(Boot_DFU_Adr + 4);
        Jump_To_DFU = (pFunction) JumpAddress;
        __set_MSP(*(__IO uint32_t *) Boot_DFU_Adr);
        Jump_To_DFU();
    }
    STM_EVAL_LEDInit(BOOT);
    STM_EVAL_LEDOn(BOOT);
    STM_EVAL_LEDInit(RESET);
    STM_EVAL_LEDInit(BUZZER);
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    STM_EVAL_LEDInit(LED1);
    STM_EVAL_LEDOn(BUZZER);
    for(volatile uint32_t i = 0; i < 100000; i++);
    STM_EVAL_LEDOff(BUZZER);

    while(1)
    {
        ///////////////////////////////////////////////
        for(volatile uint32_t i = 0; i < 500000; i++);
        ///////////////////////////////////////////////
        if(reset_state == 0)
        {
            STM_EVAL_LEDToggle(LED1);
        }
        else
        {
            STM_EVAL_LEDOn(LED1);
        }
        if((USB_Rx_Buffer[0] == 0xC0) &&
                (USB_Rx_Buffer[1] == 0x40) &&
                (USB_Rx_Buffer[2] == 0x12) &&
                (USB_Rx_Buffer[3] == 0x20) &&
                (USB_Rx_Buffer[4] == 0x55) &&
                (USB_Rx_Buffer[5] == 0x55) &&
                (reset_state == 0))
        {
            reset_state = 1;
            STM_EVAL_LEDOn(RESET);
            STM_EVAL_LEDOn(BUZZER);
            for(volatile uint32_t i = 0; i < 500000; i++);
            STM_EVAL_LEDOff(BUZZER);
            STM_EVAL_LEDOff(RESET);
        }

        /////////////////////////////////////////////////
        if(USB_Rx_Buffer[0] == 0x68 &&
                USB_Rx_Buffer[1] == 0xB  &&
                USB_Rx_Buffer[2] == 0xB  &&
                USB_Rx_Buffer[3] == 0x68  &&
                USB_Rx_Buffer[4] == 0x53  &&
                USB_Rx_Buffer[5] == 0xFD  &&
                USB_Rx_Buffer[6] == 0x52  &&
                USB_Rx_Buffer[7] == 0xFF  &&
                USB_Rx_Buffer[8] == 0xFF  &&
                USB_Rx_Buffer[9] == 0xFF  &&
                USB_Rx_Buffer[10] == 0xFF  &&
                USB_Rx_Buffer[11] == 0xFF  &&
                USB_Rx_Buffer[12] == 0xFF  &&
                USB_Rx_Buffer[13] == 0xFF  &&
                USB_Rx_Buffer[14] == 0x31  &&
                USB_Rx_Buffer[15] == 0xCC)
        {
            Start_DFU();
        }
        /////////////////////////////////////////////////
    }
}
#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while(1)
    {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
