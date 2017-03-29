; Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F


   

    PRESERVE8
	AREA   GETCPSR, CODE, READONLY

	EXPORT GET_CPSR
	EXPORT SET_CPSR
	EXPORT	 nop

GET_CPSR
			; R0 points to destination string.
			; R1 points to source string.
			MRS	R0,CPSR
			BX lr ; Return.

SET_CPSR
			; R0 points to destination string.
			; R1 points to source string.
			MSR	CPSR_cxsf,R0

			BX lr ; Return.

nop
 		 MOV       R4,R0

loop
		  NOP
		  SUB       R4,R4,#0x00000001
		  CMP       R4,#0x00000000
		  BGT       loop
			
		  BX lr ; Return.


DisIRQ
 		 MSR     CPSR_c, #Mode_SVC
		 LDR	r1,=0x80
		 b 			DisInt
DisFIQ
		 ldr r1,=0x40
DisInt
		 mrs r0,CPSR
		 orr r0,r0,r1
		 msr CPSR_cxsf,r0
		 mrs r0,CPSR
		 ands r0,r0,r1
		 beq DisInt



	END