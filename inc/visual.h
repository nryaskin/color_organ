#ifndef _VISUAL_H
#define _VISUAL_H

#define LED_RED GPIO_Pin_14
#define LED_GREEN GPIO_Pin_12
#define LED_BLUE GPIO_Pin_15
#define L_INDICATORS (LED_RED) | (LED_GREEN) | (LED_BLUE) 

void init_rgb_leds(void);

#endif
