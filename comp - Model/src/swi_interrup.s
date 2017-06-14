;/*****************************************************************************/
;/* SWI.S: SWI Handler   for critical sectons                                                    */
;*******************************************************************/

T_Bit           EQU     0x20

                PRESERVE8                      ; 8-Byte aligned Stack
                AREA    SWI_Area, CODE, READONLY
                ARM

                EXPORT  T_SWI_Handler
				EXPORT  disable_irq
				EXPORT	enable_irq
T_SWI_Handler   

                STMFD   SP!, {R12, LR}         ; Store R12, LR
                MRS     R12, SPSR              ; Get SPSR
                STMFD   SP!, {R8, R12}         ; Store R8, SPSR
                TST     R12, #T_Bit            ; Check Thumb Bit
                LDRNEH  R12, [LR,#-2]          ; Thumb: Load Halfword
                BICNE   R12, R12, #0xFF00      ;        Extract SWI Number
                LDREQ   R12, [LR,#-4]          ; ARM:   Load Word
                BICEQ   R12, R12, #0xFF000000  ;        Extract SWI Number

		CMP     R12,#0xFE            			; disable IRQ implemented as __SWI 0xFE
                BEQ     disable_irq
                CMP     R12,#0xFF              ; enable IRQ implemented as __SWI 0xFF
                BEQ     enable_irq
  
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return
          
SWI_End

disable_irq
				
		nop;
		LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
		ORR     R12, R12, #0x80        ; Set IRQ flag to disable it
                MSR     SPSR_cxsf, R12         ; Set SPSR
		LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return
			  
enable_irq	
 		 nop;
		 LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
		 BIC     R12, R12, #0x80        ; Set IRQ flag to disable it
                 MSR     SPSR_cxsf, R12        ; Set SPSR
                 LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return
			   


                END
