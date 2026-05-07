/* bsw_mcu.c — Virtual MCU stubs */
#include <time.h>
#include <stdio.h>
#include "bsw_mcu.h"

void Mcu_Init(void) {
    /* Rien à faire sur x86 — stub AUTOSAR */
}

unsigned int Mcu_GetResetReason(void) {
    return 0x01;  /* POWER_ON_RESET */
}

unsigned long Mcu_GetTimestamp_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long)(ts.tv_sec * 1000UL + ts.tv_nsec / 1000000UL);
}

void Wdg_Trigger(void) {
    /* Watchdog stub — no-op en simulation */
}