#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void USART1_Init(void);
void NVIC_Configure(void);

void EXTI15_10_IRQHandler(void);

void Delay(void);

void sendDataUART1(uint16_t data);

#define ON_UP 10
#define OFF 0
#define ON_DOWN 11
unsigned int type = OFF;

//---------------------------------------------------------------------------------------------------
void RCC_Configure(void)
{
    // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPCEN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPDEN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
    /* JoyStick up, down pin setting */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5; // up / down
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // input pushpull don't care
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* button pin setting */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // select -> PB8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // input push-pull don't care
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* LED pin setting*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* UART pin setting */
    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //  TX Enable -> PA9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // Alternative function push-pull output
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // RX Enable -> PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // TODO: Select the GPIO pin (Joystick, button, LED) used as EXTI Line using function 'GPIO_EXTILineConfig'
    // do not select the UART GPIO pin used as EXTI Line here

    //joystick
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);

    //button
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);

    // Tx & Rx
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11); //user s1

}

void EXTI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    // TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'
    // Joystick Down
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Joystick Up
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Select Button
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // User Select Button
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void NVIC_Configure(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // preemptive up / down / select / rx

    // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
    // Joystick Down
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Joystick Up
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Select
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // User Select
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // UART1
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void sendDataUART1(uint16_t data) {
    USART_SendData(USART1, data);

    // Wait till TC is set
    while ((USART1->SR & USART_SR_TC) == 0);

}



void USART1_IRQHandler() {
    uint16_t word;
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){

        // the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);

        // TODO implement
        if (word == 't'){
            if (type == OFF) type = ON_UP;
            else type = OFF;
        }
        else if (word == 'u') type = ON_UP;
        else type = ON_DOWN;

        // clear 'Read data register not empty' flag
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }

}

void EXTI15_10_IRQHandler(void){

    if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
        // TODO implement
        sendDataUART1('T');
        sendDataUART1('E');
        sendDataUART1('A');
        sendDataUART1('M');
        sendDataUART1('0');
        sendDataUART1('7');
        sendDataUART1('\r');
        sendDataUART1('\n');

        USART_ClearITPendingBit(USART1, USART_IT_TXE);
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
}

void EXTI2_IRQHandler(void){
    if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
        // TODO implement
        type = ON_DOWN;

        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

void EXTI9_5_IRQHandler(void) { // when the button is pressed
    if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
        // TODO implement
        type = ON_UP;

        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    else if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
        // TODO implement
        if(type == OFF) type = ON_UP;
        else type = OFF;

        EXTI_ClearITPendingBit(EXTI_Line8);
    }

}

void USART1_Init(void)
{
    USART_InitTypeDef USART1_InitStructure;

    // Enable the USART1 peripheral
    USART1_InitStructure.USART_BaudRate = 28800;
    USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART1_InitStructure.USART_StopBits = USART_StopBits_1;
    USART1_InitStructure.USART_Parity = USART_Parity_No;
    USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART1_InitStructure.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);

    // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
    USART_Init(USART1, &USART1_InitStructure);

    // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
    USART_Cmd(USART1, ENABLE);
}

void Delay(void) {
    int i;
    for (i = 0; i < 2000000; i++) {}
}

int main(void)
{
    unsigned int current_pos = 0;
    unsigned int onoff = 1;

    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    USART1_Init();
    EXTI_Configure();
    NVIC_Configure();

    while (1) {
        // TODO: implement
        if(type == ON_UP){
            if(current_pos >= 3) current_pos = 0;
            else current_pos += 1;
            onoff = 1;
        }
        else if(type == ON_DOWN){
            if(current_pos <= 0) current_pos = 3;
            else current_pos -= 1;
            onoff = 1;
        }
        else if (type == OFF){
            onoff = 0;
        }

        if(onoff == 1){
            switch(current_pos){
                case 0:{
                    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
                    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
                    GPIO_ResetBits(GPIOD, GPIO_Pin_7);
                    GPIO_SetBits(GPIOD, GPIO_Pin_2);
                    break;
                }
                case 1:{
                    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
                    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
                    GPIO_ResetBits(GPIOD, GPIO_Pin_7);
                    GPIO_SetBits(GPIOD, GPIO_Pin_3);
                    break;
                }
                case 2:{
                    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
                    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
                    GPIO_ResetBits(GPIOD, GPIO_Pin_7);
                    GPIO_SetBits(GPIOD, GPIO_Pin_4);
                    break;
                }
                case 3:{
                    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
                    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
                    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
                    GPIO_SetBits(GPIOD, GPIO_Pin_7);
                    break;
                }
            }
        }
        else{
            GPIO_ResetBits(GPIOD, GPIO_Pin_2);
            GPIO_ResetBits(GPIOD, GPIO_Pin_3);
            GPIO_ResetBits(GPIOD, GPIO_Pin_4);
            GPIO_ResetBits(GPIOD, GPIO_Pin_7);
        }

        Delay();
    }

    return 0;
}