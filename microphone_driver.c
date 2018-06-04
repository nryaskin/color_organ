#include "microphone_driver.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"

static void WaveRecord_DMA_Init(uint32_t main_buffer, uint32_t double_buffer,uint32_t internal_buffer_size);

void MP4SDT02_I2S_init(uint32_t main_buffer, uint32_t double_buffer,uint32_t internal_buffer_size)
{
    GPIO_InitTypeDef GPIO_init_struct;
    I2S_InitTypeDef i2s_init_struct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    RCC_PLLI2SCmd(ENABLE);

    GPIO_init_struct.GPIO_Pin = GPIO_Pin_3;
    GPIO_init_struct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_init_struct.GPIO_OType = GPIO_OType_PP;
    GPIO_init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_init_struct);
 
    GPIO_init_struct.GPIO_Pin = GPIO_Pin_10;
    GPIO_init_struct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_init_struct.GPIO_OType = GPIO_OType_PP;
    GPIO_init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_init_struct);
   
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);

    SPI_I2S_DeInit(SPI2);

    i2s_init_struct.I2S_AudioFreq = I2S_AudioFreq_32k;
    i2s_init_struct.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    i2s_init_struct.I2S_DataFormat = I2S_DataFormat_16b;
    i2s_init_struct.I2S_Mode = I2S_Mode_MasterRx;
    i2s_init_struct.I2S_Standard = I2S_Standard_Phillips;
    i2s_init_struct.I2S_CPOL = I2S_CPOL_Low;

    I2S_Init(SPI2, &i2s_init_struct);

    WaveRecord_DMA_Init(main_buffer, double_buffer, internal_buffer_size);

    I2S_Cmd(SPI2, ENABLE);

}

static void WaveRecord_DMA_Init(uint32_t main_buffer, uint32_t double_buffer,uint32_t internal_buffer_size)
{
    DMA_InitTypeDef DMA_init_struct;
    NVIC_InitTypeDef NVIC_init_struct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Stream3, DISABLE);
    DMA_DeInit(DMA1_Stream3);

    DMA_init_struct.DMA_Channel = DMA_Channel_0;
    DMA_init_struct.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
    DMA_init_struct.DMA_Memory0BaseAddr = main_buffer;
    DMA_init_struct.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_init_struct.DMA_BufferSize = internal_buffer_size;
    DMA_init_struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_init_struct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_init_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_init_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_init_struct.DMA_Mode = DMA_Mode_Circular;
    DMA_init_struct.DMA_Priority = DMA_Priority_High;
    DMA_init_struct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_init_struct.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_init_struct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_init_struct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream3, &DMA_init_struct);

    DMA_DoubleBufferModeConfig(DMA1_Stream3, double_buffer, DMA_Memory_0);

    DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
    DMA_DoubleBufferModeCmd(DMA1_Stream3, ENABLE);

    NVIC_init_struct.NVIC_IRQChannel = DMA1_Stream3_IRQn;
    NVIC_init_struct.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_init_struct.NVIC_IRQChannelSubPriority = 10;
    NVIC_init_struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_init_struct);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

    DMA_Cmd(DMA1_Stream3, ENABLE);
}

