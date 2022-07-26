/*
 * ToMorse
 *
 * A Morse-Code output utility that allows
 * for a runtime error message system.
 * 
 * Choose a simple set of error words, e.g.
 * "MOTION" for an error in a motion method,
 * and place the letters in an array that calls
 * toMorse() from a while loop.
 *
 * I have implemented a simple Hello World to get
 * you started.  Cheers!
 * 
 * @author Tom West
 * @date 7/18/2022
 * @version 1.0
 *
 *
 */

#include "kilolib.h"

enum {
	A_,
	B_,
	C_,
	D_,
	E_,
	F_,
	G_,
	H_,
	I_,
	J_,
	K_,
	L_,
	M_,
	N_,
	O_,
	P_,
	Q_,
	R_,
	S_,
	T_,
	U_,
	V_,
	W_,
	X_,
	Y_,
	Z_,
	ZERO_,
	ONE_,
	TWO_,

} MorseSymbols;

enum {
	DIT = Z_ + 1,
	DAH,
	SPACE,
	WORD,
} MorsePulseType;

uint8_t hello_size = 12;
uint8_t hello_world[hello_size] = {H_, E_, L_, L_, O_, WORD, W_, O_, R_, L_, D_, WORD};

void ditDah(int pulseType) {

	switch(pulseType) {
		case DIT:
			set_color(RGB(1,1,1));
			set_motors(40, 40);
			delay(150);
			set_color(RGB(0,0,0));
			set_motors(0,0);
			delay(150);
			break;
		case DAH:
			set_color(RGB(1,1,1));
			set_motors(40,40);
			delay(450);
			set_color(RGB(0,0,0));
			set_motors(0,0);
			delay(150);
			break;
		case SPACE:
			set_color(RGB(0,0,0));
			set_motors(0,0);
			delay(450);
		case WORD:
			set_color(RGB(0,0,0));
			set_motors(0,0);
			delay(1050);
			break;
		default:
			set_color(RGB(1,0,1)); //MAGENTA for error
			set_motors(0,0); //turn off motors, STOP
			delay(3000); //wait 3 seconds
	}//end switch
}//end ditDah()

