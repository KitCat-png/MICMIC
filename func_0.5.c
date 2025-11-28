#include <avr/interrupt.h>


const unsigned char digits[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xFF};
const unsigned char displays[] = {0b11000000, 0b10000000}; //0-9, blank
volatile unsigned char flag5ms = 0;
unsigned char tick_counter = 0;
unsigned char num_d1 = 0; //current number on display 1
unsigned char num_d0 = 0; //current number on display 0
unsigned char current_number = 0;
unsigned char switches = 0;

//flags for state of displays
unsigned char run_d1 = 0; 
unsigned char run_d0 = 0;

void init(void) {
	DDRA = 0b11000000;
	PORTA = 0b11000000;
	
	

	DDRC = 0xFF;
	PORTC = 0xFF;

	OCR0 = 77; //5ms
	TCCR0 = 0b00001111; //modo CTC, prescaler = 1024
	TIMSK |= 0b00000010; //TC0

	sei(); //activates flag I of SREG

}

ISR(TIMER0_COMP_vect) {
	flag5ms = 1;
}


void update_display(void) {
	static unsigned char current_display = 0;
	if (current_display == 0) {
		PORTA = displays[0];

		if (run_d0 == 1 || num_d0 > 0) { // if running show number
			PORTC = digits[num_d0];
		}
		else {
			PORTC = digits[10]; //blank
		}

		current_display = 1;
	}
	else {
		PORTA = displays[1];

		if (run_d1 == 1 || num_d1 > 0) { // if running show number
			PORTC = digits[num_d1];
		}
		else {
			PORTC = digits[10]; //blank
		}

		current_display = 0;
	}
}

int main(void) {

	init();

	while (1) {
		switches = PINA & 0b0001011;
		
		if (!(switches & (1 << 0))) { //SW1 pressed
			run_d1 = 1; //start display 1
		}	

		if (!(switches & (1 << 1))) { //SW2 pressed
			run_d0 = 1; //start display 0
		}	

		if (!(switches & (1 << 3))) { //SW4 pressed
			run_d1 = 0; //stop display 1
			run_d0 = 0; //stop display 0
		}

		if(flag5ms == 1){
			flag5ms = 0;

			update_display();

			tick_counter++;
		
			if(tick_counter >= 50){ //250ms 
				tick_counter = 0;

				//only increment if the display is running flag=1
				if (run_d1 == 1) {
					num_d1++;
					if (num_d1 > 9) {
						num_d1 = 0;
					}
				}

				//only increment if the display is running flag=1
				if (run_d0 == 1){
					num_d0++;
					if (num_d0 > 9) {
					num_d0 = 0;
					}
				}
			}
		}
	}
	return 0;
}