#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int computeFP(float val);
float getFP(int val);
int
multVals(int source1, int source2);
int
addVals(int source1, int source2);
int deciToBits(float frac, int E);
int intToBits(int num);
float bitsToDeci(int norm, int size, int offSet);
void printBits(int n);
int revBits(int bits, int size);

int main() {
//	float M = 1.980713;
//	int E = 1;
//	float z = pow((-1),0)*M*pow(2,E);
	int i = computeFP(0.26);
//	printf("z: %f\n",z);
	printf("i: %d\n",i);
	float j = getFP(i);
	printf("j: %f\n",j);
/*		int i2 = computeFP(15.25);
	printf("i2: %d\n",i2);
	float j2 = getFP(i2);
	printf("j2: %f\n",j2);

	int k = addVals(6676, 9680);
	printf("k: %d\n",k);
	float l = getFP(k);
	printf("l: %f\n",l);

	int k2 = multVals(6676, 9680);
	printf("k2: %d\n",k2);
	float l2 = getFP(k2);
	printf("l2: %f\n",l2);
	return 0;*/
}

int
computeFP(float val) {
  float num = val; float frac = 0.0;
  int E = 0, final = 0;
  int normalized = 0x0, expBits = 0b0, fracBits = 0b0;

  while(!(num >= 1 && num < 2)){
    if(num < 1){
      num *= 2;
      E -= 1;
    } else { 
      num /= 2;
      E += 1;
    }   
  }

  printf("num: %f\n", num);
  printf("E: %d\n", E);

  if(E + 15 > 30) return -1; 
  if(E + 15 < 1) return 0;

final = floor(num);
  frac = num - final;
 printf("frac: %f\n",frac);
  fracBits = deciToBits(frac, E);
  printf("fracBits: %d\n",fracBits);
  expBits = E + 15;
  printf("expBits: %d\n",expBits);
  normalized = (expBits << 9) | fracBits;
  printf("normalized: %d\n",normalized);
  printBits(normalized);
  return normalized;
}

float getFP(int val) {
  int normalized = 0b0;
  int E = 0, expBits = 0b0, fracBits = 0b0;
  float M = 0.0;

  normalized = intToBits(val);
 // printf("normalized: %d\n",normalized);
  fracBits = normalized & 0b111111111;
//  printf("fracBits: %d\n",fracBits);
  expBits = (normalized >> 9) & 0b11111;
 // printf("expBits: %d\n",expBits);
  M = 1 + bitsToDeci(fracBits, 8, -9);
//  printf("M: %f\n",M);
  E = expBits - 15;
//  printf("E: %d\n",E);
// For denormalized values (including 0), simply return 0.
  if(E == 0) return 0;
// For special values, return -1;
  if(E == 31) return -1;
// Using the defined representation, compute the floating point
//    value
  return pow((-1),0)*M*pow(2,E);
}


int
multVals(int source1, int source2) {
  int num1 = 0b0, num2 = 0b0;
  int E1 = 0, E2 = 0, E = 0;
  float M1 = 0, M2 = 0, M = 0;
  int expBits1 = 0b0, fracBits1 = 0b0, expBits2 = 0b0, fracBits2 = 0b0;

  num1 = intToBits(source1);
  num2 = intToBits(source2);
  fracBits1 = num1 & 0b111111111;
  expBits1 = (num1 >> 9) & 0b11111;
  M1 = 1 + bitsToDeci(fracBits1, 8, -9);
  printf("fracBits1: %d\n",fracBits1);
  printf("M1: %f\n",M1);
  E1 = expBits1 - 15;
  fracBits2 = num2 & 0b111111111;
  expBits2 = (num2 >> 9) & 0b11111;
  M2 = 1 + bitsToDeci(fracBits2, 8, -9);
  printf("fracBits2: %d\n",fracBits2);
  printf("M2: %f\n",M2);
  E2 = expBits2 - 15;

// You must implement this by using the algorithm
//    described in class:
//    Add the exponents:  E = E1+E2 
//    multiply the fractional values: M = M1*M2
  E = E1 + E2;
  M = M1 * M2; 
  printf("M: %f\n",M);
//    if M too large, divide it by 2 and increment E
//    save the result

if(M >= 2) {
	M /= 2;
	E += 1;

}
  printf("E: %d\n",E);
  printf("M: %f\n",M);
// Be sure to check for overflow - return -1 in this case
  if(E + 15 > 30) return -1;
// Be sure to check for underflow - return 0 in this case
  if(E + 15 < 1) return 0;
  int k = computeFP(pow((-1),0)*M*pow(2,E));
  printf("computeFP: %d\n", k);
  return computeFP(pow((-1),0)*M*pow(2,E));
}

