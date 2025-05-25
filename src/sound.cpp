#include <stdint.h>

#include "sound.h"

/* Tones (Hz) */
#define c    261
#define d    294
#define e    329
#define f    349
#define g    391
#define gS   415
#define a    440
#define aS   455
#define b    466
#define cH   523
#define cSH  554
#define dH   587
#define dSH  622
#define eH   659
#define fH   698
#define fSH  740
#define gH   784
#define gSH  830
#define aH   880

uint16_t alert_notes[] = {
    aH, gH, aH, gH, aH, gH, aH, gH,
    aH, aH, gH, gH, aH, gH, aH, cH
};

/* Short durations to create a sense of urgency */
uint16_t alert_durations[] = {
    150, 150, 150, 150, 150, 150, 150, 150,
    100, 100, 100, 100, 100, 100, 100, 300
};

/* Total number of notes in alert melody */
uint8_t num_notes = sizeof(alert_durations) / sizeof(alert_durations[0]);