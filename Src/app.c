#include "app.h"

uint16_t adc_val;
uint8_t buff_mic[2] = {0,0};
uint8_t sndBuffOut[129] = {0};
uint8_t sndBuffIn[129] = {0};
uint8_t singleSample;
volatile int enableTx = 0;
volatile audBuffindex = 0;

extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart1;

// Button press callback
void HAL_GPIO_EXTI_Callback(uint16_t _pin){
  if (_pin == TALK_Pin) {
    audBuffindex = 0;
    enableTx = HAL_GPIO_ReadPin(TALK_GPIO_Port, TALK_Pin);
  }
}

// Peridic sampling timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(enableTx)
  {
    static int audBuffindex;
    HAL_ADC_Start(&hadc);
    adc_val = HAL_ADC_GetValue(&hadc);
    // Fill buffer
    if(audBuffindex > 128){
      HAL_UART_Transmit_IT(&huart1,sndBuffOut,sizeof(sndBuffOut));
      audBuffindex = 0;
      HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    }
    else{
        sndBuffOut[audBuffindex++] = adc_val;
        sndBuffOut[audBuffindex++] = adc_val << 8;
    }
  }
}

// UART Rx callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if(singleSample & 128) { // 1st bit, means MSB of single sample
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  }
  else{ // means LSB, means MSB of single sample
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  }
  HAL_UART_Receive_IT(&huart1,&singleSample, 1);
}

// Startup led blink
void blink_led(){
  for (int i=0; i<3; i++){
    HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    HAL_Delay(200);
  }
}

void app_setup(){
    HAL_UART_Receive_IT(&huart1,&singleSample, 1);
}