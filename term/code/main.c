#include "main.h"
#include "Wifi.h"

void RCC_Configure(void)
{
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPDEN, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPCEN, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPEEN, ENABLE);
}

void GPIO_Configure(void)
{
   // 4개는 output, 4개는 input
   GPIO_InitTypeDef GPIO_InitStructure;

   // input
   GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_11
   | GPIO_Pin_12); // 이렇게 선언해도 되는건지는 잘 모르겠음
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
   // pull-down down일 때 low
   // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);


   // output
   GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6
   | GPIO_Pin_7);
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   // led 1 테스트용
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4
   | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOD, &GPIO_InitStructure);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
   GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

   // PIR sensor
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOC, &GPIO_InitStructure);

   //Door sensor
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
   GPIO_Init(GPIOC, &GPIO_InitStructure);

   GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
}

void NVIC_Configuration()
{

   NVIC_InitTypeDef NVIC_InitStructure;

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

   NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

   NVIC_Init(&NVIC_InitStructure);

   NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

   NVIC_Init(&NVIC_InitStructure);
}

void EXTI_Configuration()
{

   EXTI_InitTypeDef EXTI_InitStructure;

   EXTI_InitStructure.EXTI_Line = EXTI_Line1;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

   EXTI_Init(&EXTI_InitStructure);

   EXTI_InitStructure.EXTI_Line = EXTI_Line2;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

   EXTI_Init(&EXTI_InitStructure);
}

void EXTI1_IRQHandler(void)
{

   if (EXTI_GetITStatus(EXTI_Line1) != RESET)
   { /* motion sensing */

      if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1))
      {
        if(status == 1){
           GPIO_SetBits(GPIOD, GPIO_Pin_2);
             motion_flag = 1;
            /* 긴급모드 signal */
        }
         /* signal "people exist" */
      }
      else if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1))
      {

         motion_flag = 0;
         GPIO_ResetBits(GPIOD, GPIO_Pin_2);
      }
      EXTI_ClearITPendingBit(EXTI_Line1);
   }
}

void EXTI2_IRQHandler(void)
{

   if (EXTI_GetITStatus(EXTI_Line2) != RESET)
   {

      /* signal "door open" */
      if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2))
      {
        if(status == 1){
           GPIO_SetBits(GPIOD, GPIO_Pin_3);
           /* 긴급모드 signal */

        }
         /* signal "door open" */
         door_flag = 1;

      }
      else if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2))
      {

         GPIO_ResetBits(GPIOD, GPIO_Pin_3);
         door_flag = 0;
         /* signal "door close" */
      }

      EXTI_ClearITPendingBit(EXTI_Line2);
   }
}

void delay(int time)
{
   int i;
   int delay_time = 50000 * time;
   for (i = 0; i < delay_time; i++){}
}


/*
 노란선 : PC9
 빨간선 : VCC
 갈색선 : Ground
 */

void openDoor()
{
   TIM_OCInitTypeDef TIM_OCInitStructure;
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = Pulse[0];
   TIM_OC4Init(TIM3, &TIM_OCInitStructure);
}

void closeDoor()
{
   TIM_OCInitTypeDef TIM_OCInitStructure;
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = Pulse[1];
   TIM_OC4Init(TIM3, &TIM_OCInitStructure);
}

void TIM_Init()
{

   TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

   TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
   TIM_ARRPreloadConfig(TIM3, ENABLE);
   TIM_Cmd(TIM3, ENABLE);

   TIM_TimeBaseStructure.TIM_Period = 20000 - 1;
   TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(SystemCoreClock / 1000000) - 1;
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
}

