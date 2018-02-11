        .ORIG x3000             ; 	
	LEA R0, TEN             ;This instruction will be loaded into memory location x3000
	LDW R1, R0, #0          ;
        AND R1, R1, x8          ;R1<-8
        ADD R1,R1, #-16         ;R1<-(-8)
        AND R2, R2, #0          ;
        ADD R2, R1, #0          ;
        NOT R2, R2              ;
        ADD R2, R2, #1          ;
        RSHFL R2, R2, #1        ;
START	ADD R1, R1, R1          ;
        ADD R2, R2, #-1         ;
	BRZ DONE                ;
	BR START                ;
				;blank line

DONE	AND R3, R3, #0
		STB R1, R3, #31 
	TRAP x25;		;The last executable instruction
TEN	.FILL x0000A;		;This is 10 in 2's comp, hexadecimal
	.END			;The pseudo-op, delimiting the source program
        
        