//8694

int
addVals(int source1, int source2) {
// Do this function last - it is the most difficult!
  int num1 = 0b0, num2 = 0b0;
  int E1 = 0, E2 = 0, E = 0, Ediff = 0;
  float M1 = 0, M2 = 0, M = 0;
  int expBits1 = 0b0, fracBits1 = 0b0, expBits2 = 0b0, fracBits2 = 0b0;

  num1 = intToBits(source1);
  num2 = intToBits(source2);
  fracBits1 = num1 & 0b111111111;
  expBits1 = (num1 >> 9) & 0b11111;
  M1 = 1 + bitsToDeci(fracBits1, 8, -9);
  E1 = expBits1 - 15;
   printf("fracBits1: %d\n",fracBits1);
  printf("M1: %f\n",M1);
  fracBits2 = num2 & 0b111111111;
  expBits2 = (num2 >> 9) & 0b11111;
  M2 = 1 + bitsToDeci(fracBits2, 8, -9);
  E2 = expBits2 - 15;
    printf("fracBits2: %d\n",fracBits2);
  printf("M2: %f\n",M2);
    printf("E1: %d\n",E1);
 printf("E2: %d\n",E2);
// You must implement this as described in class:
//     If needed, adjust one of the two number so that 
//        they have the same exponent E
  Ediff = E1 - E2;
  if(Ediff > 0) {
    E = E1;
    M2 /= pow(2, Ediff);
  } if(Ediff < 0) {
    E = E2;
    M1 /= pow(2, (-1)*Ediff);
  }
  printf("E1: %d\n",E1);
  printf("E2: %d\n",E2);
  printf("Ediff: %d\n",Ediff);
  printf("M1: %f\n",M1);
  printf("M2: %f\n",M2);
//     Add the two fractional parts:  F1' + F2 = F
//              (assumes F1' is the adjusted F1)
//     Adjust the sum F and E so that F is in the correct range
  M = M1 + M2;    
  while(M >= 2) {
    M /= 2.0;
    E += 1;
  }
  
  printf("E: %d\n",E);
  printf("M: %f\n",M);
// As described in the handout, you only need to implement this for
//    positive, normalized numbers
// Also, return -1 if the sum overflows
  if(E + 15 > 30) return -1;
  return computeFP(pow((-1),0)*M*pow(2,E));
}




















int deciToBits(float frac, int E) {
  int deciBits = 0b0; 
  int size = 0;//, iniIsOne = 0;
  float fraction = frac;

//	if(E >= 0) size = 0;
//	else size = 1;
//	if(fraction * 2 >= 1) iniIsOne = 1;
  while (size < 9) {
    fraction *= 2;
    if(fraction == 1) {
      deciBits |= 0b1;
      break;
    } else if (fraction > 1) {
			deciBits |= 0x1;
			if(size != 8)
				deciBits <<= 1;		
		fraction -= 1;
    } else {
		if(size != 8)
    		deciBits <<= 1;
    }
	printf("fraction: %f\n", fraction);
	printf("deciBits: ");
	printBits(deciBits);
	
    size += 1;
  }
	if (size < 9)
		deciBits <<= (9 - (size + 1));
///	if (iniIsOne != 1)
//		deciBits &= (0b011111111);

  	printf("DECIBITs: ");
	printBits(deciBits);
  return deciBits;
}

int intToBits(int num) {
  int expBits = 0b0;
  int expVal = num, trigger = num, size = 0;

  while (expVal != 0) {
    trigger = expVal % 2;
    if(trigger == 1) {
      expBits |= 0b1;
	  if(expVal / 2 != 0)
      	expBits <<= 1;
    } else {
      expBits <<= 1;
    }
    expVal /= 2;
    size += 1;
  }
  //	printf("EXPBITS: ");
	//printBits(expBits);

  return revBits(expBits, size);
}

float bitsToDeci(int norm, int size, int offSet) {
  int i = 0;
  float final = 0.0;

  for(i = size; i >= 0;  i--) {
    if(norm & (1 << i)) {
      final += pow(2,(offSet + i));
    }
  }
  
  return final;
}

void printBits(int n) {
	while (n) {
		if (n & 1)
			printf("1");
		else
			printf("0");
		n >>= 1;
	}
	printf("\n");
}

int revBits(int bits, int size) {
	int revBits = 0b0;

	for(int i = 0; i < size; i++) {
		if(bits & (1 << i)) {
		revBits |= 1 << ((size - 1) - i);
		}
	}
	//printf("REVERSEBITS: ");
	//printBits(revBits);
	return revBits;
}