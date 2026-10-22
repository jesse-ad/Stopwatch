
/*************************************
// EEE2050F Assignment - STM32 code
// Author: Jesse Adams, ADMJES003
// Date: 19 May 2025 
*************************************/


#include "stm32f0xx.h"
#include <lcd_stm32f0.c>
#include <stdbool.h>

// Global variables
bool startFlag = false;
bool lapFlag = false;
bool stopFlag = false;
bool resetFlag = true;
uint8_t minutes = 0;
uint8_t seconds = 0;
uint8_t hundredths = 0; 
uint8_t lap_mins = 0;
uint8_t lap_secs = 0;
uint8_t lap_hundredths = 0;
char res[16]; 

// Function declarations 
void initGPIO();
void initTIM14();
void TIM14_IRQHandler();
void display();
void checkPB();
void convert2BCDASCII(const uint8_t min, const uint8_t sec, const uint8_t hund, char * resultPtr); 

// Initilise pins
void initGPIO(void) {
    // Enable clock for ports A and B 
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN; 

    /* Switches */
    // SW0
    GPIOA->MODER &= ~GPIO_MODER_MODER0; // Clear bits
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0; // Set pull-up resistor
    // SW1
    GPIOA->MODER &= ~GPIO_MODER_MODER1; // Clear bits
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0; // Set pull-up resistor
    // SW2
    GPIOA->MODER &= ~GPIO_MODER_MODER2; // Clear bits
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_0; // Set pull-up resistor
    // SW3
    GPIOA->MODER &= ~GPIO_MODER_MODER3; // Clear bits
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_0; // Set pull-up resistor

    /* LEDS */
    // LED 0
    GPIOB->MODER &= ~GPIO_MODER_MODER0; // Clear bits
    GPIOB->MODER |= GPIO_MODER_MODER0_0; // Set to output mode
    // LED 1
    GPIOB->MODER &= ~GPIO_MODER_MODER1; // Clear bits
    GPIOB->MODER |= GPIO_MODER_MODER1_0; // Set to output mode
    // LED 2
    GPIOB->MODER &= ~GPIO_MODER_MODER2; // Clear bits
    GPIOB->MODER |= GPIO_MODER_MODER2_0; // Set to output mode
    // LED 3
    GPIOB->MODER &= ~GPIO_MODER_MODER3; // Clear bits
    GPIOB->MODER |= GPIO_MODER_MODER3_0; // Set to output mode

    GPIOB->MODER &= ~GPIO_MODER_MODER4; // Clear bits
    GPIOB->MODER |= GPIO_MODER_MODER4_0; // Set to output mode
    // LED 2
    GPIOB->MODER &= ~GPIO_MODER_MODER5; // Clear bits
    GPIOB->MODER |= GPIO_MODER_MODER5_0; // Set to output mode
    // LED 3
    GPIOB->MODER &= ~GPIO_MODER_MODER6; // Clear bits
    GPIOB->MODER |= GPIO_MODER_MODER6_0; // Set to output mode

    GPIOB->MODER &= ~GPIO_MODER_MODER7; // Clear bits
    GPIOB->MODER |= GPIO_MODER_MODER7_0; // Set to output mode
}

// Initilise timer 14
void initTIM14(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN; // Enable clock for timer
    TIM14->PSC = 1; // Slow down timer counting
    TIM14->ARR = 39999; // 1 second
    TIM14->DIER |= TIM_DIER_UIE; // Enable interrupt update 
    TIM14->CR1 |= TIM_CR1_CEN; // Start the timer 

}
// Handles interrupts
void TIM14_IRQHandler()
{
    if (TIM14->SR & TIM_SR_UIF) { // Check UI flag
        TIM14->SR &= ~TIM_SR_UIF; // Clear interrupt flag

        hundredths += 1; 
        if (hundredths == 100) {
            hundredths = 0;
            seconds++;
        }
        if (seconds == 60) {
            seconds = 0;
            minutes++;
        }
        
    }
}

