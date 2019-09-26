#define RCC_BASE 0x40021000
#define RCC_OFFSET 0x18

#define GPIO_PORTB_BASE 0x40010C00
#define GPIO_PORTC_BASE 0x40011000
#define GPIO_PORTD_BASE 0x40011400

#define UP_BUTTON 0x20
#define DOWN_BUTTON 0x4
#define LEFT_BUTTON 0x8
#define RIGHT_BUTTON 0x10
#define SELECT_BUTTON 0x100

#define GPIO_ODR_OFFSET 0x0c
#define GPIO_IDR_OFFSET 0x08
#define GPIO_BSRR_OFFSET 0x10
#define GPIO_CRL_OFFSET 0x00
#define GPIO_CRH_OFFSET 0x04

#define LED1 0x4
#define LED2 0x8
#define LED3 0x10
#define LED4 0x80


int main (){
	int led12, led34 = 0;
	int i = 0;	
	*(volatile unsigned int *)(RCC_BASE + RCC_OFFSET) |= 0xFC; // set Port Clock
	*(volatile unsigned int *)(GPIO_PORTD_BASE + 0x0) = 0x10011100; // set PortD CRL. use output port 2,3,4,7
	*(volatile unsigned int *)(GPIO_PORTB_BASE) = 0x44444448; // set PortB CRH. To use SELECT button
	*(volatile unsigned int *)(GPIO_PORTC_BASE) = 0x44888844; // set PortC CRL. use input port 

	while(1) {
	// *(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = 0xFFFF0000;

		if (~*(volatile unsigned int *)(GPIO_PORTC_BASE + GPIO_IDR_OFFSET) & UP_BUTTON){
			*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) |= 0x10 | 0x80;
			led34 = 1;
		}
	
		else if (~*(volatile unsigned int *)(GPIO_PORTC_BASE + GPIO_IDR_OFFSET) & DOWN_BUTTON){
			*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) |= 0x00000004 | 0x00000008;
			led12 = 1;
		}
		else if (~*(volatile unsigned int *)(GPIO_PORTC_BASE + GPIO_IDR_OFFSET) & LEFT_BUTTON){
			*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = 0xFFF00000;
			led34 = 0;
		}
		else if (~*(volatile unsigned int *)(GPIO_PORTC_BASE + GPIO_IDR_OFFSET) & RIGHT_BUTTON){
			*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = 0xFF0F0000;
			led12 = 0;
		}

		else if ( ~*(volatile unsigned int *)(GPIO_PORTB_BASE + GPIO_IDR_OFFSET) & SELECT_BUTTON){
			if(led12 == 1 && led34 == 0){
				*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = 0xFFFF0000;
				*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = LED3 | LED4;
				led12 = 0;
				led34 = 1;
			}
			else if(led12 == 0 && led34 == 1){
				*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = 0xFFFF0000;
				*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = LED1 | LED2;
				led12 = 1;
				led34 = 0;
			}
			else if(led12 == 0 && led34 == 0){
				*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = LED1 | LED2 | LED3 | LED4;
				led12 = 1;
				led34 = 1;
			}
			else if(led12 == 1 && led34 == 1){
				*(volatile unsigned int *)(GPIO_PORTD_BASE + GPIO_BSRR_OFFSET) = 0xFFFF0000;
				led12 = 0;
				led34 = 0;
			}

		}
		for(i = 0; i<1000000; i++) ;
		//
	}

	return 0;
}
