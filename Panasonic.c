#include <stdio.h>
#include <stdlib.h>
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
	
	int power; // 0: power button 1:others
        int clean; //1:clean 0:none
	int mode; // 10:cold 11:dehumidify 00:air supply 
        int wind; // 01:low 10:medium 11:high
        int temperature; // setting temperature - 15, range 16~30
        
	int setCode;
	int fixCode = 0x3604;
	int codes[271];
        int i, startCode, temp, bit;
	
	if (argc != 6) {
	    printf("Usage: Panasonic power clean mode wind temperature\n");
            printf("       power: 0 power button, 1 others\n");
            printf("       clean: 0 none, 1 self clean\n");
            printf("       mode: 2 cold, 3 dehumidify, 0 air supply\n");
            printf("       wind: 1 low, 2 medium, 3 high\n");
            printf("       temperature: 16~30 degree\n");
	    exit(1);
	}
	
	//collect arguments
	sscanf(argv[1],"%i",&power);
        sscanf(argv[2],"%i",&clean);
        sscanf(argv[3],"%i",&mode);
        sscanf(argv[4],"%i",&wind);
        sscanf(argv[5],"%i",&temperature);
	
	//compile arguments into setCode
	setCode = 0;
        setCode <<= 1;
	setCode |= (power & 1);
	setCode <<= 1;
	setCode |= (clean & 1);
	setCode <<= 2;
	setCode |= (mode & 3);
        setCode <<= 3;
	setCode |= (wind & 3);
	setCode <<= 5;
        setCode |= ((temperature-15) & 15);	
	
	printf("setCode = %04X\n", setCode);
	printf("fixCode = %04X\n", fixCode);
        
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


	//make codes from fixCode low 8 bits
	startCode = 138;
	temp = fixCode & 0xff;
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
	startCode = 170;
	temp = (fixCode >> 8) & 0xff;
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

	//set separation pulse/gap
        codes[134] = onePulse;
        codes[135] = separationGap;

	//set leading pulse/gap
	codes[136] = leadingPulseDuration;
	codes[137] = leadingGapDuration;

	codes[270] = onePulse; //sending trailing pulse

	//duplicate codes
        for(i=0; i<68;i++) codes[66+i]=codes[i];
        for(i=0; i<68;i++) codes[202+i]=codes[i+136];


	int result = irSlingRaw(
		outPin,
		frequency,
		dutyCycle,
		codes,
		sizeof(codes) / sizeof(int));
	
	return result;
}
