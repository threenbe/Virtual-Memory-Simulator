/*
    Name 1: Raiyan Chowdhury
    UTEID 1: rac4444
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

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
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND2, COND1, COND0,        /*COND2 is new*/
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX,              /*DRMUX1 is new*/
    SR1MUX1, SR1MUX,            /*SRMUX1 is new*/
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
    GATE_DEC_PC,
    LD_PRIV,
    PRIVMUX,
    GATE_PSR,
    GATE_VECTOR,
    VECMUX,
    CCMUX,
    LD_USP,
    LD_SSP,
    SPMUX1, SPMUX0,
    GATE_SP,
    LD_VECTOR,
    LD_VA,
    GATE_VA,
    GATE_PTBR,
    Has_PTE,
    SET_MR,
    SJ,
    VAT,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
                            (x[J3] << 3) + (x[J2] << 2) +
                            (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetGATE_DEC_PC(int* x)   { return(x[GATE_DEC_PC]); }
int GetLD_PRIV(int* x)       { return(x[LD_PRIV]); }
int GetPRIVMUX(int* x)       { return(x[PRIVMUX]); }
int GetGATE_PSR(int* x)      { return(x[GATE_PSR]); }
int GetGATE_VECTOR(int* x)   { return(x[GATE_VECTOR]); }
int GetVECMUX(int* x)        { return(x[VECMUX]); }
int GetCCMUX(int* x)         { return(x[CCMUX]); }
int GetLD_USP(int* x)        { return(x[LD_USP]); }
int GetLD_SSP(int* x)        { return(x[LD_SSP]); }
int GetSPMUX(int* x)         { return((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetGATE_SP(int* x)       { return(x[GATE_SP]); }
int GetLD_VECTOR(int* x)     { return(x[LD_VECTOR]); }
int GetLD_VA(int* x)         { return(x[LD_VA]); }
int GetGATE_VA(int* x)       { return(x[GATE_VA]); }
int GetGATE_PTBR(int* x)      { return(x[GATE_PTBR]); }
int GetHas_PTE(int* x)       { return(x[Has_PTE]); }
int GetSET_MR(int* x)        { return(x[SET_MR]); }
int GetSJ(int* x)            { return(x[SJ]); }
int GetVAT(int* x)           { return(x[VAT]); }

/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];
int num_mem_cycles = 0;

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
int USP;
int INT_;
int EXC;
int PSR;
int Vector;
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */

/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
int Saved_J;
int is_W;
/* MODIFY: you should add here any other registers you need to implement virtual memory */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
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

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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

    switch(buffer[0]) {
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
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
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

    for (i=0; i < WORDS_IN_MEM; i++) {
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
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

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

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
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
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *pagetable_filename, char *program_filename, int num_prog_files) { 
    int i;
    init_control_store(ucode_filename);

    init_memory();
    load_program(pagetable_filename,0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(program_filename,1);
	while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.PSR = 0x8002; /*PSR[15] is initialized to 1, and PSR[2:0] is 010 for NZP*/

/* MODIFY: you can add more initialization code HERE */

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
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argv[3], argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

int marmux_data;
int alu_data;
int shf_data;
int mdr_data;
int memory_data;
int sp_data;
int vector_data;
int va_data;
int ptbr_data;

int sext(int num, int width) {
    if (num & (1 << (width - 1))) {
        num |= ((0xFFFF << width) & 0xFFFF);
    }
    return num;
}

void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */

    ptbr_data = va_data = sp_data = vector_data = marmux_data = alu_data = shf_data = mdr_data = BUS = 0;
    NEXT_LATCHES = CURRENT_LATCHES;

    int IRD = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
    int COND = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
    int J = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
    int SJ = GetSJ(CURRENT_LATCHES.MICROINSTRUCTION);
    int VAT = GetVAT(CURRENT_LATCHES.MICROINSTRUCTION);
    int next;

    if (CYCLE_COUNT == 300) {
        NEXT_LATCHES.INT_ = 1;
        NEXT_LATCHES.INTV = 0x01;
    }

    if (IRD == 1) {
        /*uknown opcode*/
        next = (CURRENT_LATCHES.IR >> 12) & 0xF;
        if ((next == 10) || (next == 11)) {
            NEXT_LATCHES.EXCV = 0x05;
        }
    } else {
        int COND_IR11 = (COND & 0x1) && ((COND >> 1) & 0x1) && !((COND >> 2) & 0x1) && ((CURRENT_LATCHES.IR >> 11) & 0x1);
        int COND_R = (COND & 0x1) && !((COND >> 1) & 0x1) && !((COND >> 2) & 0x1) && (CURRENT_LATCHES.READY);
        int COND_BEN = !(COND & 0x1) && ((COND >> 1) & 0x1) && !((COND >> 2) & 0x1) && (CURRENT_LATCHES.BEN);
        int COND_INT = !(COND & 0x1) && !((COND >> 1) & 0x1) && ((COND >> 2) & 0x1) && (CURRENT_LATCHES.INT_);
        int COND_J = (COND_IR11) + (COND_R << 1) + (COND_BEN << 2) + (COND_INT << 3); 
        next = J | COND_J;
    }

    if (VAT) {
        /*Doing address translation, save return state in register*/
        NEXT_LATCHES.Saved_J = next;
        if (next == 23 || next == 24) {
            NEXT_LATCHES.is_W = 1;
        } else {
            NEXT_LATCHES.is_W = 0;
        }
        next = 51;
    }

    if (SJ) {
        /*Address translation done, go to appropriate return state*/
        next = CURRENT_LATCHES.Saved_J;
    }

    /*check for exception*/
    if (CURRENT_LATCHES.EXC) {
        next = 49; /*49 is the state that handles protection and unaligned exceptions*/
        NEXT_LATCHES.EXC = 0;
    }

    if (CURRENT_LATCHES.STATE_NUMBER == 47) {
        NEXT_LATCHES.INT_ = 0;
    }

    if (CURRENT_LATCHES.STATE_NUMBER == 8) {
        /*printf("Starting to return from the ISR around here\n");*/
    }

    NEXT_LATCHES.STATE_NUMBER = next;
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[next], sizeof(int)*CONTROL_STORE_BITS);

}


void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */

    if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        if (++num_mem_cycles == 4) {
            NEXT_LATCHES.READY = 1;
        }
    }

    if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1 && CURRENT_LATCHES.READY == 1) {
        NEXT_LATCHES.READY = 0;
        num_mem_cycles = 0;

        if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
            int addr;
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {/*byte*/
                addr = (CURRENT_LATCHES.MAR & 0xFFFE);/*MAR[15:1]'0*/ 
            } else {
                addr = CURRENT_LATCHES.MAR;
            }
            memory_data = Low16bits((MEMORY[addr/2][1] << 8) + MEMORY[addr/2][0]);
        }

        else if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            int addr = CURRENT_LATCHES.MAR;
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {/*byte*/
                if ((CURRENT_LATCHES.MAR & 0x1) == 0) {
                    MEMORY[addr/2][0] = CURRENT_LATCHES.MDR & 0xFF;
                } else {
                    MEMORY[addr/2][1] = (CURRENT_LATCHES.MDR >> 8) & 0xFF;
                }
            } else {
                MEMORY[addr/2][0] = CURRENT_LATCHES.MDR & 0xFF;
                MEMORY[addr/2][1] = (CURRENT_LATCHES.MDR >> 8) & 0xFF;
            }
        }
    }

}


