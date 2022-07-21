/**
 *
 * Neighbor Table:
 * Keep track of what's going on over the fence.
 *
 * @author Tom West
 * @data 7/18/2022
 * @version 1.0
 *
 */


#include "kilolib.h"


//////////////
//Enums://
//////////////

enum {
	FALSE, // 0
	TRUE,  // 1
} Booleans;

enum {
	HALT,
	ID,
	TEAM,
	MOTION,
	SPEED,
} MessageIndices;

enum {
	//RED_TEAM,
	YELLOW_TEAM,
	//GREEN_TEAM,
	//CYAN_TEAM,
	BLUE,
	//MAGENTA_TEAM,
	//WHITE_TEAM, --slightly awkward...
} Teams;

enum {
	STOP,
	FORWARD,
	LEFT,
	RIGHT,
} MotionStates;

enum {
	FAST,
	DEFAULT_SPEED,
	SLOW,
} MotionRates;

enum {
	COLORLESS,
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
} Colors;


//////////////////////
//Defines/Variables://
//////////////////////

//Memory:
#define ONE_BYTE 8
#define BYTE_MAX 255

//Interrupts:
#define TOO_CLOSE 20
#define TOO_FAR 90
uint8_t halt_mode = FALSE; //halt raised?

//Timing:
#define MEM_WIPE_WAIT 160
uint8_t timestamp; //store timestamps in neighbor table to check if stale
uint32_t current_time;
uint32_t memory_timer;
uint32_t random_motion_timer;
uint32_t halt_timer;
uint32_t mail_timer;

//Motion:
uint8_t new_motion;
uint8_t previous_motion;
uint8_t new_speed;
uint8_t previous_speed;
uint8_t proximity_alert = FALSE;
int addFwd = 0;
int addLeft = 0;
int addRight = 0;

//Mail:
#define MAILBOX_SIZE 50 //determined by average velocity and rate of msg txmission
#define MAIL_PARAMS 7 //number of variables tracked in mail
uint8_t mailbox[MAILBOX_SIZE][MAIL_PARAMS] = 0;
uint8_t mailbox_slot = 0;
uint8_t mailboxRow = 0;
uint8_t mailboxCol = 0;
uint8_t full_mailbox = FALSE;

uint8_t msgID = 0;
uint8_t priorityID = 0;

//Teaming:
#define NUMBER_OF_TEAMS 2
#define MAX_NEIGHBORS 36 // assuming 10 cm comms radius, this is max density
#define NEIGHBOR_PARAMS 7 // determines number of variables to store in table
#define DIST 3
uint8_t neighbor_table[MAX_NEIGHBORS][NEIGHBOR_PARAMS] = 0; // nxm table of neighbors
uint8_t new_neighbor = FALSE;
uint8_t myTeam;
uint8_t closestDist = BYTE_MAX;
uint8_t closestID;
uint8_t closestAlly;
uint8_t closestAlly_dist = BYTE_MAX;
uint8_t closestAdversary;
uint8_t closestAdversary_dist = BYTE_MAX;
uint8_t myGradient;
uint8_t is_seed = FALSE;
uint8_t is_far = FALSE;

uint8_t theirID = 0;
uint8_t theirTeam = 0;
uint8_t theirMotion = 0;
uint8_t theirSpeed = 0;

//Messaging:
#define GRADIENT_MAX 255
message_t message_in;
message_t message_out;
message_t priority_mail;
uint8_t new_message = FALSE; //mailbox flag


////////////
//Methods://
////////////

