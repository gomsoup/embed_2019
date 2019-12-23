#include "Wifi.h"


/* Configure USART related Clock
    RCC
    GPIOA 2, 3, 9, 10
    NVIC
    USART1, 2 BaudRate 115200 */
void USART_Configure(){
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;  
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN, ENABLE);


    //USART1,2 Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1,2 RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //USART2
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //USART1 Init
    USART_Cmd(USART1, ENABLE);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    //USART2 Init   
    USART_Cmd(USART2, ENABLE);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);

    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}


/*  Configure Timer for Update Temp Password
    NVIC
    TIM2 1ms */

void ServerTimer_Configure(){
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseStructure.TIM_Period= 1000 -1; // 1ms
    TIM_TimeBaseStructure.TIM_Prescaler=72-1;
    TIM_TimeBaseStructure.TIM_ClockDivision=0;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

    TIM_Cmd(TIM2,ENABLE);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}

/*  USART1 Handler for debugging.
    Echo USART2 Rx */
void USART1_IRQHandler(){
    uint16_t word;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        word = USART_ReceiveData(USART1);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        USART_SendData(USART1, word);
    }

    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

/*  USART2 Handler. Communicate with Wifi Module
    when Rx is not empty, read Rx and fill buffer buf
    buf will be used parsing part */
void USART2_IRQHandler()
{
    uint16_t word;

    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        word = USART_ReceiveData(USART2);

        if(cnt > 100) cnt = 0;

        // when come with new line, clear buffer
        if (word == '\n')
        {
            cnt++;
            buf[cnt] = 0;
            cnt = 0;
        }
        else
        {
            buf[cnt] = word;
            cnt++;
        }

        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}
        USART_SendData(USART1, word);
        while ((USART2->SR & USART_SR_TC) == 0){}
    }

    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

/*  Timer2 Hander for continuously update temp password.
    temp_req_flag and temp_flag will be used in main.c */
void TIM2_IRQHandler(){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		temp_req_flag += 1;
		temp_flag += 1;
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

/*  Send char to USART1 */
void sendUSART1(uint16_t data)
{
    // wait Tx is clear
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}

    USART_SendData(USART1, data);

    // make Tx clear flag to 0
    while ((USART1->SR & USART_SR_TC) == 0) {}
}

/*  Send char to USART2 */
void sendUSART2(uint16_t data)
{
    // wait Tx is clear
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) {}

    USART_SendData(USART2, data);

    // make Tx clear flag to 0
    while ((USART2->SR & USART_SR_TC) == 0) {};
}

/*  Send string to USART1 for debug */
void sendTTY(char *str)
{
    int len = strlen(str);

    for (i = 0; i < len; i++){
        sendUSART1((uint16_t)str[i]);
    }
}

/*  Send AT Command to USART1 for set Wifi Module */
void sendWIFI(char *str)
{
    int len = strlen(str);

    for (i = 0; i < len; i++){
        sendUSART2((uint16_t)str[i]);
    }

    // AT Commandd must be end with \r\n
    sendUSART2('\r');
    sendUSART2('\n');
}

/*  To parse OK Command.
    When AT command properly be executed, Wifi Module return String 'OK'.
    If more AT command given while previous command is executed, Wifi module return String 'busy s...' and drop given command cus it hasn't buffer.
    So should wait previous command is executed */
void waitOK(){
	while(1){
		if(buf[0] == 'O' && buf[1] == 'K') { 
            cnt = 0; 
            memset(buf, 0, 500); 
            break; 
        }

		delay(100);
	}
}

/*  To parse specific char. 
    When give 'AT+CIPSEND' command, Wifi Module return char '>'
    For same reason waitOK() we should wait that char.  */
void wait(uint16_t str){
	while(1){
		if ( buf[0] == str) {
		    cnt = 0; 
            memset(buf, 0, 500); 
            break; 
        }

		delay(100);
	}
}

/*  To parse '+IPD,' string
    When give 'AT+CIPSEND' is executed and server return some string, Wifi Module return server response with '+IPD,[num of recv bytes]:[Server response]'
    For same reason for waitOK(), we should wait that char. */
void recvWIFI(char *str){
   if (buf[0] == '+' && buf[1] == 'I' && buf[2] == 'P' && buf[3] == 'D'){
      str[0] = buf[7];
      str[1] = buf[8];
      str[2] = buf[9];
      str[3] = buf[10];
   }
   else
      str = "Fail";

   sendWIFI("AT+CIPSEND=4");
   wait('>');
   delay(100);

   // Close Socket
   sendNoLine("CLOS");
   delay(200);
}

/*  Same functioning recvWIFI(). But, not close socket. */
void recvWIFIWithoutClose(char *str){
   if (buf[0] == '+' && buf[1] == 'I' && buf[2] == 'P' && buf[3] == 'D'){
      str[0] = buf[7];
      str[1] = buf[8];
      str[2] = buf[9];
      str[3] = buf[10];
      str[4] = '\0';
   }
   else
      str = "Fail";
}

/*  Open socket and send some string to server */
void requestWIFI(char *cmd){
	sendWIFI("AT+CIPSTART=\"TCP\",\"server.gomsoup.com\",4000");
	delay(100);
	waitOK();

	sendWIFI("AT+CIPSEND=4");
	delay(100);
	wait('>');
	delay(100);

	sendNoLine(cmd);
	delay(100);
}

/* Send some string to current socket */
void requestWIFICurrentSocket(char *cmd){
	sendWIFI("AT+CIPSEND=4");
	delay(100);
	wait('>');
	delay(100);

	sendNoLine(cmd);
	delay(100);
}

/*  Initialize Wifi module  */
void WIFI_Init(){
    // Station mode
    sendWIFI("AT+CWMODE=1");
    waitOK();

    // Connect to AP
    sendWIFI("AT+CWJAP=\"ABC\",\"123456789a\"");
    waitOK();

    // Single communication mode
    sendWIFI("AT+CIPMUX=0");
    waitOK();

    // Get Status
    sendWIFI("AT+CIPSTATUS");
    waitOK();
}

void getPassword(){
   char str[4];
   requestWIFI("PASS");
   delay(100);
   recvWIFI(str);


   if (strcmp("Fail", str)){
      password[0] = str[0]-48;
      password[1] = str[1]-48;
      password[2] = str[2]-48;
      password[3] = str[3]-48;
   }
}

void getTempPassword(){
   char pass[4];
   requestWIFI("REQT");
   recvWIFI(pass);


   if (strcmp("Fail", pass)){
      temp_password[0] = pass[0]-48;
      temp_password[1] = pass[1]-48;
      temp_password[2] = pass[2]-48;
      temp_password[3] = pass[3]-48;
   }
}

void updatePassword(char* new_passwd){
	char str[4];
	requestWIFI("UDAT");
	recvWIFIWithoutClose(str);
	sendTTY("recv PSWD : ");
	sendTTY(str);
	sendTTY("\r\n");

	if(!strcmp("PSWD", str)) 
        requestWIFICurrentSocket(new_passwd);
	else 
        LCD_ShowString(10,30, "Update Password Failed", BLACK, WHITE);

	recvWIFI(str);
}

void removeTempPassword(){
	char str[4];
	requestWIFI("DELT");
	recvWIFI(str);
}

void urgentPush(){
	char str[4];
	requestWIFI("URGT");
	recvWIFI(str);
}

void urgentDoorPush(){
	char str[4];
	requestWIFI("URDR");
	recvWIFI(str);
}

void passwordWrongPush(){
	char str[4];
	requestWIFI("OVRP");
	recvWIFI(str);
}