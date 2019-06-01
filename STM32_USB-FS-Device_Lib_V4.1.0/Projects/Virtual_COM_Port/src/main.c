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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint8_t reset_state = 0;
extern uint8_t USB_Rx_Buffer[];
/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#define BOOT  LED2// Wifi Boot Pin
#define RESET LED3// Wifi Reset Pin
int main(void)
{
    STM_EVAL_LEDInit(BOOT);
    STM_EVAL_LEDOn(BOOT);
    STM_EVAL_LEDInit(RESET);
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    STM_EVAL_LEDInit(LED1);

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
            for(volatile uint32_t i = 0; i < 500000; i++);
            STM_EVAL_LEDOff(RESET);
        }
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
