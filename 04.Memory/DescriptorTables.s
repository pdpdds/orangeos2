[GLOBAL Processor_SetGDT]

[EXTERN irqHandler]
[EXTERN exceptionHandler]
[GLOBAL commonIRQHandler]

Processor_SetGDT:
	mov eax, [esp+4]
	lgdt [eax]			; Load the GDT
	
	mov ax, 0x10		; Give the CPU the new descriptor offsets
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:.codeFlush	; This sets the code descriptor offset to 0x8
.codeFlush:
	ret


%macro Exception_NoErrorCode 1
     [GLOBAL exception%1]
    exception%1
        cli
        push byte 0
        push byte %1
        jmp commonExceptionHandler
%endmacro

%macro Exception_ErrorCode 1
     [GLOBAL ISR%1]
    ISR%1:
        cli
        push byte %1
        jmp commonExceptionHandler
%endmacro

Exception_ErrorCode 0
Exception_ErrorCode 1
Exception_ErrorCode 2
Exception_ErrorCode 3
Exception_ErrorCode 4
Exception_ErrorCode 5
Exception_ErrorCode 6
Exception_ErrorCode 7
Exception_ErrorCode 8
Exception_ErrorCode 9
Exception_ErrorCode 10
Exception_ErrorCode 11
Exception_ErrorCode 12
Exception_ErrorCode 13
Exception_ErrorCode 14
Exception_ErrorCode 15
Exception_ErrorCode 16
Exception_ErrorCode 17
Exception_ErrorCode 18
Exception_ErrorCode 19
Exception_ErrorCode 20
Exception_ErrorCode 21
Exception_ErrorCode 22
Exception_ErrorCode 23
Exception_ErrorCode 24
Exception_ErrorCode 25
Exception_ErrorCode 26
Exception_ErrorCode 27
Exception_ErrorCode 28
Exception_ErrorCode 29
Exception_ErrorCode 30
Exception_ErrorCode 31
Exception_ErrorCode 32
Exception_ErrorCode 33

commonExceptionHandler:
    pusha            ; Pushes EDI, ESI, EBP, ESP, EBX, EDX, ECX and EAX

    mov ax, ds            ; Set AX to the current data segment descriptor
    push eax            ; Save the data segment descriptor on the stack

    mov ax, 0x10        ; Give the CPU the kernel¡¯s clean data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call exceptionHandler

    pop eax            ; Get the orginal data segment descriptor back
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Pops EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
    add esp, 8            ; Get rid of the pushed error code and interrupt vector
    sti                ; Re-enable interrupts
    iret            ; Tidy up the stack, ready for the next interrupt

%macro IRQ 2
  [GLOBAL IRQ%1]
  IRQ%1:
    cli
    push byte 0
    push byte %2
    jmp commonIRQHandler
%endmacro

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

commonIRQHandler:
    pusha            ; Pushes EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX

    mov ax, ds        ; Set AX to the current data segment descriptor
    push eax        ; Save the data segment descriptor on the stack

    mov ax, 0x10        ; Give the CPU the kernel¡¯s clean data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irqHandler

    pop ebx            ; Get the original data segment descriptor back
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa            ; Pops EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
    add esp, 8        ; Get rid of the pushed error code and interrupt vector
    sti                ; Re-enable interrupts
    iret            ; Tidy up the stack, ready for the next interrupt
