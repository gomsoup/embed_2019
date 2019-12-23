#include <string.h>
#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "core_cm3.h"
#include "lcd.h"
//#include "main.h"

uint16_t buf[500]= { 0,};
extern uint16_t password[4];
extern uint16_t temp_password[4];

int temp_req_flag = 0;
int temp_flag = 0;
int i = 0, cnt = 0;

// Configure Clock, Interrupt
void USART_Configure();
void ServerTimer_Configure();

// Configure Handler
void TIM2_IRQHandelr();
void USART1_IRQHandler();
void USART2_IRQHandler();

// USART Communication Configure
void sendUSART1(uint16_t data);
void sendUSART2(uint16_t data);

void sendTTY(char *str);
void sendWIFI(char *str);

// Wifi module parsing functions
void waitOK();
void wait(uint16_t str);
void recvWIFI(char *str);
void recvWIFIWithoutClose(char *str);
void requestWIFI(char *cmd);
void requestWIFICurrentSocket(char *cmd);
void WIFI_Init();

// Server-Client Communication Configure
void getPassword();
void getTempPassword();
void updatePassword(char* new_password);
void removeTempPassword();
void urgentPush();
void urgentDoorPush();
void PasswordWrongPush();
