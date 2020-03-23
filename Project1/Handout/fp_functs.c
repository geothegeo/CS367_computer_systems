// George Tang, G01068335
// CS 367
// Project 1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fp.h"

// Global constants ................................................................
#define bias 15
#define expSize 9

// Prototypes ......................................................................
// helper functions
int fracToBits(float frac);
float bitsToFrac(int norm);

// Function that takes in the float value to be represented and returns an integer version
int computeFP(float val) {
  float num = val; int E = 0;
  int normalized = 0b0, expBits = 0b0, fracBits = 0b0;

  // divide or multiply the value by 2
  // until it is in the correct range (between 1 and 2).
  while(!(num >= 1 && num < 2)){
    if(num < 1){
      num *= 2;
      E -= 1;
    } else { 
      num /= 2;
      E += 1;
    }
  }

  // Check for overflow and underflow
  // for overflow (exp > 30), return -1
  if(E + bias > 30) return -1;
  // For underflow (exp < 1), return 0 
  if(E + bias < 1) return 0;
  
  // convert the exp and fraction to binary, merge them, then return the integer
  fracBits = fracToBits(num - 1);
  expBits = E + bias;
  normalized = (expBits << expSize) | fracBits;
  return normalized;
}

// Function that takes in an integer value and returns the float version
float getFP(int val) {
  float num = 0.0, M = 0; int E = 0;
  int normalized = 0b0, expBits = 0b0, fracBits = 0b0;

  // extract the bits and get the values for the Mantissa and Exponent
  normalized = val;
  fracBits = normalized & 0b111111111;
  expBits = (normalized >> expSize) & 0b11111;
  M = 1 + bitsToFrac(fracBits);
  E = expBits - bias;

  // For denormalized values (including 0), simply return 0.
  if(E == 0) return 0;
  // For special values, return -1;
  if(E == 31) return -1;

  // Using the defined representation, compute the floating point value
  return pow((-1),0)*M*pow(2,E);
}

// Function that takes in two integers and breaks them down into their respective
// float versions before multiplying them together
int multVals(int source1, int source2) {
  int num1 = 0b0, num2 = 0b0;
  int E1 = 0, E2 = 0, E = 0;
  float M1 = 0, M2 = 0, M = 0;
  int expBits1 = 0b0, fracBits1 = 0b0, expBits2 = 0b0, fracBits2 = 0b0;

  // extract the bits and get the values for the Mantissa and Exponent for respective integers
  num1 = source1; 
  fracBits1 = num1 & 0b111111111;
  expBits1 = (num1 >> expSize) & 0b11111;
  M1 = 1 + bitsToFrac(fracBits1);
  E1 = expBits1 - bias;
  
  num2 = source2;
  fracBits2 = num2 & 0b111111111;
  expBits2 = (num2 >> expSize) & 0b11111;
  M2 = 1 + bitsToFrac(fracBits2);
  E2 = expBits2 - bias;

  // Add the exponents:  E = E1+E2 
  E = E1 + E2;
  // multiply the fractional values: M = M1*M2
  M = M1 * M2; 

  // if M too large, divide it by 2 and increment E and save the result
  if(M >= 2){
    M /= 2;
    E += 1;
  }

  // check for overflow - return -1 in this case
  if(E + bias > 30) return -1;
  // check for underflow - return 0 in this case
  if(E + bias < 1) return 0;

  return computeFP(pow((-1),0)*M*pow(2,E));
}

// Function that takes in two integers and breaks them down into their respective
// float versions before adding them together
int addVals(int source1, int source2) {
// Do this function last - it is the most difficult!
  int num1 = 0b0, num2 = 0b0;
  int E1 = 0, E2 = 0, E = 0, Ediff = 0;
  float M1 = 0, M2 = 0, M = 0;
  int expBits1 = 0b0, fracBits1 = 0b0, expBits2 = 0b0, fracBits2 = 0b0;

  // extract the bits and get the values for the Mantissa and Exponent for respective integers
  num1 = source1;
  fracBits1 = num1 & 0b111111111;
  expBits1 = (num1 >> expSize) & 0b11111;
  M1 = 1 + bitsToFrac(fracBits1);
  E1 = expBits1 - bias;

  num2 = source2;
  fracBits2 = num2 & 0b111111111;
  expBits2 = (num2 >> expSize) & 0b11111;
  M2 = 1 + bitsToFrac(fracBits2);
  E2 = expBits2 - bias;

  // If needed, adjust one of the two number so that they have the same exponent E.
  // Find the larger of the two E's and manipulate the M's accordingly
  Ediff = E1 - E2;
  if(Ediff > 0) {
    E = E1;
    M2 /= pow(2, Ediff);
  } if(Ediff < 0) {
    E = E2;
    M1 /= pow(2, (-1)*Ediff);
  }

  // Add the two fractional parts:
  M = M1 + M2;
  // Adjust the sum F and E so that F is in the correct range      
  while(M >= 2) {
    M /= 2.0;
    E += 1;
  }

  // Also, return -1 if the sum overflows
  if(E + bias > 30) return -1;

  return computeFP(pow((-1),0)*M*pow(2,E));
}

// Helper Function to turn the convert the fraction part to a 9-bit binary representation
int fracToBits(float frac) {
  int deciBits = 0b0; 
  int size = 0;
  float fraction = frac;

  // while the 9 bits have not been occupied yet, fill in the bits from left to right
  while (size < expSize) {
    fraction *= 2;
    // if there is a 1, end the loop early (possibly)
    if(fraction == 1) {
      deciBits |= 0b1;
      break;
    // flip the bit to 1 and shift, not to the last bit
    } else if (fraction > 1) {
			deciBits |= 0x1;
			if(size != 8)
				deciBits <<= 1;		
		  fraction -= 1;
    // only shift, not to the last bit
    } else {
			if(size != 8)
				deciBits <<= 1;	
    }
    size += 1;
  }
  // if 9 bits have not been filled, then shift to fill in the "remaining bits"
	if (size < expSize)
		deciBits <<= (expSize - (size + 1));
    
  return deciBits;
}

// Helper Function to turn the convert the fraction part to a 9-bit binary representation
float bitsToFrac(int norm) {
  int i = 0;
  float final = 0.0;

  // sum up the values according to 2 ^ (-exponent), starting from the left most frac bit.
  for(i = 8; i >= 0;  i--) {
    if(norm & (1 << i)) {
      final += pow(2,(-9 + i));
    }
  }
  
  return final;
}