/**
 * BinID* :
 * Unique, Randomly-Generated Names for N Agents
 *
 * Uses rand() mod(2) to generate N unique id's in an N-Agent system.
 *
 * Requirements:
 * - Locomotion
 * - Inter-agent communication
 * - Number of agents, N, must be known
 *
 * O function:
 * - up to N rounds, N/2 in practice
 * 
 * Memory:
 * - up to N bits, N/2 in practice
 *
 * @author Tom West
 * @date 7/18/2022
 * @version 1.0
 */

#include "kilolib.h"

// word_t name;
uint8_t name = 0; // all UIDs initialized to 0
one_byte = 8;
const int MAX_BITS = sizeof(name) * one_byte;
#define NUM_BRANCHES 2

int i; // indexing
int num_neighbors;
int curr_bit; // stores coin flip outcome
int entangled_outcome; // stores duel outcome
int same_outcome = 0; // flag for duel
int converged = 0; 

int coinFlip(){
	return rand_hard() % NUM_BRANCHES;
}//end coinFlip()

void findOtherOnes(){
	checkNeighbors();
}//end findOtherOnes()

void stayPut(){
}//end stayPut()

void countNeighbors(){
}//end countNeighbors()



while (!converged) {
	for (i = 0; i < MAX_BITS; i++) {
		if (num_neighbors > 2) {  
			curr_bit = coinFlip(); //0 or 1 assigned
			name |= curr_bit << i;
			if (curr_bit) { //1's move to new location
				findOtherOnes();
			} else { //0's stay put
				stayPut();
				countNeighbors();
			}//end if-else

			//recount neighbors for next round:
			num_neighbors = countNeighbors(); 
		} else { //when coalition size reaches two, IDs *must* split
			while (same_outcome) {
				entangled_outcome = rand() % 2;
				same_outcome = checkFlip(flip_outcome);	
			}
			converged = TRUE;
			break; //get out of for loop
		}
	}	
}



void setup() {
}//end setup()

void loop() {
}//end loop()

int main() {

	kilo_init();
	//kilo_message_rx = message_rx;
	//kilo_message_tx = message_tx;
	//kilo_message_tx success = message_tx_success;
	kilo_start(setup, loop);

	return 0;

}//end MAIN()

//end program{}
