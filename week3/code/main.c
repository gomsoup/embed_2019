#include "stm32f10x.h"

#define GPIO_PORTD_BASE 0x40011400
#define vui *(volatile unsigned int *)

void SysInit(void) {
    /* Set HSION bit */
    /* Internal Clock Enable */
    RCC->CR |= (uint32_t)0x00000001; //HSION

    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
    RCC->CFGR &= (uint32_t)0xF0FF0000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;

    /* Reset PLL2ON and PLL3ON bits */
    RCC->CR &= (uint32_t)0xEBFFFFFF;

    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x00FF0000;

    /* Reset CFGR2 register */
    RCC->CFGR2 = 0x00000000;

    // for debug
    //RCC->APB2ENR = 0xfc;
    //vui(GPIO_PORTD_BASE) = 0x10011100;
}

void SetSysClock(void) {
    volatile uint32_t StartUpCounter = 0, HSEStatus = 0;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
        HSEStatus = (uint32_t)0x01;
    }
    else {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01) {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;
        /* Flash 0 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;

//@TODO - 1 Set the clock
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV2;
        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

        /* Configure PLLs ------------------------------------------------------*/
        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 |
                                RCC_CFGR_PLLMULL7);


        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                                  RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8
                                 | RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV10 );

//@End of TODO - 1

        /* Enable PLL2 */
        RCC->CR |= RCC_CR_PLL2ON;
        /* Wait till PLL2 is ready */
        while ((RCC->CR & RCC_CR_PLL2RDY) == 0)
        {
        }
        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;
        /* Wait till PLL is ready */
        while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }
        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
        /* Select System Clock as output of MCO */
//@TODO - 2 Set the MCO port for system clock output

        RCC->CFGR &= ~(uint32_t)RCC_CFGR_MCO;
        RCC->CFGR |= (uint32_t)RCC_CFGR_MCO_SYSCLK;
        // RCC->CFGR |= ??
//@End of TODO - 2


    }
    else {
        /* If HSE fails to start-up, the application will have wrong clock
        configuration. User can add here some code to deal with this error */
    }
}

void RCC_Enable(void) {
//@TODO - 3 RCC Setting
    /*---------------------------- RCC Configuration -----------------------------*/
    /* GPIO RCC Enable  */
    /* UART Tx, Rx, MCO */
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPDEN);

    //RCC->APB2ENR |= ??
    /* USART RCC Enable */

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    // RCC->APB2ENR |= ??
}

void GPIO_Configuration(void) {
//@TODO - 4 GPIO Configuration
    /* Reset Port A CRH */
    GPIOA->CRH = 0x44444444;

    // GPIOA->CRH &= ??
    /* Reset Port B CRH */
    GPIOB->CRH = 0x44444444;
    GPIOD->CRH = 0x44444444;
    // GPIOB->CRH &= ??

    /* MCO Pin Configuration */
    GPIOA->CRH = 0x444444BB;

// GPIOA->CRH |= ??
    /* USART Pin Configuration */
    // GPIOA->CRH |= ??
    /* Joy-stick SELECT Button Configuration */
    GPIOB->CRH = 0x44444441;
    GPIOD->CRH = GPIO_CRH_CNF11_1;

    // GPIOB->CRH |= ??
}

void GPIO_Pin_InitSetting(void) {
//@TODO - 5 GPIO Set & Reset
    /* Enable MCO */
    // GPIOA->BSRR |= ??
    GPIOA->BSRR |= GPIO_BSRR_BS8;

    /* Joy-stick Button Reset */
    // GPIOB->BRR |= ??
    GPIOB->BSRR |= GPIO_BSRR_BS8;
}

void UartInit(void) {
    /*---------------------------- USART CR1 Configuration -----------------------*/
    /* Clear M, PCE, PS, TE and RE bits */
    USART1->CR1 &= ~(uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE);
    /* Configure the USART Word Length, Parity and mode ----------------------- */
    /* Set the M bits according to USART_WordLength value */
//@TODO - 6: WordLength : 8bit
    USART1->CR1 &= ~(USART_CR1_M);
    /* Set PCE and PS bits according to USART_Parity value */
//@TODO - 7: Parity : None
    USART1->CR1 &= ~(USART_CR1_PCE);
    /* Set TE and RE bits according to USART_Mode value */
//@TODO - 8: Enable Tx and Rx
    // USART1->CR1 |= ??
    USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE);

    /*---------------------------- USART CR2 Configuration -----------------------*/
    /* Clear STOP[13:12] bits */
    USART1->CR2 &= ~(uint32_t)(USART_CR2_STOP);
    /* Configure the USART Stop Bits, Clock, CPOL, CPHA and LastBit ------------*/
    USART1->CR2 &= ~(uint32_t)(USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_CLKEN);
    /* Set STOP[13:12] bits according to USART_StopBits value */
//@TODO - 9: Stop bit : 1bit
    USART1->CR2 &= ~(USART_CR2_STOP);

    /*---------------------------- USART CR3 Configuration -----------------------*/
    /* Clear CTSE and RTSE bits */
    USART1->CR3 &= ~(uint32_t)(USART_CR3_CTSE | USART_CR3_RTSE);
    /* Configure the USART HFC -------------------------------------------------*/
    /* Set CTSE and RTSE bits according to USART_HardwareFlowControl value */
//@TODO - 10: CTS, RTS : disable
    USART1->CR3 |= USART_CR3_CTSIE;
    USART1->CR3 &= ~(USART_CR3_CTSE);
    USART1->CR3 &= ~(USART_CR3_RTSE);

    /*---------------------------- USART BRR Configuration -----------------------*/
    /* Configure the USART Baud Rate -------------------------------------------*/
    /* Determine the integer part */
    /* Determine the fractional part */
//@TODO - 11: Calculate & configure BRR
    // USART1->BRR |= ??
    USART1->BRR |= 0x1e6;
    /*---------------------------- USART Enable ----------------------------------*/
    /* USART Enable Configuration */
//@TODO - 12: Enable UART
    // USART1->CR1 |= ??
    USART1->CR1 |= USART_CR1_UE;
}

void delay(void) {
    int i = 0;
    for(i=0; i<1000000; i++);
}

void SendData(int data) {
    int i = 0;
    USART1->DR = data & 0xFF;
    delay();
}

void send() {
    while(1) {
        if(~(GPIOD->IDR) & GPIO_IDR_IDR11)
        {
            SendData(0x48); // H
            SendData(0x65); // e
            SendData(0x6c); // l
            SendData(0x6c); // l
            SendData(0x6f); // o
            SendData(0x54); // T
            SendData(0x65); // e
            SendData(0x61); // a
            SendData(0x6d); // m
            SendData(0x30); // 0
            SendData(0x37); // 7
            SendData(0x0d);
            SendData(0x0a);
            SendData(0x00);
        }
        delay();
    }
}