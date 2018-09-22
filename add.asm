; the user program
		.ORIG x3000

		LEA R0, LABEL2
		LDW R0, R0, #0		; R0 = xC000
		LEA R1, LABEL3
		LDW R1, R1, #0		; R1 = -20
		AND R5, R5, #0

		LDB R2, R0, #0		; R2 will hold the final value
		ADD R0, R0, #1		; R0 = xC001
		ADD R5, R5, #1		; R5 = 1
AGAIN	LDB R3, R0, #0
		ADD R2, R2, R3
		ADD R0, R0, #1
		ADD R5, R5, #1
		ADD R4, R5, R1
		BRnp AGAIN

		STW R2, R0, #0		; store result at xC014
		
		; JMP R2	

		HALT

LABEL2	.FILL xC000
LABEL3	.FILL #-20
		.END
