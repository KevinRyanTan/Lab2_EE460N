        .ORIG x3000             ; 	


	LEA R0, OK             ;This instruction will be loaded into memory location x3000        
        LDW R2, R0, #0          ;R2<-0x4000
		ADD R1, R1, #10
        STB R1, R2, #-1          ;
        ADD R3, R3, #2           ;
        ADD R4, R4, #3
		ADD R5, R5, #4
		RET
		ADD R6, R6, #5
	TRAP x25;		;The last executable instruction
TEN	.FILL x0000A		;This is 10 in 2's comp, hexadecimal
OK    .FILL x4000          ;
BIG     .FILL x4356            ;
SMALL   .FILL x0001            ;
	.END			;The pseudo-op, delimiting the source program
        
        