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

//#include "/mnt/c/Users/twest/OneDrive/Desktop/OtteLab_SU22/Misc/otteumd-code-d56d8da35990/Kilolib/kilolib.h"

#define ONE_BYTE 8
#define BYTE_MAX 255
#define TOO_CLOSE 40
#define TOO_FAR 90
#define MEM_WIPE_WAIT 320
#define MAILBOX_SIZE 5 // determined by average velocity and rate of msg txmission
#define MAIL_PARAMS 7	// number of variables tracked in mail
#define NUMBER_OF_TEAMS 2
#define MAX_NEIGHBORS 36  // assuming 10 cm comms radius, this is max density
#define NEIGHBOR_PARAMS 7 // determines number of variables to store in table
#define DIST 3
#define GRADIENT_MAX 255

//////////////
// Enums://
//////////////

enum
{
	FALSE, // 0
	TRUE,  // 1
} Booleans;

enum
{
	HALT,
	ID,
	TEAM,
	MOTION,
	SPEED,
} MessageIndices;

enum
{
	// RED_TEAM,
	YELLOW_TEAM,
	// GREEN_TEAM,
	// CYAN_TEAM,
	BLUE_TEAM,
	// MAGENTA_TEAM,
	// WHITE_TEAM, --slightly awkward...
} Teams;

enum
{
	STOP,
	FORWARD,
	LEFT,
	RIGHT,
} MotionStates;

enum
{
	FAST,
	DEFAULT_SPEED,
	SLOW,
} MotionRates;

enum
{
	COLORLESS,
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
} Colors;

struct GLOBALS
{
	// global variables
	// NOTE: the use of a GLOBALS struct will also work on a normal kilobot,
	//       but is not required on a normal kilobot.
	//       It is, however, required by the simulator.

	//////////////////////
	// Defines/Variables://
	//////////////////////

	// Memory:

	// Interrupts:

	uint8_t halt_mode = FALSE; // halt raised?

	// Timing:
	uint8_t timestamp; // store timestamps in neighbor table to check if stale
	uint32_t current_time;
	uint32_t memory_timer;
	uint32_t random_motion_timer;
	uint32_t halt_timer;
	uint32_t mail_timer;

	// Motion:
	uint8_t new_motion;
	uint8_t previous_motion;
	uint8_t new_speed;
	uint8_t previous_speed;
	uint8_t proximity_alert = FALSE;
	int addFwd = 0;
	int addLeft = 0;
	int addRight = 0;

	// Mail:
	uint8_t mailbox[MAILBOX_SIZE][MAIL_PARAMS] = {};
	uint8_t mailbox_slot = 0;
	uint8_t mailboxRow = 0;
	uint8_t mailboxCol = 0;
	uint8_t full_mailbox = FALSE;

	uint8_t msgID = 0;
	uint8_t priorityID = 0;

	// Teaming:
	uint8_t neighbor_table[MAX_NEIGHBORS][NEIGHBOR_PARAMS] = {}; // nxm table of neighbors
	uint8_t new_neighbor = FALSE;
	uint8_t newNeighborIndex;
	uint8_t myID;
	uint8_t myTeam;
	uint8_t closest;
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

	// Messaging:
	message_t message_in;
	message_t message_out;
	message_t priority_mail;
	uint8_t new_message = FALSE; // mailbox flag

} * g; // there should only be one GLOBAL, this is it, remember to register it in main()

////////////
// Methods://
////////////

// Motion:
void set_motion(int next_motion, int next_speed)
{

	if (next_motion != g->previous_motion)
	{
		// check speed
		switch (next_speed)
		{
		case FAST:
			g->addFwd = 10;
			g->addLeft = 7;
			g->addRight = 7;
			break;
		case SLOW:
			g->addFwd = -5;
			g->addLeft = -4;
			g->addRight = -4;
			break;
		case DEFAULT_SPEED:
			g->addFwd = 0;
			g->addLeft = 0;
			g->addRight = 0;
			break;
		default:
			// ERROR
			set_color(RGB(1, 0, 1));
			set_motors(0, 0);
		} // end switch

		switch (next_motion)
		{
		case STOP:
			set_motors(0, 0);
			break;
		case FORWARD:
			spinup_motors();
			set_motors(kilo_straight_left + g->addFwd,
					   kilo_straight_right + g->addFwd);
			break;
		case LEFT:
			set_motors(BYTE_MAX, 0);
			delay(15);
			set_motors(kilo_turn_left + g->addLeft, 0);
			break;
		case RIGHT:
			set_motors(0, BYTE_MAX);
			delay(15);
			set_motors(0, kilo_turn_right + g->addRight);
			break;
		default:
			// ERROR
			set_motors(0, 0);
			set_color(RGB(1, 0, 1));
		} // end switch

		//UPDATE Motion/Speed parameters in message:
		g->message_out.data[MOTION] = next_motion;
		g->message_out.data[SPEED] = next_speed;
		g->message_out.crc = message_crc(&(g->message_out));
	}	  // end if (else do nothing)

} // end set_motion()

