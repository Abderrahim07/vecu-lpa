#ifndef BSW_MCU_H
#define BSW_MCU_H

void Mcu_Init(void);
unsigned int Mcu_GetResetReason(void);
unsigned long Mcu_GetTimestamp_ms(void);
void Wdg_Trigger(void);

#endif