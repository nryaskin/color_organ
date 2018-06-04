#include "visual.h"
#include "microphone_driver.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#define MICROPHONE 1

volatile uint8_t calc_ones_arr[256] = {0,1,1,3,1,2,2,3,1,2,2,3,2,3,3,4,
    1,2,2,4,2,3,3,4,2,3,3,4,3,4,4,5,
    1,2,2,4,2,3,3,4,2,3,3,4,3,4,4,5,
    3,4,4,6,4,5,5,6,4,5,5,6,5,6,6,7,
    1,2,2,4,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,5,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,5,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,6,4,5,5,6,4,5,5,6,5,6,6,7,
    1,2,2,4,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,5,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,5,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,6,4,5,5,6,4,5,5,6,5,6,6,7,
    2,3,3,5,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,6,4,5,5,6,4,5,5,6,5,6,6,7,
    3,4,4,6,4,5,5,6,4,5,5,6,5,6,6,7,
    4,5,5,7,5,6,6,7,5,6,6,7,6,7,7,8};


#define MIC_FILTER_RESULT_LENGTH    16
#define INTERNAL_BUFFER_SIZE        64
#define NOISE_FLOOR                 1000

volatile int16_t rec_buf_0[MIC_FILTER_RESULT_LENGTH];
volatile int16_t rec_buf_1[MIC_FILTER_RESULT_LENGTH];

uint16_t mic_DMA_PDM_Buffer0[INTERNAL_BUFFER_SIZE];
uint16_t mic_DMA_PDM_Buffer1[INTERNAL_BUFFER_SIZE];

uint8_t res_data[INTERNAL_BUFFER_SIZE];
volatile uint32_t result = 0;

int main(void) 
{
    init_rgb_leds();
#if MICROPHONE
    MP4SDT02_I2S_init((uint32_t)mic_DMA_PDM_Buffer0, (uint32_t)&mic_DMA_PDM_Buffer1, (uint32_t)INTERNAL_BUFFER_SIZE);
#endif
    GPIO_SetBits(GPIOD, LED_GREEN);
    for (;;) 
    {
    }
}
#if MICROPHONE
void DMA1_Stream3_IRQHandler(void)
{
    uint8_t i;
    uint8_t temp;
    uint32_t floor_tmp = NOISE_FLOOR;
    GPIO_ResetBits(GPIOD, LED_RED);
    if (DMA_GetFlagStatus(DMA1_Stream3, DMA_FLAG_TCIF3) != RESET)
    {
        DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3);

        if ((DMA1_Stream3->CR & DMA_SxCR_CT) == 0)
        {
            result = 0;
            for (i=0;i < INTERNAL_BUFFER_SIZE; i+=4)
            {
               temp = calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i]>>8)] + calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i])]
                  +calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i+1]>>8)] + calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i+1])]
                 +calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i+2]>>8)] + calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i+2])]
                +calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i+3]>>8)] + calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i+3])];
                if (temp > 32)
                {
                    temp = temp-32;
                    result = result+temp*temp;
                } else if (temp < 32)
                {
                    temp = 32 - temp;
                    result = result + temp*temp;
                } 
            }
            result = result >> 4;
#if 0
        if (result > floor_tmp) 
        {
            GPIO_SetBits(GPIOD, LED_RED);
        }
#endif

        }
        else
        {
            for (i=0;i < INTERNAL_BUFFER_SIZE; i+=2)
            {
                res_data[i>>1] = calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i]>>8)] + calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i])];
                res_data[i>>1] = res_data[i>>1]+calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i+1]>>8)] + calc_ones_arr[(uint8_t)(mic_DMA_PDM_Buffer0[i+1])];
            }
        }
    }
}
#endif

