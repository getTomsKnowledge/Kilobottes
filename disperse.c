#include "kilolib.h"

#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

int cur_motion = STOP;
int new_message = 0;
uint32_t last_changed = 0;

int random_number = 0;
int dice = 0;


message_t message;

message_t *message_tx(){
	return &message;
}

void message_rx(message_t *message, distance_measurement_t *distance)
{
    // Set the flag on message reception.
    new_message = 1;
}

void set_motion(int new_motion){
    if(cur_motion != new_motion){
        if(new_motion == STOP){
            cur_motion = 0;
            set_motors(0,0);
        }
        else if (new_motion == FORWARD){
            cur_motion = 1;
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
        }
        else if (new_motion == LEFT){
            cur_motion = 2;
            set_motors(255, 0);
            delay(15);
            set_motors(kilo_turn_left, 0);
        }
        else {
            cur_motion = 3;
            spinup_motors();
            set_motors(0, 255);
            delay(15);
            set_motors(0, kilo_turn_right);
        }
    }
}

void setup(){

    // Initialize message:
    // The type is always NORMAL.
    message.type = NORMAL;
    // It's important that the CRC is computed after the data has been set;
    message.data[0] = 0;
    // otherwise it would be wrong and the message would be dropped by the
    // receiver.
    message.crc = message_crc(&message);
    last_changed = kilo_ticks;


}

void loop(){

    //dice stuff:
    random_number = rand_hard();
    dice = (random_number % 4); // C syntax for computing modulo 4

    //disperse stuff:
    
    if (kilo_ticks > (last_changed + 32)){
		last_changed = kilo_ticks;
        if (new_message == 1)
        {
            new_message = 0;
            
            // Generate an 8-bit random number (between 0 and 2^8 - 1 = 255).
            int random_number = rand_hard();
            
            // Compute the remainder of random_number when divided by 4.
            // This gives a new random number in the set {0, 1, 2, 3}.
            int random_direction = (random_number % 4);
            
            // There is a 50% chance of random_direction being 0 OR 1, in which
            // case set the LED green and move forward.
            if ((random_direction == 0) || (random_direction == 1))
            {
                set_color(RGB(0, 1, 0));
                set_motion(FORWARD);
            }
            // There is a 25% chance of random_direction being 2, in which case
            // set the LED red and move left.
            else if (random_direction == 2)
            {
                set_color(RGB(1, 0, 0));
                set_motion(LEFT);
            }
            // There is a 25% chance of random_direction being 3, in which case
            // set the LED blue and move right.
            else if (random_direction == 3)
            {
                set_color(RGB(0, 0, 1));
                set_motion(RIGHT);
            }
        }
        // If no messages were received within the last second, set the LED white
        // and stop moving.
        else
        {
            set_color(RGB(1, 1, 1));
            set_motion(STOP);
        }
    }
      

}

int main(){
    
    kilo_init();
   
    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;
   
    kilo_start(setup, loop);
   
    return 0;

}