int eval_SR1() {
    if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
        return (CURRENT_LATCHES.IR >> 9) & 0x7;
    } else if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        return (CURRENT_LATCHES.IR >> 6) & 0x7;
    } else {
        return 6;
    }
}

int eval_adder1() {
    int SR1;
    switch(GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
        case 0: 
        return CURRENT_LATCHES.PC;

        case 1: /* figure out SR1/BaseR */
        SR1 = eval_SR1();
        return CURRENT_LATCHES.REGS[SR1];
    }
}

int eval_adder2() {
    switch(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
        case 0:
        return 0;

        case 1:
        return Low16bits(sext((CURRENT_LATCHES.IR & 0x3F), 6));

        case 2:
        return Low16bits(sext((CURRENT_LATCHES.IR & 0x1FF), 9));

        case 3:
        return Low16bits(sext((CURRENT_LATCHES.IR & 0x7FF), 11));

    }
}

int eval_marmux() {
    if (GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION)) { /*select adder output*/
        int adder1, adder2;
        adder1 = eval_adder1();
        adder2 = eval_adder2();
        return Low16bits((adder1 + (adder2 << GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION))));
    } else { /* select LSHF(ZEXT[IR[7:0]],1) */
        return Low16bits(((CURRENT_LATCHES.IR & 0xFF) << 1));
    }
}