void toMorse(uint8_t curr_sym, int word_length){

	uint8_t sym_size;
	int i;

	switch(curr_symbol) {
		case WORD:
			break;
		case A_:; //.-
			sym_size = 2;
			uint8_t seqA[sym_size] = {DIT, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqA[i]);
			}//end for
			free(seqA);
			break;
		case B_:; //-...
			sym_size = 4;
			uint8_t seqB[sym_size] = {DAH, DIT, DIT, DIT};
			for (i = 0; i < sym_size; i++){
				ditDah(seqB[i]);
			}//end for
			free(seqB);
			break;
		case C_:; //-.-.
			sym_size = 4;
			uint8_t seqC[sym_size] = {DAH, DIT, DAH, DIT};
			for (i = 0; i < sym_size; i++){
				ditDah(seqC[i]);
			}//end for
			free(seqC);
			break;
		case D_:; //-.. 
			sym_size = 3;
			uint8_t seqD[sym_size] = {DAH, DIT, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqD[i]);
			}//end for
			free(seqD);
			break;
		case E_:; // .
			ditDah(DIT);
			break;
		case F_:; //..-.
			sym_size = 4;
			uint8_t seqF[sym_size] = {DIT, DIT, DAH, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqF[i]);
			}//end for	
			free(seqF);
			break;
		case G_:; //--.
			sym_size = 3;
			uint8_t seqG[sym_size] = {DAH, DAH, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqG[i]);
			}//end for
			free(seqG);
			break;
		case H_:; //....
			sym_size = 4;
			uint8_t seqH[sym_size] = {DIT, DIT, DIT, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqH[i]);
			}//end for
			free(seqH);
			break;
		case I_:; //..
			sym_size = 2;
			uint8_t seqI[sym_size] = {DIT, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqI[i]);
			}//end for
			free(seqI);
			break;
		case J_:; //.---
			sym_size = 4;
			uint8_t seqJ[sym_size] = {DIT, DAH, DAH, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqJ[i]);
			}//end for
			free(seqJ);
			break;
		case K_:; // -.-
			sym_size = 3;
			uint8_t seqK[sym_size] = {DAH, DIT, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqK[i]);
			}//end for
			free(seqK);
			break;
		case L_:; //.-..
			sym_size = 4;
			uint8_t seqL[sym_size] = {DIT, DAH, DIT, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqL[i]);
			}//end for
			free(seqL);
			break;
		case M_:; // --
			sym_size = 2;
			uint8_t seqM[sym_size] = {DAH, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqM[i]);
			}//end for
			free(seqM);
			break;
		case N_:; // -.
			sym_size = 2;
			uint8_t seqN[sym_size] = {DAH, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqN[i]);
			}//end for
			free(seqN);
			break;
		case O_:; // ---
			sym_size = 3;
			uint8_t seqO[sym_size] = {DAH, DAH, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqO[i]);
			}//end for
			free(seqO);
			break;
		case P_:; // .--.
			sym_size = 4;
			uint8_t seqP[sym_size] = {DIT, DAH, DAH, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqP[i]);
			}//end for
			free(seqP);
			break;
		case Q_:; //--.-
			sym_size = 4;
			uint8_t seqQ[sym_size] = {DAH, DAH, DIT, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqQ[i]);
			}//end for
			free(seqQ);
			break;
		case R_:; //.-.
			sym_size = 3;
			uint8_t seqR[sym_size] = {DIT, DAH, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqR[i]);
			}//end for
			free(seqR);
			break;
		case S_:; // ...
			sym_size = 3;
			uint8_t seqS[sym_size] = {DIT, DIT, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqS[i]);
			}//end for
			free(seqS);
			break;
		case T_:; // -
			ditDah(DAH);
			break;
		case U_:; //..-
			sym_size = 3;
			uint8_t seqU[sym_size] = {DIT, DIT, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqU[i]);
			}//end for
			free(seqU);
			break;
		case V_:; // ...- (Beethoven's Vth)
			sym_size = 4;
			uint8_t seqV[sym_size] = {DIT, DIT, DIT, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqV[i]);
			}//end for
			free(seqV);
			break;
		case W_:; //.--
			sym_size = 3;
			uint8_t seqW[sym_size] = {DIT, DAH, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqW[i]);
			}//end for
			free(seqW);
			break;
		case X_:; //-..-
			sym_size = 4;
			uint8_t seqX[sym_size] = {DAH, DIT, DIT, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqX[i]);
			}//end for
			free(seqX);
			break;
		case Y_:; // -.--
			sym_size = 4;
			uint8_t seqY[sym_size] = {DAH, DIT, DAH, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqY[i]);
			}//end for
			free(seqY);
			break;
		case Z_:; // --..
			sym_size = 4;
			uint8_t seqZ[sym_size] = {DAH, DAH, DIT, DIT};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqZ[i]);
			}//end for
			free(seqZ);	
			break;
		case ZERO_:; // -----
		   	sym_size = 5;
			uint8_t seqZERO[sym_size] = {DAH, DAH, DAH, DAH, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqZERO[i]);
			}//end for
			free(seqZERO);		
			break;
		case ONE_:; // .----
			sym_size = 5;
			uint8_t seqONE[sym_size] = {DIT, DAH, DAH, DAH, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqONE[i]);
			}//end for
			free(seqONE);
			break;
		case TWO_:; //..---
			sym_size = 5;
			uint8_t seqTWO[sym_size] = {DIT, DIT, DAH, DAH, DAH};
			for (i = 0; i < sym_size; i++) {
				ditDah(seqTWO[i]);
			}//end for
			free(seqTWO);
			break;
		default :
		        set_color(RGB(1,0,1)); //MAGENTA for error	
			set_motors(0,0);
	}//end switch

	if (curr_sym != WORD) {
		ditDah(SPACE);
	} else {
		ditDah(WORD);
	}//end if-else
}//end toMorse()

void setup(){
	//insert setup sequence here...
	
	//comms:
	//outgoing_message.data[0] = FALSE;
	//outgoing_message.type = NORMAL;
	//outgoing_message.crc = message_crc(&outgoing_message);
} // end setup()

void loop() {
	//insert your runtime code here...
	while(1){
		for (i = 0; i < hello_size; i++) {
			toMorse(hello_world[i]); //"Hello, World!"
		}//end for
	}//end while
} // end loop()

int main() {

	//standard startup sequence:
	kilo_init();
	//kilo_message_rx = message_rx;
	//kilo_message_tx = message_tx;
	//kilo_message_tx_success = message_tx_success;
	kilo_start(setup, loop);

	return 0;
}

// end program{}