void INPUT_PASSWORD()
{


   GPIO_ResetBits(GPIOA, GPIO_Pin_4);
   GPIO_SetBits(GPIOA, GPIO_Pin_5);
   GPIO_SetBits(GPIOA, GPIO_Pin_6);
   GPIO_SetBits(GPIOA, GPIO_Pin_7);
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
   value = 'D';
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)) {
   value = '#';
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)) {
   value = 0;
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)) {
   value = '*';
   delay(10);
   }

   GPIO_SetBits(GPIOA, GPIO_Pin_4);
   GPIO_ResetBits(GPIOA, GPIO_Pin_5);
   GPIO_SetBits(GPIOA, GPIO_Pin_6);
   GPIO_SetBits(GPIOA, GPIO_Pin_7);
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
   value = 'C';
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)) {
   value = 9;
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)) {
   value = 8;
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)) {
   value = 7;
   delay(10);
   }

   GPIO_SetBits(GPIOA, GPIO_Pin_4);
   GPIO_SetBits(GPIOA, GPIO_Pin_5);
   GPIO_ResetBits(GPIOA, GPIO_Pin_6);
   GPIO_SetBits(GPIOA, GPIO_Pin_7);
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
   value = 'B';
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)) {
   value = 6;
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)) {
   value = 5;
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)) {
   value = 4;
   delay(10);
   }

   GPIO_SetBits(GPIOA, GPIO_Pin_4);
   GPIO_SetBits(GPIOA, GPIO_Pin_5);
   GPIO_SetBits(GPIOA, GPIO_Pin_6);
   GPIO_ResetBits(GPIOA, GPIO_Pin_7);
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
   value = 'A';
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)) {
   value = 3;
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)) {
   value = 2;
   delay(10);
   }
   while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)) {
   value = 1;
   delay(10);
   }

   if ((value >= 0 && value < 10) || value == 'A' || value == 'B' || value == 'C' || value == 'D')
      {
         password_index = (password_index + 1) % 4;
         // 10자 넘어가면 어떤 방식으로 신호를 줌
         keypad_input[password_index] = value;
      }


   for (i = 0; i<4 ; i++) {
        if(keypad_input[i] == -1)
           LCD_output[i]= '\0';
        else if (keypad_input[i] == 'A' || keypad_input[i] == 'B' || keypad_input[i] == 'C' || keypad_input[i] == 'D' )
           LCD_output[i] = keypad_input[i];
        else {
           LCD_output[i] = keypad_input[i] + 48;
        }
      }

   LCD_ShowString(30, 120, "Your PW: ", BLACK, WHITE);
   LCD_ShowString(120, 120, LCD_output, BLACK, WHITE);
}




void Check_Password()
{

	int check = 0;
   // 현재까지 입력 내용(keypad_input) LCD 화면에 출력

   //LCD_ShowCharString(10, 100, LCD_output, BLACK, WHITE);
   //LCD_ShowString(10, 100, LCD_output, BLACK, WHITE);

   if (value == '*')
   {
      // 패스워드 입력 종료
      password_index = -1;
      for (i = 0; i < 4; i++) {
         LCD_output[i] = '\0';
      }

      for (i = 0; i < 4; i++)
      {
         if ( (keypad_input[i] != password[i]) && (keypad_input[i] != temp_password[i]))
         {
            fail_counter++;

            for (j = 0; j < 4; j++)
            {
               keypad_input[j] = -1;
               LCD_output[j] = '\0';
               LCD_ShowString(60, 120, LCD_output, BLACK, WHITE);
               LCD_Clear(WHITE);
            } // 입력 버퍼 초기화
            return;
         }

      }

      if((keypad_input[0] == 0 && keypad_input[1] == 0 && keypad_input[2] == 0 && keypad_input[3] == 0) &&(temp_password[0] == 0 && temp_password[1] == 0 && temp_password[2] == 0 && temp_password[3] == 0)){
    	  fail_counter++;
          for (j = 0; j < 4; j++)
          {
             keypad_input[j] = -1;
             LCD_output[j] = '\0';
             LCD_ShowString(60, 120, LCD_output, BLACK, WHITE);
             LCD_Clear(WHITE);
          } // 입력 버퍼 초기화

          return;
      }

    	  /* 비밀번호 맞았을때 */
      for (j = 0; j < 4; j++)
      {
         keypad_input[j] = -1;
      } // 입력 버퍼 초기화
      LCD_ShowString(60, 120, LCD_output, BLACK, WHITE);
      status = 0;
      fail_flag = 0;
      fail_counter = 0;
      openDoor();
   }

   if (fail_counter >= 3)
   {
	   passwordWrongPush();
	   fail_counter = 0;
      /* 비밀번호 실패 */
   }
}