// Displays text on LCD screen
void display(void)
{
    if (!startFlag && !lapFlag && !stopFlag && resetFlag) {
        lcd_putstring("Stopwatch");
        lcd_command(LINE_TWO);
        lcd_putstring("Press SW0..."); 
        GPIOB->BSRR |= GPIO_BSRR_BS_3; 
    }
    if (startFlag && !lapFlag && !stopFlag && !resetFlag) {
        lcd_putstring("Time");
        lcd_command(LINE_TWO);
        convert2BCDASCII(minutes, seconds, hundredths, res);
        lcd_putstring(res);
        GPIOB->BSRR |= GPIO_BSRR_BS_0;
        
    }
    if (!startFlag && lapFlag && !stopFlag && !resetFlag) {
        lcd_putstring("Time");
        lcd_command(LINE_TWO);
        convert2BCDASCII(lap_mins, lap_secs, lap_hundredths, res);
        lcd_putstring(res);
        GPIOB->BSRR |= GPIO_BSRR_BS_1;
       
    }
    if (!startFlag && !lapFlag && stopFlag && !resetFlag) {
        lcd_putstring("Time");
        lcd_command(LINE_TWO);
        convert2BCDASCII(minutes, seconds, hundredths, res);
        lcd_putstring(res);
        GPIOB->BSRR |= GPIO_BSRR_BS_2;
        
    }
}

// Checks which button has been pressed and 
void checkPB(void)
{

    uint16_t pressed = GPIOA->IDR;
    // Check if SW0 pressed - START
    if ((pressed & GPIO_IDR_0) == 0) {
        GPIOB->ODR = 0x00000000;

        startFlag = true;
        lapFlag = false;
        stopFlag = false;
        resetFlag = false;

        GPIOA->ODR |= GPIO_ODR_7;
        NVIC_EnableIRQ(TIM14_IRQn);
    }
    // Check if SW1 pressed - LAP
     if ((pressed & GPIO_IDR_1) == 0) {
        GPIOB->ODR = 0x00000000;

        startFlag = false;
        lapFlag = true;
        stopFlag = false;
        resetFlag = false;

        lap_mins = minutes;
        lap_secs = seconds;
        lap_hundredths = hundredths;

        GPIOA->ODR &= ~GPIO_ODR_7;
    }
    // Check if SW2 pressed - STOP
     if ((pressed & GPIO_IDR_2) == 0) {
        GPIOB->ODR = 0x00000000;

        startFlag = false;
        lapFlag = false;
        stopFlag = true;
        resetFlag = false;

        NVIC_DisableIRQ(TIM14_IRQn);
    }
    // Check if SW3 pressed - RESET
     if ((pressed & GPIO_IDR_3) == 0) {
        GPIOB->ODR = 0x00000000;

        startFlag = false;
        lapFlag = false;
        stopFlag = false;  
        resetFlag = true;

        NVIC_DisableIRQ(TIM14_IRQn);

        minutes = 0;
        seconds = 0;
        hundredths = 0;
    }
}

void convert2BCDASCII(const uint8_t min, const uint8_t sec, const uint8_t hund, char *resultPtr)
{
    // BCD
    uint8_t min_0 = min % 10;
    uint8_t min_1 = min / 10;
    uint8_t sec_0 = sec % 10; 
    uint8_t sec_1 = sec / 10; 
    uint8_t hund_0 = hund % 10; 
    uint8_t hund_1 = hund / 10; 

    // ASCII
    resultPtr[0] = min_1 + 48;
    resultPtr[1] = min_0 + 48;
    resultPtr[2] = ':';
    resultPtr[3] = sec_1 + 48;
    resultPtr[4] = sec_0 + 48;
    resultPtr[5] = '.';
    resultPtr[6] = hund_1 + 48;
    resultPtr[7] = hund_0 + 48;
    resultPtr[8] = '\0';


}

int main (void)
{
  initGPIO();
  initTIM14();
  init_LCD();
  
    
    while (1){
        checkPB();
        lcd_command(CLEAR);
        display();
        delay(10000);
    }

}	