int eval_alu() {
    int SR1 = eval_SR1();
    int op1 = CURRENT_LATCHES.REGS[SR1];
    int SR2, op2;
    if (CURRENT_LATCHES.IR & 0x20) {
        op2 = sext((CURRENT_LATCHES.IR & 0x1F), 5);
    } else {
        SR2 = CURRENT_LATCHES.IR & 0x7;
        op2 = CURRENT_LATCHES.REGS[SR2];
    }

    switch(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION)) {
        case 0:
        return Low16bits(op1 + op2);

        case 1:
        return Low16bits(op1 & op2);

        case 2:
        return Low16bits(op1 ^ op2);

        case 3:
        return Low16bits(op1);
    }
}

int eval_shf() {
    int SR1 = eval_SR1();
    int op = CURRENT_LATCHES.REGS[SR1];
    int shiftAmt = CURRENT_LATCHES.IR & 0xF;
    int shiftType = (CURRENT_LATCHES.IR >> 4) & 0x3;

    switch(shiftType) {
        case 0: /* LSHF */
        return Low16bits(op << shiftAmt);

        case 1: /* RSHFL */
        return Low16bits(op >> shiftAmt);

        case 3: /* RSHFA */
        return Low16bits(sext(op >> shiftAmt, 16 - shiftAmt));
    }
}

int eval_mdr() {
    if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)) { /* word */
        return Low16bits(CURRENT_LATCHES.MDR);
    } else { /* byte */
        if (CURRENT_LATCHES.MAR & 0x1) {
            int val = (CURRENT_LATCHES.MDR >> 8) & 0xFF;
            return Low16bits(sext(val, 8));
        } else {
            int val = CURRENT_LATCHES.MDR & 0xFF;
            return Low16bits(sext(val, 8));
        }
    }
}

int eval_vector() {
    return Low16bits(0x0200 + CURRENT_LATCHES.Vector);
}

int eval_sp() {
    int SR1 = eval_SR1(); /*must update*/
    switch(GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
        case 0:
        return CURRENT_LATCHES.USP;

        case 1:
        return CURRENT_LATCHES.REGS[SR1] + 2;

        case 2:
        return CURRENT_LATCHES.REGS[SR1] - 2;

        case 3:
        return CURRENT_LATCHES.SSP;
    }
}

int eval_va() {
    /*This returns the physical address we want based on VA translation
    va_data[13:9] = MDR[13:9], va_data[8:0] = VA[8:0]
    */
    int mdr_bits = CURRENT_LATCHES.MDR & 0x3E00;
    int va_bits = CURRENT_LATCHES.VA & 0x1FF;
    return (mdr_bits | va_bits);
}

int eval_ptbr() {
    /*This returns the address of the PTE of the page containing the VA
    ptbr_data[15:8] = PTBR[15:8], ptbr_data[7:0] = LHSF(VA[15:9],1)
    */
    int ptbr_bits = CURRENT_LATCHES.PTBR & 0xFF00;
    int va_bits = (CURRENT_LATCHES.VA & 0xFE00) >> 8;
    return (ptbr_bits | va_bits);
}