// Messaging:
void message_rx(message_t *msg, distance_measurement_t* dist) {

	if (g->halt_mode == TRUE) {
		log_message("HELLO message_RX");
	}

	g->new_message = TRUE; // We just got a letter...

	if ((msg->data[HALT]) == TRUE) {
		log_message("HALT!");
		g->halt_mode = TRUE;
		g->priorityID++;
		g->priority_mail = *msg; // dereference, store whole message for external processing
		g->priority_mail.crc = message_crc(&(g->priority_mail));
		 // hop back to loop()

	} else {							

		// Tidy indices:
		if (g->msgID > BYTE_MAX)
		{
			g->msgID = 0;
		} // end if

		if (g->mailbox_slot == (MAILBOX_SIZE - 1))
		{
			g->full_mailbox = TRUE;
		} // end if

		g->mailbox[g->mailbox_slot][0] = g->msgID;
		g->mailbox[g->mailbox_slot][ID] = msg->data[ID];
		g->theirTeam = msg->data[TEAM];
		if (g->theirTeam == g->myTeam)
		{
			g->mailbox[g->mailbox_slot][TEAM] = TRUE; // ALLY
		}
		else
		{
			g->mailbox[g->mailbox_slot][TEAM] = FALSE; // ADVERSARY
		}											   // end if-else

		g->mailbox[g->mailbox_slot][DIST] = estimate_distance(dist);
		g->mailbox[g->mailbox_slot][MOTION + 1] = msg->data[MOTION];
		g->mailbox[g->mailbox_slot][SPEED + 1] = msg->data[SPEED];
		g->mailbox[g->mailbox_slot][6] = kilo_ticks - g->mail_timer;

		g->msgID++; // increment counter

		if (g->mailbox_slot < (MAILBOX_SIZE - 1))
		{
			g->mailbox_slot++; // keep incrementing until full
		}
		else
		{
			g->full_mailbox = TRUE; // time to fetch the mail
			g->mailbox_slot = 0;	// reset mailbox index
		}
	}
} // end message_rx()

message_t *message_tx()
{
	if (g->halt_mode == TRUE){
		log_message("HELLO msg_tx!");
	}
	return &(g->message_out);
} // end message_tx()

void message_tx_success() {
	if (g->halt_mode == TRUE) {
		log_message("HELLO msg_tx_success!");
	}
    // Do nothing
}

// Proximity:
void show_gradient(uint8_t *thisGradient)
{

	int grad = *thisGradient;
	if (grad < 6)
	{
		switch (grad)
		{ // check number of hops
		case 0:
			set_color(RGB(1, 0, 0)); // RED
			break;
		case 1:
			set_color(RGB(1, 1, 0)); // YELLOW
			break;
		case 2:
			set_color(RGB(0, 1, 0)); // GREEN
			break;
		case 3:
			set_color(RGB(0, 1, 1)); // CYAN
			break;
		case 4:
			set_color(RGB(0, 0, 1)); // BLUE
			break;
		case 5:
			set_color(RGB(1, 0, 1)); // MAGENTA
			break;
		default:
			// ERROR
			set_color(RGB(0, 0, 0)); // WHITE
			spinup_motors();		 // Chirp
		}							 // end switch
	}
	else
	{
		set_color(RGB(1, 1, 1)); // WHITE = far away
		g->is_far = TRUE;
	} // end if-else
} // end showGradient()

