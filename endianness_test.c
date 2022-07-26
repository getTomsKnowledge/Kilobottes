/**

    Endianness Test

    @author Tom West
    @date 07/26/2022
    @version 0.0

*/


#include <stdio.h>
#include <stdint.h>

#define BYTE_SIZE 8


int main() {
    
    
        
    short testID = 0;
    uint8_t MSB = 0;
    uint8_t LSB = 0;
    uint8_t k = 0;
    
    testID = 1;
    uint8_t multiplier = sizeof(testID);

    printf("Short (2-byte) test variable = %d\n  Mem. size = %d\n", testID, multiplier);
    
    for (int i = 0; i < 2; i++) {
    
    	for (int j = ((BYTE_SIZE * multiplier) - 1); j >= 0; j--) {
    		k = testID >> j;
    	 	if (i >= BYTE_SIZE) {
    			MSB |= k << j; 
    		} else {
    			LSB |= k << j;
    		}
    	}//end for
    }//end for
    
    printf("Your processor is...");
    
    if ((LSB == 1) && (MSB == 0)) {
    	printf("\n\nLittle Endian!\n");
        printf("MSB:  %d\n", MSB);
        printf("LSB: %d\n", LSB);
    } else {
        printf("Big Endian!\n");
        printf("MSB:  %d\n", MSB);
        printf("LSB: %d\n", LSB);
    }
    
    return 0;
    
}//end main()