//Motion:
void set_motion(int next_motion, int next_speed) {

	if (next_motion != previous_motion) { 
		//check speed
		switch (next_speed) {
			case FAST:
				addFwd = 10;
				addLeft = 7;
				addRight = 7;
				break;
			case SLOW:
				addFwd = -5;
				addLeft = -4;
				addRight = -4;
				break;
			case DEFAULT:
				addFwd = 0;
				addLeft = 0;
				addRight = 0;
				break;
			default:
				//ERROR
				set_color(RGB(1,0,1));
				set_motors(0,0);
		}//end switch
		
		switch (next_motion) {
			case STOP:
				set_motors(0,0);
				break;
			case FORWARD:
				spinup_motors();
				set_motors(kilo_straight_left + addFwd,\
						kilo_straight_right + addFwd);
				break;
			case LEFT:
				set_motors(BYTE_MAX, 0);
				delay(15);
				set_motors(kilo_turn_left + addLeft, 0);
				break;
			case RIGHT:
				set_motors(0, BYTE_MAX);
				delay(15);
				set_motors(0, kilo_turn_right + addRight);
				break;
			default:
				//ERROR
				set_motors(0,0);
				set_color(RGB(1,0,1));
		}//end switch
	}//end if (else do nothing)
}//end set_motion()

//Messaging:
void message_rx(message_t* msg, distance_estimate_t dist) {

	new_message = TRUE; //We just got a letter...

	if (msg.data[HALT] == TRUE) {
		halt_mode = TRUE;
		priorityID++;
		priority_mail = *msg; //dereference, store whole message for external processing
		return; //hop back to loop()
	}//end if

	//Tidy indices:
	if (msgID > BYTE_MAX) {
		msgID = 0;
	} //end if

	if (mailbox_slot == (MAILBOX_SIZE - 1)){
		full_mailbox = TRUE;
	}//end if


	mailbox[mailbox_slot][0] = msgID;
	mailbox[mailbox_slot][ID] = msg.data[ID];
	theirTeam = msg.data[TEAM];
	if (theirTeam == myTeam) {
		mailbox[mailbox_slot][TEAM] = TRUE; // ALLY
	} else {
		mailbox[mailbox_slot][TEAM] = FALSE; // ADVERSARY
	} //end if-else

	mailbox[mailbox_slot][DIST] = estimate_distance(dist);
	mailbox[mailbox_slot][MOTION + 1] = msg.data[MOTION];
	mailbox[mailbox_slot][SPEED + 1] = msg.data[SPEED];
	mailbox[mailbox_slot][6] = kilo_ticks - mail_timer;

	msgID++; //increment counter

	if (mailbox_slot < (MAILBOX_SIZE - 1)) {
		mailbox_slot++; //keep incrementing until full
	} else {
		full_mailbox = TRUE; //time to fetch the mail
		mailbox_slot = 0; //reset mailbox index
	}
 
}//end message_rx()

message_t* message_tx() {
	return &message_out;
}//end message_tx()


//Proximity:
void show_gradient(uint8_t *thisGradient) {

	int grad = *thisGradient;
	if (grad < 6) {
		switch(grad) { //check number of hops
			case 0:
				set_color(RGB(1,0,0)); //RED
				break;
			case 1:
				set_color(RGB(1,1,0)); //YELLOW
				break;
			case 2:
				set_color(RGB(0,1,0)); //GREEN
				break;
			case 3:
				set_color(RGB(0,1,1)); //CYAN
				break;
			case 4: 
				set_color(RGB(0,0,1)); //BLUE
				break;
			case 5:
				set_color(RGB(1,0,1)); //MAGENTA
				break;
			default :
				//ERROR
				set_color(RGB(0,0,0)); //WHITE
				spinup_motors(); //Chirp
		}//end switch
	} else {
		set_color(RGB(1,1,1)); //WHITE = far away
		is_far = TRUE;
	}//end if-else
}//end showGradient()

//Teaming:
void showTeamSpirit(int thisTeam){
	switch (thisTeam) {
		//case RED_TEAM:
		//	set_color(RGB(1,0,0));
		//	break;
		case YELLOW_TEAM:
			set_color(RGB(1,1,0));
			break;
		//case GREEN_TEAM:
		//	set_color(RGB(0,1,0));
		//	break;
		//case CYAN_TEAM:
		//	set_color(RGB(0,1,1));
		//	break;
		case BLUE_TEAM:
			set_color(RGB(0,0,1));
			break;
		//case MAGENTA_TEAM:
		//	set_color(RGB(1,0,1));
		//	break;
		//case WHITE_TEAM:
		//	set_color(RGB(1,1,1));
		//	break;
		default :
			set_color(RGB(0,0,0)); //COLORLESS...
			spinup_motors();//chirp
	}//end switch
}

