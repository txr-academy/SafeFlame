/*
 * hx711.c
 *
 *  Created on: May 25, 2026
 *      Author: Rithika
 */

/*  This HX711 driver works by manually implementing the chip’s custom synchronous protocol through GPIO toggling.
 *  At initialization, PB1 (SCK) is set low to establish a stable starting state.
 *   During a read, the STM32 first waits until HX711 signals data ready by pulling PB0 (DOUT) low, then generates 24 clock pulses on PB1;
 *   each rising edge causes HX711 to shift out one bit of its 24 bit ADC register, which the code assembles into value.
 *   After the 24th bit, the code issues one extra pulse (25th), which is not part of the data but re selects the next conversion mode —
 *   in this case Channel A with Gain 128, the default configuration. Because HX711 outputs a 24 bit two’s complement signed value,
 *   the MCU must sign extend it into 32 bits so negative readings are preserved correctly. The tare routine averages multiple samples to
 *   establish a baseline offset, while the weight function subtracts this offset from new raw readings and multiplies by a calibration
 *   factor to convert counts into kilograms. In essence, the code handles clock generation, protocol framing, signed data conversion, and
 *   calibration, turning the milli-volt signal from the load cell into a stable, meaningful weight measurement.*/



#include "hx711.h"

void HX711_Init(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // PB1 = SCK
}

long HX711_Read(void) {
    long value = 0;
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)); // wait for DT low


    for (int i = 0; i < 24; i++) {               //Generate clock pulse that toggles exactly 24 times, and reads data when clock pulse is high.
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

        value = (value << 1) | HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);// left shift the value by 1 and perform bit wise OR operation

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);//generate the 25th clock pulse to tell the processor to continue with channel A gain 128
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    // Proper sign extension from 24-bit to 32-bit
        if (value & 0x800000) {        // if MSB (bit 23) is 1
            value |= 0xFF000000;       // set upper 8 bits to 1s
         }
        return value;
}

long HX711_Tare(int samples) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        //sum += HX711_Read();
    	/*New line added for debugging
    	    long dummy = 1000;
    	    return dummy++;
    	    */
    }
    return sum / samples;
}
float HX711_GetWeight(long offset, float factor) {
    long raw = HX711_Read();   // The value returned by the function HX711_Read() is assigned to raw.
	//long raw=1000;
	long net = raw - offset;           //Actual weight calculation
    return net * factor;


}




