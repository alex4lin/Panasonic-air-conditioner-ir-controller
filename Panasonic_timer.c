#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "irslinger.h"

int main(int argc, char *argv[])
{
	uint32_t outPin = 17;           // The Broadcom pin number the signal will be sent on
	int frequency = 38000;          // The frequency of the IR signal in Hz
	double dutyCycle = 0.5;         // The duty cycle of the IR signal. 0.5 means for every cycle,
	                                // the LED will turn on for half the cycle time, and off the other half
        
	int separationGap = 14680;
	int leadingPulseDuration = 3497;
	int leadingGapDuration = 3470;
	int onePulse = 894;
	int zeroPulse = 894;
	int oneGap = 2601;
	int zeroGap = 856;
	int sendTrailingPulse = 1;
	
	int cancelCode = 0x3296;
        
	int command; // reserve:1  or cancel:0
        int timer; // on: 1 or off:0
	int hours; // hours

	int setCode;
	int codes[201];
        int i, startCode, temp, bit;
	
	if (argc != 2 && argc != 4) {
	    printf("Usage: Panasonic_timer command timer hour\n");
            printf("       command: reserve or cancel\n");
            printf("       timer: on or off (after time up)\n");
            printf("       hours: 1~12\n");
	    exit(1);
	}
	
	//collect arguments
	if (argc == 2 && strcmp(argv[1],"cancel")==0) {
            setCode = cancelCode;	
	} else {
	    if (argc == 4 && strcmp(argv[1],"reserve")==0) {
	       
	       timer = -1;
               if(strcmp(argv[2],"on")==0) timer = 1;
               if(strcmp(argv[2],"off")==0) timer = 0;
               if (timer == -1) exit(-1);
               
	       sscanf(argv[3],"%i",&hours);
               if (hours>12 || hours<1) exit(-1);

	       if (timer==1){
		   setCode = 16 - hours;
                   setCode <<= 4;
		   setCode |= 0x1000;
	       } else {
		   setCode = 16 - hours;
		   setCode |= 0x1000;
	       };
            } else exit(-1);  
	};

	printf("setCode = %04X\n", setCode);
        
	//make codes from setCode low 8 bits
	startCode = 2;
	temp = setCode & 0xff;
        for(i=0; i<8; i++){
	    bit = temp & 1;
            if (bit == 0) {
                codes[startCode+i*2] = zeroPulse;
		codes[startCode+i*2+1] = zeroGap;
	    
                codes[startCode+i*2+16] = zeroPulse;
		codes[startCode+i*2+17] = zeroGap;
	    } else{
                codes[startCode+i*2] = onePulse;
		codes[startCode+i*2+1] = oneGap;
	    
                codes[startCode+i*2+16] = onePulse;
		codes[startCode+i*2+17] = oneGap;
	    };
            temp >>= 1;	
	}

	//make codes from setCode high 8 bits
	startCode = 34;
	temp = (setCode >> 8) & 0xff;
        for(i=0; i<8; i++){
	    bit = temp & 1;
            if (bit == 0) {
                codes[startCode+i*2] = zeroPulse;
		codes[startCode+i*2+1] = zeroGap;
	    
                codes[startCode+i*2+16] = zeroPulse;
		codes[startCode+i*2+17] = zeroGap;
	    } else{
                codes[startCode+i*2] = onePulse;
		codes[startCode+i*2+1] = oneGap;
	    
                codes[startCode+i*2+16] = onePulse;
		codes[startCode+i*2+17] = oneGap;
	    };
            temp >>= 1;	
	}


	//set leading pulse/gap
        codes[0] = leadingPulseDuration;
        codes[1] = leadingGapDuration;

	//set leading pulse/gap
	codes[198] = leadingPulseDuration;
	codes[199] = leadingGapDuration;

	codes[200] = onePulse; //sending trailing pulse

	//duplicate codes
        for(i=0; i<66;i++) {
	    codes[66+i]=codes[i];
	    codes[132+i]=codes[i];
        }

	int result = irSlingRaw(
		outPin,
		frequency,
		dutyCycle,
		codes,
		sizeof(codes) / sizeof(int));
	
	return result;
}