int getVelocity(int t1, int t2, int r1, int r2) {

	int velocity = 0; //radial velocity

	int dt = t2 - t1;
	int dr = r2 - r1;

	if (dt != 0) { //beware ye who divideth by zero!
		velocity = (int) dr/dt; //avoid integer division
	}

	return velocity; //truncated, but also avoids integer-division errors
}

// Teaming:
void showTeamSpirit(int thisTeam)
{
	switch (thisTeam) {
		// case RED_TEAM:
		//	set_color(RGB(1,0,0));
		//	break;
		case YELLOW_TEAM:
			set_color(RGB(1, 1, 0));
			break;
		// case g->GREEN_TEAM:
		//	set_color(RGB(0,1,0));
		//	break;
		// case g->CYAN_TEAM:
		//	set_color(RGB(0,1,1));
		//	break;
		case BLUE_TEAM:
			set_color(RGB(0, 0, 1));
			break;
		// case g->MAGENTA_TEAM:
		//	set_color(RGB(1,0,1));
		//	break;
		// case g->WHITE_TEAM:
		//	set_color(RGB(1,1,1));
		//	break;
		default:
			set_color(RGB(0, 0, 0)); // COLORLESS...
			spinup_motors();		 // chirp
	}							 // end switch
}

//////////////////////////////
////Setup()/Loop()/Main():////
//////////////////////////////

void setup() {

	g->priority_mail.data[0] = FALSE;
	g->priority_mail.type = NORMAL;
	g->priority_mail.crc = message_crc(&(g->priority_mail));

	g->closestAlly_dist = BYTE_MAX;
	g->closestAdversary_dist = BYTE_MAX;
	g->closest = BYTE_MAX;

	g->current_time = kilo_ticks;

	g->myID = rand_hard() % BYTE_MAX;

	//	for (int k = 0; k < g->ONE_BYTE; k++) {
	//		g->myID |= kilo_uid << k;
	//	}//end for

	g->myTeam = g->myID % NUMBER_OF_TEAMS;

	//output team color:
	showTeamSpirit(g->myTeam);


	// Assign normal-mode values:
	g->message_out.data[HALT] = FALSE;			 // set to FALSE
	g->message_out.data[ID] = g->myID;			 // currently limited to uint8_t
	g->message_out.data[TEAM] = g->myTeam;		 // mod #teams
	g->message_out.data[MOTION] = g->new_motion; // start in STOP mode
	g->message_out.data[SPEED] = g->new_speed;	 // start at PACE
	g->message_out.type = NORMAL;
	g->message_out.crc = message_crc(&(g->message_out));

	log_message("setup complete.");

	set_motion(g->new_motion, g->new_speed);
} // end SETUP()