int main(void)
{
   int p = 0;
   char str[4], str2[4];
   int buf_index;
   SystemInit();
   RCC_Configure();
   GPIO_Configure();
   TIM_Init();
   USART1_Init();
   USART2_Init();
   NVIC_Configuration();
   EXTI_Configuration();
   LCD_Init();
   LCD_Clear(WHITE);
   ServerTimer_Configure();

   LCD_ShowString(50, 50, "Now Initializing...", BLACK, WHITE);
   delay(10);
   WIFI_Init();

   getPassword();
   getTempPassword();
   LCD_Clear(WHITE);


   while (1)
   {
	   if(p != status){
		   LCD_Clear(WHITE);
		   p = status;
	   }

	      if(door_flag == 1) LCD_ShowString(30, 30, "Door : Open", BLACK, WHITE);
	      else LCD_ShowString(30, 30, "Door : Close",  BLACK, WHITE);
	      if(motion_flag == 1) LCD_ShowString(30, 50, "PIR : Detected",BLACK, WHITE);
	      else LCD_ShowString(30, 50, "PIR : Not Detected", BLACK, WHITE);
	      if(status == 1) LCD_ShowString(30, 70, "Lock : Locked", BLACK, WHITE);
	      else LCD_ShowString(30, 70, "Lock : Open", BLACK, WHITE);

	      if(status == 1 && motion_flag == 1) LCD_ShowString(10, 90, "STATUS : Urgent!", RED, WHITE);
	      else if (door_flag == 1 && status == 1) LCD_ShowString(10, 90, "STATUS : Urgent!", RED, WHITE);
	      else LCD_ShowString(30, 90, "STATUS : Normal", BLACK, WHITE);
	      LCD_ShowString(30,150, "Current PW: ", BLACK, WHITE);
	      LCD_ShowNum(120,150, password[0], 1, BLACK, WHITE);
	      LCD_ShowNum(130,150, password[1],1, BLACK, WHITE);
	      LCD_ShowNum(140,150, password[2], 1, BLACK, WHITE);
	      LCD_ShowNum(150,150, password[3], 1, BLACK, WHITE);

	      if(temp_password[0] == 0 && temp_password[1] == 0 && temp_password[2] == 0 && temp_password[3] == 0){
	    	  LCD_ShowString(120,120, "None", BLACK, WHITE);
	      }else{
	      LCD_ShowString(30,170, "Temp PW: ", BLACK, WHITE);
	      LCD_ShowNum(120,170, temp_password[0], 1, BLACK, WHITE);
	      LCD_ShowNum(130,170, temp_password[1],1, BLACK, WHITE);
	      LCD_ShowNum(140,170, temp_password[2], 1, BLACK, WHITE);
	      LCD_ShowNum(150,170, temp_password[3], 1, BLACK, WHITE);
	      }
      value = -1;
      if (status == 0)
      {
         GPIO_SetBits(GPIOD, GPIO_Pin_4);
         GPIO_ResetBits(GPIOD, GPIO_Pin_7);
         // 문열림 상태일 때
         INPUT_PASSWORD();

         if (value == '#')
         {
            if (door_flag == 0)
            {
               closeDoor();
               move_send = 1;
               status = 1;
            }
         }

         if (value == '*')
         { // 문 열림 상태에서 #을 누르면 비밀번호 변경 상태로 감
            for (i = 0; i < 4; i++)
            {
               password[i] = -1;
            } // 패스워드 초기화
            password_index = -1;

            buf_index = -1;
            while (1)
            {
               value = -1;
               INPUT_PASSWORD();

               if ((value >= 0 && value < 10) || value == 'A' || value == 'B' || value == 'C' || value == 'D')
               { // #이랑 * 빼고 모든 문자
                  buf_index = (buf_index + 1) % 4;
                  password[buf_index] = value;
               }
               if (value == '*')
               {
            	  str[0] = password[0]+48; str[1] = password[1]+48; str[2] = password[2]+48; str[3] = password[3]+48;
            	  updatePassword(str);
                  break;
               } // 비밀번호 변경 상태에서 다시 * 입력되면 비밀번호 변경 끝

               // LCD 화면에 패스워드 입력 내용 출력
            }
         }
      }
      else if (status == 1)
      { // 문 잠김 (방범모드)

         GPIO_SetBits(GPIOD, GPIO_Pin_7);
         GPIO_ResetBits(GPIOD, GPIO_Pin_4);
         if (fail_flag == 1)
         {
         }
         if (door_flag == 1)
         {
             urgentDoorPush();
             door_flag = 0;
         }
         if (motion_flag == 1 && move_send == 1)
         {
            move_send = 0;
            urgentPush();
         }

         INPUT_PASSWORD();
         Check_Password();
      }

      if(temp_req_flag > 60000){
    	  temp_req_flag = 0;
    	  getTempPassword();
      }
      if(temp_flag > 300000){
    	  temp_flag = 0;
    	  removeTempPassword();
      }
   }


   return 0;
}