//////////////////////////////
////Setup()/Loop()/Main():////
//////////////////////////////

void setup() {

	current_time = kilo_ticks;
	
	myID = rand_hard() % BYTE_MAX;

	
//	for (int k = 0; k < ONE_BYTE; k++) {
//		myID |= kilo_uid << k;
//	}//end for

	myTeam = myID % NUMBER_OF_TEAMS;

	//Assign normal-mode values:
	message_out.data[HALT] = FALSE; // set to FALSE
	message_out.data[ID] = myID; // currently limited to uint8_t
	message_out.data[TEAM] = myTeam;  // mod #teams
	message_out.data[MOTION] = new_motion; // start in STOP mode
	message_out.data[SPEED] = new_speed; // start at PACE
	message_out.type = NORMAL;
	message_out.crc = message_crc(&message_out);

	set_motion(new_motion, new_speed);
} //end SETUP()

void loop() {
	if (kilo_ticks > (memory_timer + MEM_WAIT)) {
		//reset clock
		memory_timer = kilo_ticks;

		//erase mailbox, neighbors
		uint8_t tmpTimestamp = 0;

		for (int i = 0; i < MAX_NEIGHBORS; i++) {
			tmpTimestamp = neighbor_table[i][0];
			if (((memory_timer - tmpTimestamp) >= MEM_WAIT)\
					&& (tmpTimestamp != 0)) { //0 for uninitialized
				for (int j = 0; j < NEIGHBOR_PARAMS; j++) {
					neighbor_table[i][j] = 0;
				}//end for	
			}//end if
		}//end for
	}

	if (!halt_mode) {
		
		//sort the mail (either it's been too long, or mailbox is full):
		if ((kilo_ticks > (mail_timer + 32)) || (full_mailbox == TRUE)) {

			mail_timer = kilo_ticks; //reset clock

			if (full_mailbox == TRUE) {
				full_mailbox == FALSE;
			}//end if
			
			//Checks: 1. Proximity?  2. Bearing? 3. New? 4. Collision?
			uint8_t tmpID = BYTE_MAX;
			uint8_t tmpAlly = FALSE;
			uint8_t tmpDist = BYTE_MAX;
			uint8_t tmpMotion = BYTE_MAX;
			uint8_t tmpSpeed = BYTE_MAX;


		
			//open the mail:
			for (int i = 0; i > -1; i--) {

				//don't check empty slot
				if (mailbox[i][DIST] > 0) {
					tmpID = mailbox[i][ID];
					tmpAlly = mailbox[i][TEAM]; //FALSE if adversary
					tmpDist = mailbox[i][DIST];
					tmpMotion = mailbox[i][MOTION + 1];
					tmpSpeed = mailbox[i][SPEED + 1];
	
					//empty mailbox slot:
					for (int j = 0; j < MAIL_PARAMS; j++) {
						mailbox[i][j] = 0; //reset to default
					}//end for
	
					//update neighbor table
					if (tmpID == neighbor_table[i][ID]) {
						if ((kilo_ticks - memory_timer)\
								< (BYTE_MAX + 1)) {//avoid overflow
							neighbor_table[i][0] =\
								(uint8_t) (kilo_ticks - memory_timer);\
								//cast to avoid
						} else {
							neighbor_table[i][0] = BYTE_MAX; //time maxed-out
						}//end if-else
						neighbor_table[i][DIST] = tmpDist;
						neighbor_table[i][MOTION + 1] = tmpMotion;
						neighbor_table[i][SPEED + 1] = tmpSpeed;
					} else if ((tmpID != neighbor_table[i][ID]) && (i == 0)) {\
						//sweeps whole table
						new_neighbor = TRUE;
						if ((kilo_ticks - memory_timer) < (BYTE_MAX + 1)) {
							neighbor_table[i][0] =\
								(uint8_t) (kilo_ticks - memory_timer);
						} else {
							neighbor_table[i][0] = BYTE_MAX;
						}//end if-else
						neighbor_table[i][DIST] = tmpDist;
						neighbor_table[i][MOTION + 1] = tmpMotion;
						neighbor_table[i][SPEED + 1] = tmpSpeed;
					}//end if-else-if
		
					if (tmpAlly) {
						if (tmpDist < closestAlly_dist) {
							closestAlly_dist = tmpDist;
							//in future add velocity checking...
						}//end if
					} else {
						if (tmpDist < closestAdversary_dist) {
							closestAdversary_dist = tmpDist;
						}//end if
					}//end if-else

					if ((closestAlly_dist < closest) ||\
							(closestAdversary_dist < closest)) {
						//possible threat!
						closestID = tmpID; //keep watch

						//friend or foe?
						if (closestAlly_dist < closestAdversary_dist) {
							closest = closestAlly_dist;
						} else {
							proximity_alert = TRUE; //set the alarm
							closest = closestAdversary_dist;
						}//end if-else
					}//end if
				}//end for


		//collision detection
		//divert maneuvers
		if (proximity_alert == TRUE) {
			proximity_alert = FALSE; //reset alarm

			if (closest < TOO_CLOSE) {
				closestAdversary_dist = BYTE_MAX; //reset distance trigger
				closestAlly_dist = BYTE_MAX; //reset per above
				halt_mode = TRUE; //sound the alarm!
				is_seed = TRUE;
				return; //go into halt mode, priority mail checking
			}

			//prioritize threats
			//with different distance thresholds
			//identify friend/foe and flock or disperse

		////////////////
		//NORMAL MODE://
		////////////////

		} else if (kilo_ticks > (random_motion_timer + 128)) {
			//Nobody's watching!
			//Do your little Kilobot thing!!

			//update
			random_motion_timer = kilo_ticks;

			//time-discrete random walk
			previous_motion = new_motion;
			new_motion = rand_hard() % 4;
			previous_speed = new_speed;
			new_speed = rand_hard() % 3;
			
			set_motion(new_motion, new_speed); //reset motion

		}//end if-else-if
	} else {
		//go to SPECIAL MODE

		//STOP
		set_motors(0,0);
		set_color(RGB(1,1,1));

		message_out.data[HALT] = TRUE;
		
		if (is_seed == TRUE) {
			message_out.data[5] = myID;
			message_out.data[6] = 0;
			message_out.crc = message_crc(&message_out);
		} else {
			uint8_t seed_id = priority_mail.data[ID];
			uint8_t seed_team = priority_mail.data[TEAM];
			uint8_t seed_motion = priority_mail.data[MOTION];
			uint8_t seed_speed = priority_mail.data[SPEED];

			myGradient = GRADIENT_MAX;
			message_out.data[6] = myGradient;
			message_out.crc = message_crc(&message_out);
		}

		show_gradient(&myGradient);
		while (halt_mode) {
			
			//regulate message flow
			if (kilo_ticks > (halt_timer + 8)) {
				halt_timer = kilo_ticks;
				show_gradient(&myGradient);
				if (new_message == TRUE) {
					new_message = FALSE;
					uint8_t thisGradient =\
						priority_mail.data[6];
					//activate gradient check
					if ((is_seed == FALSE) && (myGradient > (thisGradient + 1))) {
						myGradient = thisGradient + 1;
						message_out.data[6] = myGradient;
						message_out.crc = message_crc(&message_out);
					}
				}
			}

		}	
		
	}

}//end LOOP()

int main() {
	kilo_init();
	//kilo_message_rx = message_rx;
	//kilo_message_tx = message_tx;
	//kilo_message_tx_success = message_tx_success;
	kilo_start(setup, loop);
}//end MAIN()

//end program{}

