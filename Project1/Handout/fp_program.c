#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fpParse.h"

typedef enum {PRINT_OP, ASSIGN_OP, ADD_OP, MULT_OP} operation;

typedef struct l {
   operation op;
   int id1,id2,id3;
   float fpv;
} op_rec;

// Prototypes
void execute_op();
void match(int token);
int is_id(int token);
void line();
void print();
void assign();

extern int yylex();
extern void test_it();

// shared with the lexer
extern int lineno;
extern float fp_val;

// functions you will implement are:
extern int computeFP(float val);
extern float getFP(int val) ;
extern int addVals(int source1, int source2); 
extern int multVals(int source1, int source2); 


int variable[27];
op_rec current_operation;

int lookahead;


int main(int argc, char**argv) {
   if ((argc == 2) && (strcmp(argv[1],"-t")==0)) {
      test_it();  exit(1);
   }
   printf("> ");
   lookahead = yylex();
   while (is_id(lookahead) || (lookahead==PRINT)) {line(); }
   printf("Exiting\n");
   return 0;
}

// Calls the functions you will implement based on the input program

void execute_op() {
   switch(current_operation.op) {
      case ASSIGN_OP: 
	variable[current_operation.id1] =  computeFP(current_operation.fpv); 
	break;

	case ADD_OP:
      	variable[current_operation.id1] =
        	addVals(variable[current_operation.id2],
			variable[current_operation.id3]); 
		break;

	case MULT_OP:
      	variable[current_operation.id1] =
       		multVals(variable[current_operation.id2],
			variable[current_operation.id3]); 
		break;
   }
}

// The code below is a recursive descent parser that processes the input
//    program and builds the current_operation information.  If there is a
//    syntax error in the input, the program will halt.  Once a line has
//    been processed, execute_op() is called. 
void match(int token) {
   if (token == lookahead)  lookahead = yylex();
   else {
      printf("Unexpected input\n");
      exit(2);
   }
}

int is_id(int token) {
   return ((token > 0) && (token <=26));
}

void line() {
   if (lookahead == PRINT) print(); else {
     assign();
     execute_op();
   }
}


void print() {
  match(PRINT);
  current_operation.op = PRINT_OP;
  current_operation.id1 = lookahead;
  if (is_id(lookahead)) match(lookahead); 
  else {printf("ID expected line %d\n",lineno); exit(3);}
  printf("%c = %0.10f\n",current_operation.id1+'a' -1,
  	   getFP(variable[current_operation.id1])); 
  printf("> ");
  match(EOLN);
}

void assign() {
  current_operation.id1 = lookahead;
  if (is_id(lookahead)) match(lookahead);
  else {printf("ID expected line %d\n",lineno); exit(3);}
  match(ASSIGN);

  if (lookahead == FLOAT) {
     current_operation.op = ASSIGN_OP;
     current_operation.fpv = fp_val;
     match(FLOAT);
  } else {
    current_operation.id2 = lookahead;
    if (is_id(lookahead)) match(lookahead);
    else {printf("ID expected line %d\n",lineno); exit(3);}
    if (lookahead == PLUS) {
	match(PLUS);
        current_operation.op = ADD_OP;
    } else {
	match(MULT);
        current_operation.op = MULT_OP;
    }
    current_operation.id3 = lookahead;
    if (is_id(lookahead)) match(lookahead);
    else {printf("ID expected line %d\n",lineno); exit(3);}
  }
  printf("> ");
  match(EOLN);
 }


