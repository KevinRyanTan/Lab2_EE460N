/*
REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

Name 1: Kevin Tan
Name 2: Enrique Callado
UTEID 1: kct566
UTEID 2: eic296
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1


/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct {

            int PC,		/* program counter */
                N,		/* n condition bit */
		Z,		/* z condition bit */
		P;		/* p condition bit */
	int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
	printf("----------------LC-3b ISIM Help-----------------------\n");
	printf("go               -  run program to completion         \n");
	printf("run n            -  execute program for n instructions\n");
	printf("mdump low high   -  dump memory from low to high      \n");
	printf("rdump            -  dump the register & bus values    \n");
	printf("?                -  display this help menu            \n");
	printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {
	process_instruction();
	CURRENT_LATCHES = NEXT_LATCHES;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
	int i;

	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating for %d cycles...\n\n", num_cycles);
	for (i = 0; i < num_cycles; i++) {
		if (CURRENT_LATCHES.PC == 0x0000) {
			RUN_BIT = FALSE;
			printf("Simulator halted\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating...\n\n");
	while (CURRENT_LATCHES.PC != 0x0000)
		cycle();
	RUN_BIT = FALSE;
	printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
	int address; /* this is a byte address */

	printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	printf("-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	printf("\n");

	/* dump the memory contents into the dumpsim file */
	fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	fprintf(dumpsim_file, "-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
	int k;

	printf("\nCurrent register/bus values :\n");
	printf("-------------------------------------\n");
	printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
	printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	printf("Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	printf("\n");

	/* dump the state information into the dumpsim file */
	fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
	fprintf(dumpsim_file, "-------------------------------------\n");
	fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
	fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	fprintf(dumpsim_file, "Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
	char buffer[20];
	int start, stop, cycles;

	printf("LC-3b-SIM> ");

	scanf("%s", buffer);
	printf("\n");

	switch (buffer[0]) {
	case 'G':
	case 'g':
		go();
		break;

	case 'M':
	case 'm':
		scanf("%i %i", &start, &stop);
		mdump(dumpsim_file, start, stop);
		break;

	case '?':
		help();
		break;
	case 'Q':
	case 'q':
		printf("Bye.\n");
		exit(0);

	case 'R':
	case 'r':
		if (buffer[1] == 'd' || buffer[1] == 'D')
			rdump(dumpsim_file);
		else {
			scanf("%d", &cycles);
			run(cycles);
		}
		break;

	default:
		printf("Invalid Command\n");
		break;
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
	int i;

	for (i = 0; i < WORDS_IN_MEM; i++) {
		MEMORY[i][0] = 0;
		MEMORY[i][1] = 0;
	}
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
	FILE * prog;
	int ii, word, program_base;

	/* Open program file. */
	prog = fopen(program_filename, "r");
	if (prog == NULL) {
		printf("Error: Can't open program file %s\n", program_filename);
		exit(-1);
	}

	/* Read in the program. */
	if (fscanf(prog, "%x\n", &word) != EOF)
		program_base = word >> 1;
	else {
		printf("Error: Program file is empty\n");
		exit(-1);
	}

	ii = 0;
	while (fscanf(prog, "%x\n", &word) != EOF) {
		/* Make sure it fits. */
		if (program_base + ii >= WORDS_IN_MEM) {
			printf("Error: Program file %s is too long to fit in memory. %x\n",
				program_filename, ii);
			exit(-1);
		}

		/* Write the word to memory array. */
		MEMORY[program_base + ii][0] = word & 0x00FF;
		MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
		ii++;
	}

	if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

	printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
	int i;

	init_memory();
	for (i = 0; i < num_prog_files; i++) {
		load_program(program_filename);
		while (*program_filename++ != '\0');
	}
	CURRENT_LATCHES.Z = 1;
	NEXT_LATCHES = CURRENT_LATCHES;

	RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
	FILE * dumpsim_file;

	/* Error Checking */
	if (argc < 2) {
		printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
			argv[0]);
		exit(1);
	}

	printf("LC-3b Simulator\n\n");

	initialize(argv[1], argc - 1);

	if ((dumpsim_file = fopen("dumpsim", "w")) == NULL) {
		printf("Error: Can't open dumpsim file\n");
		exit(-1);
	}

	while (1)
		get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
You are allowed to use the following global variables in your
code. These are defined above.

MEMORY

CURRENT_LATCHES
NEXT_LATCHES

You may define your own local/global variables and functions.
You may use the functions to get at the control bits defined
above.

Begin your code here 	  			       */

/***************************************************************/

void setCCs(int CC_SETTER);
int signExtend(int extend,int bit);

void process_instruction() {
	/*  function: process_instruction
	*
	*    Process one instruction at a time
	*       -Fetch one instruction
	*       -Decode
	*       -Execute
	*       -Update NEXT_LATCHES
	*/
	int currentPC = CURRENT_LATCHES.PC;
	int lowByte = MEMORY[currentPC >> 1][0];
	int highByte = MEMORY[currentPC >> 1][1];
	int DR, SR1, SR2, operand1,operand2, CC_SETTER = 0;
	
        
        NEXT_LATCHES = CURRENT_LATCHES;
	/**********************************ADD*********************************************/
	if (highByte >> 4 == 1)
	{
		DR = (highByte >> 1) & 0x7;
		SR1 = (highByte & 0x1 << 2) + ((lowByte >> 6) & 0x3);
                
                operand1 = CURRENT_LATCHES.REGS[SR1];
                if(operand1 & 0x8000) operand1 = signExtend(operand1, 16);
                        
                /*imm5*/
                if (lowByte & 0x20) {
                        operand2 = (lowByte & 0x1F);
                        /*if immediate is negative, sign extend it, otherwise stays the same*/
                        if (operand2 & 0x10) operand2 = signExtend(operand2, 5);
		}
		/*2 SR*/
                else {
                        SR2 = lowByte & 0x07;
                        operand2 = CURRENT_LATCHES.REGS[SR2];
                        if(operand2 & 0x8000) operand2 = signExtend(operand2, 16);
    
                }
                CC_SETTER = operand1 + operand2;
                setCCs(CC_SETTER);
                NEXT_LATCHES.REGS[DR] = Low16bits(CC_SETTER);
                NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);        
	}
	/*************************************AND******************************************/
	if (highByte >> 4 == 5)
	{  
                DR = (highByte >> 1) & 0x7;
		SR1 = (highByte & 0x1 << 2) + ((lowByte >> 6) & 0x3);
                
                operand1 = CURRENT_LATCHES.REGS[SR1];
                if(operand1 & 0x8000) operand1 = operand1 = signExtend(operand1, 16);
               
                /*imm5*/
                if (lowByte & 0x20) {
                        operand2 = (lowByte & 0x1F);
                        /*if immediate is negative, sign extend it, otherwise stays the same*/
                        if (operand2 & 0x10) operand2 = signExtend(operand2, 5);
		}
		/*2 SR*/
                else {
                        SR2 = lowByte & 0x07;
                        operand2 = CURRENT_LATCHES.REGS[SR2];
                        if(operand2 & 0x8000) operand2 = signExtend(operand2, 16);
                }
                CC_SETTER = operand1 & operand2;
                setCCs(CC_SETTER);
                NEXT_LATCHES.REGS[DR] = Low16bits(CC_SETTER);
                NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2); 
	}
	/*************************************BR*****************************************/
	if (highByte >> 4 == 0)
	{
            NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
            if((CURRENT_LATCHES.N & ((highByte >> 3) & 0x1)) ||(CURRENT_LATCHES.Z & ((highByte >> 2) & 0x1)) || (CURRENT_LATCHES.P & ((highByte >> 1) & 0x1)) ){
                operand1 = ((highByte & 0x1)<< 8) + (lowByte & 0xFF);
                if(operand1 & 0x100) operand1 = signExtend(operand1, 9);
                operand1 = operand1 << 1;
                NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + operand1);
            }
	}
	/*********************************JMP/RET**************************************/
	if (highByte >> 4 == 12)
	{
            SR1 = ((highByte & 0x1) << 2) + ((lowByte & 0xC0) >> 6);
            NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[SR1]);
	}
	/**********************************JSR*****************************************/
        /*error handling for uneven address needed?*/
	if (highByte >> 4 == 4)
	{
            if(highByte & 0x08){
                operand1 = ((highByte & 0x7) << 8) + (lowByte & 0xFF);
                if(operand1 & 0x400) operand1 = signExtend(operand1, 11);
                operand1 = operand1 << 1;
                NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2 + operand1);
            }
            else{
                SR1 = ((highByte & 0x1) << 2) + ((lowByte & 0xC0) >> 6);
                NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[SR1]);
            }
            NEXT_LATCHES.REGS[7] = Low16bits(CURRENT_LATCHES.PC + 2);
	}
	/*********************************LDB*****************************************/
	if (highByte >> 4 == 2)
	{
            DR = (highByte >> 1) & 0x7;
            SR1 = ((highByte & 0x1) << 2) + ((lowByte >> 6) & 0x3);
            operand1 = CURRENT_LATCHES.REGS[SR1];
            operand2 = lowByte & 0x3F;
            if(operand2 & 0x20) operand2 = signExtend(operand2, 6);
            int address = operand1 + operand2;
            int waddr = address >> 1;
            int hilo = address & 1;
            CC_SETTER = 0xFF & (MEMORY[waddr][hilo]);
            if(CC_SETTER & 0x80) CC_SETTER = signExtend(CC_SETTER, 8);
            NEXT_LATCHES.REGS[DR] = Low16bits(CC_SETTER);
            setCCs(CC_SETTER);   
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}
	/**********************************LDW****************************************/
	if (highByte >> 4 == 6)
	{
            DR = (highByte >> 1) & 0x7;
            SR1 = ((highByte & 0x1) << 2) + ((lowByte >> 6) & 0x3);
            operand1 = CURRENT_LATCHES.REGS[SR1];
            operand2 = lowByte & 0x3F;
            if(operand2 & 0x20) operand2 = signExtend(operand2, 6);
            operand2 = operand2 << 1;
            int address = operand1 + operand2;
            CC_SETTER = ((MEMORY[address >> 1][1] & 0xFF) << 8) + (MEMORY[address >> 1][0] & 0xFF);
            if(CC_SETTER & 0x8000) CC_SETTER = signExtend(CC_SETTER, 16);
            NEXT_LATCHES.REGS[DR] = Low16bits(CC_SETTER);
            setCCs(CC_SETTER); 
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}
	/*********************************LEA*****************************************/
	/*Should NOT  set condition codes*/
	if (highByte >> 4 == 14)
	{
            DR = (highByte >> 1) & 0x7;
            operand1 = ((highByte & 0x1) << 8) + (lowByte & 0xFF);
            if(operand1 & 0x100) operand1 = signExtend(operand1, 9);
            operand1 = operand1 << 1;
            NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.PC + 2 + operand1);
            NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);

	}
	/*SHF*/
	if (highByte >> 4 == 13)
	{
            int flag = (lowByte >> 4) & 0x11;
		operand1 = lowByte & 0x0F;
		operand2 = CURRENT_LATCHES.REGS[SR1];
		if (flag == 0) {
			NEXT_LATCHES.REGS[DR] = CC_SETTER = operand2 << operand1;
		}
		/*logical*/
		else if (flag == 1) {
			NEXT_LATCHES.REGS[DR] = CC_SETTER = ((unsigned)operand2) >> operand1;
		}
		/*arithmetic*/
		else {
			if (operand2 & 0x8000) operand2 = signExtend(operand2, 16);
			CC_SETTER = operand2 >> operand1;
			NEXT_LATCHES.REGS[DR] = Low16bits(CC_SETTER);
		}
		setCCs(CC_SETTER);
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}
	/********************************STB*******************************************/
	if (highByte >> 4 == 3)
	{
            /*SR1 has info to store*/
            SR1 = (highByte >> 1) & 0x7;
            SR2 = ((highByte & 0x1) << 2) + ((lowByte >> 6) & 0x3); 
            operand1 = CURRENT_LATCHES.REGS[SR2];
            if(operand1 & 0x8000) operand1 = signExtend(operand1,16);
            operand2 = lowByte & 0x3F;
            if(operand2 & 0x20) operand2 = signExtend(operand2, 6);
            int address = operand1 + operand2;
            MEMORY[address >> 1][address & 0x1] = CURRENT_LATCHES.REGS[SR1] & 0xFF;
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;            
	}
	/*********************************STW******************************************/
	if (highByte >> 4 == 7)
	{   
            SR1 = (highByte >> 1) & 0x7;
            SR2 = ((highByte & 0x1) << 2) + ((lowByte >> 6) & 0x3); 
            operand1 = CURRENT_LATCHES.REGS[SR2];
            if(operand1 & 0x8000) operand1 = signExtend(operand1,16);
            operand2 = lowByte & 0x3F;
            if(operand2 & 0x20) operand2 = signExtend(operand2, 6);
            operand2 = operand2 >> 1;
            int address = operand1 + operand2;
            MEMORY[address >> 1][0] = CURRENT_LATCHES.REGS[SR1] & 0xFF;
            MEMORY[address >> 1][1] = CURRENT_LATCHES.REGS[SR1] & 0xFF00 >> 8;
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2; 
	}
	/*TRAP*/
	if (highByte >> 4 == 15)
	{
            /*r7 loaded with incremented pc*/	
            NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC + 2;
		NEXT_LATCHES.PC = MEMORY[lowByte][0];
	}
	/*XOR*/
	if (highByte >> 4 == 9)
	{
		DR = (highByte >> 1) & 0x7;
		SR1 = (highByte & 0x1 << 2) + ((lowByte >> 6) & 0x3);
		/*imm5*/
		operand1 = lowByte & 0x1F;
		if (operand1 & 0x10) operand1 = signExtend(operand1, 5);
		/*SR*/
		operand2 = CURRENT_LATCHES.REGS[SR1];
		if (operand2 & 0x8000) operand2 = signExtend(operand2, 16);

		/*imm5*/
		if (lowByte & 0x20 == 1) {
			/*NOT*/
			if (operand1 == 0x1F) {
				NEXT_LATCHES.REGS[DR] = Low16bits(!(operand2));
			}
			else {
				CC_SETTER = operand2 ^ (operand1);
				NEXT_LATCHES.REGS[DR] = Low16bits(CC_SETTER);
			}
		}
		/*2 SR*/
		else {
			SR2 = CURRENT_LATCHES.REGS[lowByte & 0x7];
			if (SR2 & 0x8000) SR2 = signExtend(SR2, 16);
			CC_SETTER = operand2 ^ SR2;
			NEXT_LATCHES.REGS[DR] = Low16bits(CC_SETTER);
		}
		setCCs(CC_SETTER);
		NEXT_LATCHES.PC = NEXT_LATCHES.PC + 2;
	}
}

void setCCs(int CC_SETTER){
    NEXT_LATCHES.N = (CC_SETTER < 0);
    NEXT_LATCHES.P = (CC_SETTER > 0);
    NEXT_LATCHES.Z = (CC_SETTER == 0);
}

int signExtend(int extend,int bit)
{
    int mask = 0xFFFF0000;
    mask = mask >> (16-bit);	
    return extend+mask;
}