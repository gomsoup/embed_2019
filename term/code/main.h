#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_exti.h"

#include "misc.h"
#include "core_cm3.h"
#include "lcd.h"

int keypad_input[4] = {-1, -1, -1, -1}; // 키패드 입력 버퍼
int password[4] = {1, 2, 3, 4};         // 비밀번호 배열
volatile uint16_t temp_password[4] = {
    0,
};
char LCD_output[5] = {'\0', '\0', '\0', '\0', '\0'};   // LCD output용 버퍼
int status = 0;       // 0 이면 열림모드, 1 이면 방범모드
int fail_counter = 0; // 비밀번호 틀림 횟수 카운터
int motion_flag = 0;  // 움직임 센서 탐지 여부 (1이면 탐지)
int door_flag = 0, move_send = 0;    // 문 열림 센서 탐지 여부 (1이면 열림상태)
int fail_flag = 0;    // 비밀번호 3회 이상 틀리면 fail_flag = 1
int flag = 1;

int temp_flag = 0;

int i = 0, j = 0, lcd_x=10, lcd_y=10;
int value = -1;
int password_index = -1;

uint16_t Pulse[2] = {2300, 700};

void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configuration();
void EXTI_Configuration();
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);

void TIM_Init();

void delay(int time);
void openDoor();
void closeDoor();
void INPUT_PASSWORD();
void Check_Password();


