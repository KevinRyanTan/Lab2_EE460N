        .ORIG x3000             ; 	
	LEA R0, TEN             ;This instruction will be loaded into memory location x3000
	LDW R1, R0, #0          ;R1<-10
        LEA R0, ADDR            ;
        LDW R2, R0, #0          ;R2<-0x4000
        STB R1, R2, #1          ;
        LEA R0,SMALL            ;
        LDW R1, R0, #-1         ;R1<- x4356
        STW R1, R0, #0          ; 
	TRAP x25;		;The last executable instruction
TEN	.FILL x0000A		;This is 10 in 2's comp, hexadecimal
ADDR    .FILL x4000            ;
BIG     .FILL x4356            ;
SMALL   .FILL x0001            ;
	.END			;The pseudo-op, delimiting the source program
        
        