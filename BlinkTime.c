/**
 * BlinkTime
 *
 * A stopwatch utility to determine  
 * the time length of an operation
 * in kilo_ticks.
 *
 * Note:  Affected by endianness--check
 * processor and adjust bit-shifts/reads
 * accordingly.
 *
 * Outputs LSB binary number where
 * YELLOW is "START/STOP",
 * RED is 0,
 * WHITE is 1.
 *(Read left-to-right in time)
 *
 * @author Tom West
 * @date 7/18/2022
 * @version 1.0
 *
 */

#include "kilolib.h"

#define MAX_COUNT 10000

enum {
	ONE,
	ZERO,
	START,
	STOP,
} TimeSyms;

//operations
int counter = 0;

//stopwatch
uint32_t t_initial = 0;
uint32_t t_final = 0;
uint32_t t_delta = 0;

//showTime:
int i;
int k;
int timeWordSize = 8; //assume 8-bit word
uint8_t binary_time = 0;

//Inspired by the bitwise decimal-to-binary
//converter from Geeks for Geeks found here:
//      https://www.geeksforgeeks.org/program-decimal-binary-conversion/

void showTime(uint32_t time_in) {

	set_color(RGB(0,3,0)); //Blink GREEN at beginning
	delay(250);
	set_color(RGB(0,0,0));
	delay(250);

	for (j = timeWordSize; j >= 0; j--) {
		k = time_in >> j;
		if (k & 1) {
			set_color(RGB(3,3,3)); //flash WHITE for 1
			delay(250);
			set_color(RBB(0,0,0));
			delay(250);	
		} else {
			set_color(RGB(1,1,0)); //flash YELLOW for 0
			delay(250);
			set_color(RGB(0,0,0));
			delay(250);
		}//end if-else
	}//end for	

	set_color(RGB(3,0,0)); //blink RED at end
	delay(250);
	set_color(RGB(0,0,0));
	delay(250);

}//end showTime()

void setup(){
	//initialize kilo_ticks at t_0
	//(don't forget to zero-out assignment time :-)
	t_initial = kilo_ticks;
}//end setup()

void loop(){

	while (counter < MAX_COUNT) {
		counter++; //do until max count reached
	}//end for

	//takes trivial time to compute... but, must be considered
	//in more-precise applications!
	t_final = kilo_ticks; 
	t_delta = t_final - t_initial;

	while(1) {
		showTime(t_delta); //output binary # representation
	}//end while

}//end loop()

int main() {
	kilo_init();
	kilo_start(setup, loop);
}//end main()

//end program{}

