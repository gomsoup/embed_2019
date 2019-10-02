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
#define USR_SEL 0x800

#define MOVE_F 0x3000000
#define MOVE_L 0x1000000
#define MOVE_R 0x2000000

#define vsi *(volatile unsigned int *)

int main () {
    unsigned int i = 0;
    vsi(RCC_BASE + RCC_OFFSET) |= 0x38; // set Port Clock B, C, D
    vsi(GPIO_PORTC_BASE) = 0x44488844; // set PortC 2, 3, 4, 5 as input
    vsi(GPIO_PORTC_BASE + GPIO_CRH_OFFSET) = 0x44444411; // set PortC 8, 9 as output
    vsi(GPIO_PORTD_BASE + GPIO_CRH_OFFSET) = 0x44448444; // set PORTD 11 as input

    while(1) {
        if(~vsi(GPIO_PORTD_BASE + GPIO_IDR_OFFSET) & USR_SEL) { // user button pressed -> Random
            switch(i % 3) {
            case 0:
                *(volatile unsigned int *)(GPIO_PORTC_BASE + GPIO_BSRR_OFFSET) = MOVE_F;
                break;
            case 1:
                *(volatile unsigned int *)(GPIO_PORTC_BASE + GPIO_BSRR_OFFSET) = MOVE_L;
                break;
            case 2:
                *(volatile unsigned int *)(GPIO_PORTC_BASE + GPIO_BSRR_OFFSET) = MOVE_R;
                break;
            }
            for(i = 0; i < 5000000; i++) {}
        }
        else if(~vsi(GPIO_PORTC_BASE + GPIO_IDR_OFFSET)& UP_BUTTON) { // up button pressed -> move Forward
            vsi(GPIO_PORTC_BASE + GPIO_BSRR_OFFSET) = MOVE_F;
            for(i = 0; i < 5000000; i++) {}
        }
        else if(~vsi(GPIO_PORTC_BASE + GPIO_IDR_OFFSET) & LEFT_BUTTON) { // Left button pressed -> Left turn
            vsi(GPIO_PORTC_BASE + GPIO_BSRR_OFFSET) = MOVE_L;
            for(i = 0; i < 5000000; i++) {}
        }
        else if(~vsi(GPIO_PORTC_BASE + GPIO_IDR_OFFSET) & RIGHT_BUTTON) { // Right button pressed -> Right turn
            vsi(GPIO_PORTC_BASE + GPIO_BSRR_OFFSET) = MOVE_R;
            for(i = 0; i < 5000000; i++) {}
        }


        vsi(GPIO_PORTC_BASE + GPIO_BSRR_OFFSET) |= 0x300; // reset
        i++; // random count
    }

    return 0;
}