
;defines for the full feature uart
;VF BP 31.10.2007 - modified for POLARIS board.

STUART_BASE      EQU	0x40700000


ST_THR_OFFSET    EQU	0x0      
ST_RBR_OFFSET    EQU	0x0      
ST_DLL_OFFSET    EQU	0x0      
ST_IER_OFFSET    EQU	0x4      
ST_DLH_OFFSET    EQU	0x4      
ST_IIR_OFFSET    EQU	0x8      
ST_FCR_OFFSET    EQU	0x8      
ST_LCR_OFFSET    EQU	0xC      
ST_MCR_OFFSET    EQU	0x10     
ST_LSR_OFFSET    EQU	0x14     
ST_MSR_OFFSET    EQU	0x18     
ST_SPR_OFFSET    EQU	0x1C     
ST_ISR_OFFSET    EQU	0x20     
ST_FOR_OFFSET    EQU	0x24     
ST_ABR_OFFSET    EQU	0x28     
ST_ACR_OFFSET    EQU	0x2C     

;INCLUDE plus_specific/asm_defs.inc


	AREA 	INT_Code, CODE, READONLY, ALIGN=5
	
	EXPORT 	UartInit
	EXPORT 	PrintStr

;/***************************************************************************************/	
UartInit 
	
	ldr 	r0, =STUART_BASE
 
	; /* Disable UART and disable interrupts */
	ldr 	r1, =0x0
	str 	r1, [r0, #0x0c] ; /* (DLAB OFF) */
	str 	r1, [r0, #0x04] ; /* IER_DLH = 0x0 */
 
	; /* Set baud rate divisor (115200 baud => 0x08) (38400 baud => 0x18) */
	ldr 	r1, =0x80
	str	r1, [r0, #0x0c] ; /* (DLAB ON) */
	ldr 	r1, =0x08
	str 	r1, [r0]     	; /* THR_RBR_DLL = 0x08 */
	ldr 	r1, =0x0
	str 	r1, [r0, #0x04] ; /* IER_DLH = 0x0 */
 
	; /* Set communications parameters to 8,N,1 */
	ldr 	r1, =0x0
	str 	r1, [r0, #0x0c] ; /* (DLAB OFF) */
	ldr 	r1, =0x3
	str 	r1, [r0, #0x0c] ; /* LCR = 0x3 */
 
	; /* Clear and enable fifos */
	ldr 	r1, =0x6   		; /*7*/
	str 	r1, [r0, #0x08] ; /* IIR_FCR = 0x8 */
 
	; /* Set polled mode */
	ldr 	r1, =0x0
	str 	r1, [r0, #0x04] ; /* IER_DLH = 0x0 */
 
	; /* Set normal UART mode */
	ldr 	r1, =0x0
	str 	r1, [r0, #0x10] ; /* MCR = 0 */
 
 	; /* Enable UART */
	ldr 	r1, [r0, #0x04] ; /* r1 = IER_DLH                    */
	orr 	r1, r1, #0x40   ; /* Set the enable uart bit         */
	str 	r1, [r0, #0x04] ; /*                                 */
	ldr 	r1, [r0]        ; /* load from Data Holding register */
	
	mov 	pc,lr		


;/*******************************************************************************************/	
PrintStr
	
	ldr     r1,=STUART_BASE	; /* Get Fifo Uart Base					*/
	ldr     r2, [r1, #ST_LSR_OFFSET]
	ands    r2, r2, #0x40   ; /* mask all but bit 6, and set Z if result=0            */
	beq     PrintStr
	
	ldrb    r2, [r0]        ;/* load the first byte (agin, for now... really need another	*/
	strb    r2, [r1]		; /* transmit a byte						*/
	cmp     r2, #0         	; /* is it NULL?							*/
	moveq   pc,lr          	; /* if so, let's end now (search forward, this macro only)	*/
	
	add     r0, r0, #1      ;/* and increment the byte pointer				*/
	b       PrintStr       	; /* otherwise, keep looping (search backwards, this macro only)  */

	mov     pc, lr


	END