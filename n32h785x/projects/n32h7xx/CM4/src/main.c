/**
*     Copyright (c) 2025, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/
 
/**
 *\*\file main.c
 *\*\author Nations
 *\*\version v1.0.0
 *\*\copyright Copyright (c) 2025, Nations Technologies Inc. All rights reserved.
 **/

#include "main.h"
#include "delay.h"
#include "log.h"

void GPIO_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
static void MPU_Config(void);
void SRAM_RecvM7Data(void );

#define   SHARING_MEMORY_ADDRESS    (0x24015000UL)
#define   SHARING_MEMORY_SIZE       (MPU_REGION_SIZE_2KB)
/*M4 Receive Msgs*/
#define   SRAM_READ_ADDR           (0x24015000UL)
#define   SRAM_READ_BUFFER_SIZE    (0X100)

uint8_t M4ReceiveFinishFlag = 0 ;
uint8_t last_state = 1;
/**
 *\*\name   main.
 *\*\fun    Main program.
 *\*\param  none
 *\*\return none
 */
int main(void)
{
    /* Add Cortex-M4 user application code here */
    /* RCC configuration -------------------------------------------------------*/
    RCC_Configuration();
    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration();
    /* NVIC_Configuration ------------------------------------------------------*/
    NVIC_Configuration();
    /* MPU_Configuration ------------------------------------------------------*/
    MPU_Config();
    /* Int Config */
    DCMU_ConfigInt(DCMU_CTRL_RFIE0_MASK , ENABLE);

    
    
    while (1)
    {
        if(last_state != M4ReceiveFinishFlag)
        {
            last_state = M4ReceiveFinishFlag;
            if(last_state) GPIO_SetBits(LED2_PORT,LED2_PIN);
            else GPIO_ResetBits(LED2_PORT,LED2_PIN);
        }
        // if (DCMU_GetIntPendingFlags(DCMU_STS_RFF0_MASK) == DCMU_STS_RFF0_MASK)
        // {
        //     DCMU_ReceiveMsg((uint8_t)RCVMSG_IDX0, NON_BLOCKING);
        //     last_state = !last_state;
        //     if (last_state)
        //         GPIO_SetBits(LED2_PORT, LED2_PIN);
        //     else
        //         GPIO_ResetBits(LED2_PORT, LED2_PIN);
        // }
        systick_delay_ms(10);
    }
}

/**
*\*\name    GPIO_Configuration.
*\*\fun     Configures the different GPIO ports.
*\*\return  none
**/
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin     = LED2_PIN;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull    = GPIO_NO_PULL;
    GPIO_InitPeripheral( LED2_PORT, &GPIO_InitStructure );

}
/**
*\*\name    RCC_Configuration.
*\*\fun     Configures the different system clocks.
*\*\return  none
**/
void RCC_Configuration(void)
{
    /* Enable peripheral clocks ------------------------------------------------*/
    /* Enable LED2 clocks */
    RCC_EnableAHB5PeriphClk1(LED2_CLOCK, ENABLE);

    /* Enable RCC DCMU CLK */
    RCC_EnableCFG4PeriphClk1(RCC_CFG4_PERIPHEN_M4DCMUCLK, ENABLE);
}


/**
*\*\name    NVIC_Configuration.
*\*\fun     Configures Vector Table base location.
*\*\return  none
**/
void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /* Configure and enable DCMU interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = DCMUA_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void DCMUA_IRQHandler(void)
{
    if(DCMU_GetIntPendingFlags(DCMU_STS_RFF0_MASK) == DCMU_STS_RFF0_MASK)
    {
        DCMU_ClearIntPendingFlags(DCMU_STS_RFF0_MASK);
        DCMU_ReceiveMsg((uint8_t)RCVMSG_IDX0, NON_BLOCKING);
        SRAM_RecvM7Data();
    }
}

static void MPU_Config(void)
{
    MPU_Region_InitType MPU_InitStruct;

    /* Disable the MPU */
    MPU_Disable();

    /* Configure the MPU as Strongly ordered for not defined regions */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = SHARING_MEMORY_ADDRESS;
    MPU_InitStruct.Size = SHARING_MEMORY_SIZE;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

    MPU_ConfigRegion(&MPU_InitStruct);
    /* Enable the MPU */
    MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


void SRAM_RecvM7Data(void)
{
    uint32_t  index,i;
    uint32_t  data;
    data = *(__IO uint32_t*) (SRAM_READ_ADDR);
    /* Write data to the SDRAM memory */
    if (data == 0x88)
    {
        /* code */
        M4ReceiveFinishFlag = !M4ReceiveFinishFlag;
    }
    
}