void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */ 

    if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
        marmux_data = eval_marmux();
    } else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION)) {
        alu_data = eval_alu();
    } else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)) {
        shf_data = eval_shf();
    } else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        mdr_data = eval_mdr();
    } else if (GetGATE_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        vector_data = eval_vector();
    } else if (GetGATE_SP(CURRENT_LATCHES.MICROINSTRUCTION)) {
        sp_data = eval_sp();
    } else if (GetGATE_VA(CURRENT_LATCHES.MICROINSTRUCTION)) {
        va_data = eval_va();
    } else if (GetGATE_PTBR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        ptbr_data = eval_ptbr();
    }

}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */       

   if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = marmux_data;
    } else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = CURRENT_LATCHES.PC;
    } else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = alu_data;
    } else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = shf_data;
    } else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = mdr_data;
    } else if (GetGATE_DEC_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = CURRENT_LATCHES.PC - 2;
    } else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = CURRENT_LATCHES.PSR;
    } else if (GetGATE_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = vector_data;
    } else if (GetGATE_SP(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = sp_data;
    } else if (GetGATE_VA(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = va_data;
    } else if (GetGATE_PTBR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        BUS = ptbr_data;
    }

}


void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */     

    if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.MAR = Low16bits(BUS);

        /*check to see if we set EXC*/
        int MAR_LSB = NEXT_LATCHES.MAR & 0x1;
        /*int priv = (CURRENT_LATCHES.PSR >> 15) & 0x1;*/

        /*unaligned exception -- THIS NOW NEEDS TO BE CHECKED UNDER LD_VA*/
        if (MAR_LSB == 1 && GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            NEXT_LATCHES.EXCV = 0x03;
            NEXT_LATCHES.EXC = 1;
        }

        /*protection exception, takes priority over unaligned --- REMOVED FOR LAB 5, DONE ELSEWHERE*/
        /*if (priv && CYCLE_COUNT > 0 && NEXT_LATCHES.MAR >= 0x0000 && NEXT_LATCHES.MAR <= 0x2FFF && CURRENT_LATCHES.STATE_NUMBER != 15) {
            NEXT_LATCHES.EXCV = 0x02;
            NEXT_LATCHES.EXC = 1;
        }*/
    }

    if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        if (GetSET_MR(CURRENT_LATCHES.MICROINSTRUCTION)) {
            NEXT_LATCHES.MDR = (CURRENT_LATCHES.MDR | 0x1);
            if (CURRENT_LATCHES.is_W == 1) {
                NEXT_LATCHES.MDR = (CURRENT_LATCHES.MDR | 0x2);
            }
        } else if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)) {
            NEXT_LATCHES.MDR = Low16bits(memory_data);

            /*check for page fault and protection exceptions here*/
            if (GetHas_PTE(CURRENT_LATCHES.MICROINSTRUCTION)) {
                if (CURRENT_LATCHES.READY) {
                    int priv = (CURRENT_LATCHES.PSR >> 15) & 0x1;

                    /*if valid bit is 0, then page fault exception*/
                    if (!(NEXT_LATCHES.MDR & 0x04)) {
                        NEXT_LATCHES.EXCV = 0x02;
                        NEXT_LATCHES.EXC = 1;
                        /*printf("Seems like a page fault exception happened!\n");*/
                    }

                    /*if we're in priv = user mode and protection bit is 0, protection exception, takes priority over page fault*/
                    if (priv && !(NEXT_LATCHES.MDR & 0x8)) {
                        NEXT_LATCHES.EXCV = 0x04;
                        NEXT_LATCHES.EXC = 1;
                        /*printf("Seems like a protection exception happened!\n");*/
                    }
                }
            }
        } else {
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)) { /* word */
                NEXT_LATCHES.MDR = Low16bits(BUS);
            } else { /* byte */
                NEXT_LATCHES.MDR = Low16bits(((BUS & 0xFF) << 8) + (BUS & 0xFF));
            }
        }
    }
    if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.IR = Low16bits(BUS);
    }
    if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION)) {
        int n = (CURRENT_LATCHES.IR >> 11) & 0x1;
        int z = (CURRENT_LATCHES.IR >> 10) & 0x1;
        int p = (CURRENT_LATCHES.IR >> 9) & 0x1;
        NEXT_LATCHES.BEN = (CURRENT_LATCHES.N & n) | (CURRENT_LATCHES.Z & z) | (CURRENT_LATCHES.P & p);
    }
    if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION)) {
        int DR;
        if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            DR = 7;
        } else if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
            DR = (CURRENT_LATCHES.IR >> 9) & 0x7;
        } else {
            DR = 6;
        }
        NEXT_LATCHES.REGS[DR] = Low16bits(BUS);
    }
    if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION)) {
        if (GetCCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
            if (Low16bits(BUS) & 0x8000) {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            } else if (Low16bits(BUS) == 0) {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            } else {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            /*we also need to set the NZP bits of the PSR here*/
            int PSR_NZP = (NEXT_LATCHES.N << 2) | (NEXT_LATCHES.Z << 1) | NEXT_LATCHES.P;
            NEXT_LATCHES.PSR = Low16bits(CURRENT_LATCHES.PSR & 0xFFF8); /*clear the last 3 bits*/
            NEXT_LATCHES.PSR = Low16bits(NEXT_LATCHES.PSR | PSR_NZP);
        } else {
            NEXT_LATCHES.N = (BUS >> 2) & 0x1;
            NEXT_LATCHES.Z = (BUS >> 1) & 0x1;
            NEXT_LATCHES.P = BUS & 0x1;
            int PSR_NZP = (NEXT_LATCHES.N << 2) | (NEXT_LATCHES.Z << 1) | NEXT_LATCHES.P;
            NEXT_LATCHES.PSR = Low16bits(CURRENT_LATCHES.PSR & 0xFFF8); /*clear the last 3 bits*/
            NEXT_LATCHES.PSR = Low16bits(NEXT_LATCHES.PSR | PSR_NZP);
        }
    }
    if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
        int adder1, adder2;
        switch(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
            case 0:
            NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
            break;

            case 1:
            NEXT_LATCHES.PC = Low16bits(BUS);
            break;

            case 2:
            adder1 = eval_adder1();
            adder2 = eval_adder2();
            NEXT_LATCHES.PC = Low16bits((adder1 + (adder2 << GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION))));
            break;
        }
    }

    /*  GATE_DEC_PC,
    LD_PRIV,
    PRIVMUX,
    GATE_PSR,
    GATE_VECTOR,
    VECMUX,
    CCMUX,
    LD_USP,
    LD_SSP,
    SPMUX1, SPMUX0,
    GATE_SP,
    LD_VECTOR,

    int marmux_data;
    int alu_data;
    int shf_data;
    int mdr_data;
    int memory_data;
    int sp_data;
    int vector_data;
    */

    if (GetLD_PRIV(CURRENT_LATCHES.MICROINSTRUCTION)) {
        switch(GetPRIVMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
            case 0:
            NEXT_LATCHES.PSR = Low16bits(CURRENT_LATCHES.PSR & 0x7FFF);/*clear PSR[15]*/
            break;

            case 1:
            NEXT_LATCHES.PSR = Low16bits(BUS);/*if we're setting PSR[15] equal to BUS[15] then we're also setting PSR = BUS, so we can just do it all at once here*/
            break;
        }
    }

    if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION)) {
        int SR1 = eval_SR1();
        NEXT_LATCHES.USP = Low16bits(CURRENT_LATCHES.REGS[SR1]);
    }

    if (GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION)) {
        int SR1 = eval_SR1();
        NEXT_LATCHES.SSP = Low16bits(CURRENT_LATCHES.REGS[SR1]);
    }

    if (GetLD_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        switch(GetVECMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
            case 0:
            NEXT_LATCHES.Vector = Low16bits(CURRENT_LATCHES.EXCV << 1);
            break;

            case 1:
            NEXT_LATCHES.Vector = Low16bits(CURRENT_LATCHES.INTV << 1);
            break;
        }
    } 

    if (GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.VA = Low16bits(BUS);

        /*check to see if we set EXC*/
        int VA_LSB = NEXT_LATCHES.VA & 0x1;

        /*unaligned exception*/
        if (VA_LSB == 1 && GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            NEXT_LATCHES.EXCV = 0x03;
            NEXT_LATCHES.EXC = 1;
        }
    }
  

}

