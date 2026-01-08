#include <avr/io.h>

/* Make the function visible to C code */
.global read_adc_avg

read_adc_avg:
    ; --- Setup ---
    ; Select ADC1 (MUX0=1)

    ldi r20, 0b00100001 ; REFS=00 (AREF), ADLAR=1 (Left Adjustement), MUX=00001 (ADC1)
    out _SFR_IO_ADDR(ADMUX), r20

    ldi r24, 0      ; Clear Low Byte of Accumulator
    ldi r25, 0      ; Clear High Byte of Accumulator
    ldi r18, 4      ; Counter = 4 readings

loop_read:
    ; Start Conversion
    sbi _SFR_IO_ADDR(ADCSRA), ADSC

wait_adc:
    ; Wait for conversion to finish
    sbis _SFR_IO_ADDR(ADCSRA), ADIF ; Skip next if ADIF is set (interrupt flag)
    rjmp wait_adc

    ; Clear Flag
    sbi _SFR_IO_ADDR(ADCSRA), ADIF

    ; Read Value
    in r19, _SFR_IO_ADDR(ADCH) ; Read only High byte (8-bit precision)

    ; Add to Accumulator (R25:R24)
    add r24, r19
    adc r25, r1    ; Add carry (r1 is usually zero in AVR-GCC)

    dec r18
    brne loop_read  ; If counter != 0, loop again

    ; Average (Divide by 4)
    ; Right shift the 16-bit value twice
    lsr r25
    ror r24
    lsr r25
    ror r24

    ; Result is now in R24 so we return
    ret
