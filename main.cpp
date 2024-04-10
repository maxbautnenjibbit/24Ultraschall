#include "mbed.h"
#include "LCD.h"

lcd mylcd;
DigitalOut trigger(PA_10);
InterruptIn echo(PC_8);

bool sendTrigger = false;
float abstand = 0;

void tim6isr() {
    TIM6->SR=0;
}

void startReceive() {
    TIM6->CNT = 0;
}

void stopReceive() {
    float timeDur = TIM6->CNT;

    abstand = timeDur / 58;

    sendTrigger = false;
}

void init() {
    RCC->APB1ENR|=0b10000;
    TIM6->PSC=31;
    TIM6->ARR=0xFFFF;
    TIM6->SR=0;
    TIM6->CNT=0;
    TIM6->CR1=1;

    NVIC_SetVector(TIM6_IRQn, (uint32_t) &tim6isr);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);


}

int main()
{
    init();

    echo.mode(PullDown);
    echo.rise(&startReceive);
    echo.fall(&stopReceive);
    echo.enable_irq();

    mylcd.clear();

    while (true) {
        mylcd.cursorpos(0);
        mylcd.printf("%f", abstand);

        if (!sendTrigger) {
            trigger = 1;
            sendTrigger = true;
            TIM6->CNT = 0;
            while (TIM6->CNT < 10);
            trigger = 0;
        }
    }
}

