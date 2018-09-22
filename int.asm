; interrupt service routine
; this traverses the entire page table and clears the reference bit of each PTE
; starting address is x1000 and there are 128 PTEs
; remember to push registers used here onto the stack
		.ORIG x1200
		ADD R6, R6, #-2
		STW R0, R6, #0	; push R0 
		ADD R6, R6, #-2
		STW R1, R6, #0	; push R1
		ADD R6, R6, #-2
		STW R2, R6, #0	; push R2
		ADD R6, R6, #-2
		STW R3, R6, #0	; push R3
		LEA R0, LABEL
		LDW R2, R0, #0	; R2 = #128
		LEA R0, LABEL3
		LDW R3, R0, #0	; R3 = xFFFE, mask to clear R bit of PTE
		LEA R0, LABEL2
		LDW R0, R0, #0	; R0 = x1000
AGAIN	LDW R1, R0, #0	; R1 = M[x1000], first PTE
		AND R1, R1, R3	; PTE & xFFFE, clears last bit which is R bit
		STW R1, R0, #0	; store the PTE back
		ADD R0, R0, #2	; R0 = x1002, and so on, for next PTE
		ADD R2, R2, #-1	; 127 PTEs left to do, and so on
		BRp AGAIN		; when R2 = 0, 0 PTEs left to do, so we can stop
		LDW R3, R6, #0
		ADD R6, R6, #2	; pop R3
		LDW R2, R6, #0
		ADD R6, R6, #2	; pop R2
		LDW R1, R6, #0
		ADD R6, R6, #2	; pop R1
		LDW R0, R6, #0
		ADD R6, R6, #2	; pop R0
		RTI
LABEL	.FILL #128
LABEL2	.FILL x1000
LABEL3	.FILL xFFFE
		.END