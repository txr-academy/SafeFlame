/*
 * status.h
 *
 *  Created on: Aug 6, 2026
 *      Author: Rithika
 */

#ifndef INC_STATUS_H_
#define INC_STATUS_H_
typedef enum {
    LEAK_NORMAL = 0,   // No leak
    LEAK_SLOW   = 1,   // Slow leak
    LEAK_SUDDEN = 2    // Sudden leak
} LeakType_t;

typedef struct {
    LeakType_t leakType;     // Leak classification
    float remainingGas;      // kg
    int anomalyFlag;         // 0=Normal, 1=LPG/Methane, 2=CO
    float compensatedGas;    // after environmental compensation
    float humidity;
    float temperature;
    float weight;
    float mq2_ppm;
    float mq4_ppm;
    float mq7_ppm;
} StatusData_t;


#endif /* INC_STATUS_H_ */