void loop() {

/*	if (kilo_ticks > (g->memory_timer + MEM_WIPE_WAIT)) {

		log_message("Neighbors: memory wipe.");
		// reset clock
		g->memory_timer = kilo_ticks;

		// erase mailbox, neighbors
		uint8_t tmpTimestamp = 0;

		for (int i = 0; i < MAX_NEIGHBORS; i++) {
			tmpTimestamp = g->neighbor_table[i][0];
			if (((g->memory_timer - tmpTimestamp) >= MEM_WIPE_WAIT) && (tmpTimestamp != 0)) { // 0 for uninitialized
				for (int j = 0; j < NEIGHBOR_PARAMS; j++) {
					g->neighbor_table[i][j] = 0;
				} // end for
			}	  // end if
		}		  // end for
	}
*/

	if(g->halt_mode) {
		log_message("Halt in loop.");
	}

	if (!(g->halt_mode)) {
		
		// sort the mail (either it's been too long, or mailbox is full):
		if ((kilo_ticks > (g->mail_timer + 160)) || (g->full_mailbox == TRUE)) {

			//log_message("Opening the mail.");

			g->mail_timer = kilo_ticks; // reset clock

			if (g->full_mailbox == TRUE)
			{
				g->full_mailbox = FALSE;
			} // end if

			// Checks: 1. Proximity?  2. Bearing? 3. New? 4. Collision?
			uint8_t tmpID = BYTE_MAX;
			uint8_t tmpAlly = FALSE;
			uint8_t tmpDist = BYTE_MAX;
			uint8_t tmpMotion = BYTE_MAX;
			uint8_t tmpSpeed = BYTE_MAX;
			uint8_t is_neighbor = FALSE;
			uint8_t neighborIndex = 0;

			// open the mail:
			for (int i = 0; i < MAILBOX_SIZE; i++)  {

				// don't check empty slot
				if (g->mailbox[i][DIST] > 0)  {
					tmpID = g->mailbox[i][ID];
					tmpAlly = g->mailbox[i][TEAM]; // FALSE if adversary
					tmpDist = g->mailbox[i][DIST];
					tmpMotion = g->mailbox[i][MOTION + 1];
					tmpSpeed = g->mailbox[i][SPEED + 1];
					
					//log_message("Mailbox: ID %d Ally? %d Dist %d Mot %d Spd %d",\
						tmpID, tmpAlly, tmpDist, tmpMotion, tmpSpeed);

					// empty mailbox slot:
					for (int j = 0; j < MAIL_PARAMS; j++)  {
						g->mailbox[i][j] = 0; // reset to default
					} // end for

					// sweeps whole table
					for (uint8_t j = 0; j < MAX_NEIGHBORS; j++) {
						if (tmpID == g->neighbor_table[j][ID]){
							is_neighbor = TRUE;
							neighborIndex = j;
						} //is_neighbor/neighborIndex scopes are local; default to FALSE, 0
					}

					// update neighbor table
					if (is_neighbor) {
						//update neighbor flag
						is_neighbor = FALSE;
						//log_message("Found an old friend.");
						if ((kilo_ticks - g->memory_timer) < (BYTE_MAX + 1)) { // avoid overflow
							g->neighbor_table[neighborIndex][0] =\
								(uint8_t)(kilo_ticks - g->memory_timer); // cast to avoid
						} else {
							g->neighbor_table[neighborIndex][0] = BYTE_MAX; // time maxed-out
						}  // end if-else
						g->neighbor_table[neighborIndex][DIST] = tmpDist;
						g->neighbor_table[neighborIndex][MOTION + 1] = tmpMotion;
						g->neighbor_table[neighborIndex][SPEED + 1] = tmpSpeed;
					} else { 
						g->new_neighbor = TRUE;
						g->neighbor_table[g->newNeighborIndex][ID] = tmpID;
						//log_message("New neighbor detected!");
						if ((kilo_ticks - g->memory_timer) < (BYTE_MAX + 1)) {
							g->neighbor_table[g->newNeighborIndex][0] =
								(uint8_t)(kilo_ticks - g->memory_timer);
						} else {
							g->neighbor_table[g->newNeighborIndex][0] = BYTE_MAX;
						} // end if-else

						g->neighbor_table[g->newNeighborIndex][DIST] = tmpDist;
						g->neighbor_table[g->newNeighborIndex][MOTION + 1] = tmpMotion;
						g->neighbor_table[g->newNeighborIndex][SPEED + 1] = tmpSpeed;
						if (g->newNeighborIndex < MAX_NEIGHBORS) {
							g->newNeighborIndex = 0;
						} else {
							if (g->newNeighborIndex < MAX_NEIGHBORS) {
								g->newNeighborIndex++;
							} else {
								//log_message("Too many neighbors; starting from 0.");
								g->newNeighborIndex = 0;
							}
						}
					}
				} // end if-else-if, mail open

				//log_message("Neighbors: time %d ID %d dist %d %d %d", \
				*(&g->neighbor_table[i][0]), *(&g->neighbor_table[i][ID]),\
				*(&g->neighbor_table[i][DIST]), *(&g->neighbor_table[i][MOTION + 1]),\
				*(&g->neighbor_table[i][SPEED +1]));
				//log_message("Ally? %d allyDist: %d adversDist: %d closest: %d", tmpAlly, *(&g->closestAlly_dist), *(&g->closestAdversary_dist), *(&g->closest));

				if (tmpAlly == TRUE)  {
					if (tmpDist < g->closestAlly_dist) {
						//log_message("Hugs and kisses!");
						g->closestAlly_dist = tmpDist;
						// in future add velocity checking...
					} // end if
				} else {
					if (tmpDist < g->closestAdversary_dist)  {
						//log_message("Hey, buddy!  Watch it!");
						g->closestAdversary_dist = tmpDist;
					} // end if
				}	  // end if-else

				if ((g->closestAlly_dist < g->closest) || \
					(g->closestAdversary_dist < g->closest)) {
					// possible threat!
					g->closestID = tmpID; // keep watch

					// friend or foe?
					if (g->closestAlly_dist < g->closestAdversary_dist) {
						g->closest = g->closestAlly_dist;
					} else {
						g->proximity_alert = TRUE; // set the alarm
						g->closest = g->closestAdversary_dist;
					} // end if-else
				}	  // end if
			} // end for
		}
		// collision detection
		// divert maneuvers
		if (g->proximity_alert == TRUE) {
			
			g->closestAdversary_dist = BYTE_MAX;
			g->closestAlly_dist = BYTE_MAX;
			//log_message("Too close!");

			g->proximity_alert = FALSE; // reset alarm
			log_message("I got to proximity alert...");
			if (g->closest < TOO_CLOSE) {
					log_message("Halt mode.");
					// g->closestAdversary_dist = BYTE_MAX reset distance trigger
					// g->closestAlly_dist = BYTE_MAX		 reset per above
					g->halt_mode = TRUE;					// sound the alarm!
					g->is_seed = TRUE;
					log_message("I'm a seed.");
			}

			// prioritize threats
			// with different distance thresholds
			// identify friend/foe and flock or disperse


			/////////////////
			// NORMAL MODE://
			/////////////////
		} else if (kilo_ticks > (g->random_motion_timer + 128)) {
			// Nobody's watching!
			// Do your little Kilobot thing!!
			// update
			g->random_motion_timer = kilo_ticks;

			// time-discrete random walk
			g->previous_motion = g->new_motion;
			g->new_motion = rand_hard() % 4;
			g->previous_speed = g->new_speed;
			g->new_speed = rand_hard() % 3;
			set_motion(g->new_motion, g->new_speed); // reset motion

		} // end if-else-if
	} else {
		// go to SPECIAL MODE
		// STOP
		set_motors(0, 0);
		set_color(RGB(1, 1, 1));

		log_message("I'm in halt mode.");

		g->message_out.data[HALT] = TRUE;

		if (g->is_seed == TRUE) {
			g->message_out.data[5] = g->myID;
			g->message_out.data[6] = 0;
			g->message_out.crc = message_crc(&(g->message_out));
		} else {
			uint8_t seed_id = g->priority_mail.data[ID];
			uint8_t seed_team = g->priority_mail.data[TEAM];
			uint8_t seed_motion = g->priority_mail.data[MOTION];
			uint8_t seed_speed = g->priority_mail.data[SPEED];
			g->myGradient = GRADIENT_MAX;
			g->message_out.data[6] = g->myGradient;
			g->message_out.crc = message_crc(&(g->message_out));
		}

		show_gradient(&(g->myGradient));

		while (g->halt_mode) {
			// regulate message flow
			if (kilo_ticks > (g->halt_timer + 8)) {
				g->halt_timer = kilo_ticks;
				show_gradient(&(g->myGradient));
				if (g->new_message == TRUE) {
					g->new_message = FALSE;
					uint8_t thisGradient =\
						g->priority_mail.data[6];

					// activate gradient check
					if ((g->is_seed == FALSE) && (g->myGradient > (thisGradient + 1))) {

						log_message("My gradient is %d", *(&(g->myGradient)));

						g->myGradient = thisGradient + 1;
						g->message_out.data[6] = g->myGradient;
						g->message_out.crc = message_crc(&(g->message_out));
						show_gradient(&(g->myGradient));
					}
				}
			}
		}
	}
} // end LOOP()

int main() {

    GLOBALS* g_safe =  (GLOBALS*)malloc(sizeof(GLOBALS));

    #ifdef USING_SIMULATION
      // register the global variables (only necessary on simulation)
      kilo_globals = (void**)&g;
    #endif

    kilo_init();

	g = g_safe;

	kilo_message_rx = message_rx;
	kilo_message_tx = message_tx;
	kilo_message_tx_success = message_tx_success;

    

    #ifdef USING_SIMULATION
      kilo_start(setup, loop, message_rx, message_tx, message_tx_success);
    #else
      kilo_start(setup, loop);
    // free user defined globals
    free(g_safe);
    #endif

	return 0;

} // end MAIN()

// end program{}
