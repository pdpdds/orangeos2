%include "Common.inc"
[SECTION .text]
[BITS 32]

[GLOBAL DetectMemory]
[GLOBAL FindContinuousBitsInBitmap]
[GLOBAL SetBitsInBitmap]

DetectMemory:
    push edi
    mov edi, 2*1024*1024		; This will detect the amount of RAM present in the system the memory scan starts from 2MB since the minimum req is 2 MB
    .Detect:
	mov byte [ds:edi], 0x0f		; writing and comparing a bit pattern
	cmp byte [ds:edi], 0x0f
	jne .DetectDone
	mov byte [ds:edi], 0xf0		; writing and comparing a bit pattern
	cmp byte [ds:edi], 0xf0
	jne .DetectDone

	add edi,1024			; Next Kilobyte

	jmp .Detect
    .DetectDone:	                ; now the edi contains the amount of RAM installed in bytes
    mov eax, edi                        ; returning the result

    pop edi
    ret

;This function scans for a number of set bits with in limit of a bitmap
;Input :
;        8  Bitmap Address
;        12 ScanFor         - 0 Scan for Cleared, 1 scan for Set bits
;        16 StartIndex      - Starting Index
;        20 Size            - Number of bits to scan
;        24 Limit           - Scan length
;Output :
;        Address where the first bit found in the Bitmap
FindContinuousBitsInBitmap:
    prologue 20

    push ds
    push edi
    push ebx
    push ecx
    push edx

    xor ebx, ebx

    ;mov ax, DATA_SELECTOR
    ;mov ds, ax

    mov edi, [ebp+8]                    ; Bitmap Start Address
    mov eax, [ebp+16]	                ; starting index
    mov ecx, [ebp+24]                   ; limit
    sub ecx, [ebp+20]                   ; ecx = limit-size
    ;Note:------------*** Only works upto 512 MB due to bt instruction*******************
    .Loop:
        bt [ds:edi], eax		; check the bit
        setc bl                         ; store the result in ebx
        cmp ebx, [ebp+12]               ; compare it with set/clear
        je .Found                       ; found set or clear bit
        inc eax				; not found goto next bit
        cmp eax,ecx        		; compare eax with limit-size
        jl .Loop
    jmp .NotFound

    .Found:   ;---eax contains the required page bit index
    mov edx, [ebp+20]                  ; size
    .FoundLoop:
        bt [ds:edi], eax               ; check the bit
        setc bl                        ; store the result in ebx
        cmp ebx, [ebp+12]              ; compare it with set/clear
        jne .GotoMain

        inc eax                        ; next bit
        dec edx                        ; decrement the size
        cmp edx,0
        jg .FoundLoop

    sub eax, [ebp+20]                  ; eax = first bit found
    jmp .Done

    .GotoMain:
        jmp .Loop

    .NotFound:
        xor eax, eax		       ; not found return 0
    .Done:

    pop edx
    pop ecx
    pop ebx
    pop edi
    pop ds

    epilogue
    ret
;This function sets or clears a no of bits specified
;Input : Bitmap Address
;        ClearOrSet - 0 Clear, 1 Set
;        StartIndex - Starting Index bit
;        Size       - Number of bits to be set or cleared
SetBitsInBitmap:
    prologue 16

    push ds
    push ecx
    push edi

    mov edi, [ebp+8]            ; Bitmap
    mov eax, [ebp+16]           ; starting index
    mov ecx, [ebp+20]           ; size
    .Loop:
        cmp dword[ebp+12], 1    ; set / clear
        je .SetBit
        btr [ds:edi], eax       ; clear the bit
        jmp .AfterSet
        .SetBit:
        bts [ds:edi], eax       ; set the bit

        .AfterSet:
        inc eax

        dec ecx
        cmp ecx, 0
        jg .Loop

    pop edi
    pop ecx
    pop ds

    epilogue

    ret



[SECTION .data]

[GLOBAL mm_PhysicalTotal]
[GLOBAL mm_PhysicalAvailable]

[GLOBAL mm_PhysicalBitmap]
[GLOBAL mm_PhysicalBitmapSize]

mm_PhysicalTotal	dd 0		; amount of RAM installed in the system in MB
mm_PhysicalAvailable   dd 0

mm_PhysicalBitmap      dd 0            ; pointer to the RAM Free Bitmap
mm_PhysicalBitmapSize  dw 0            ; size of the RAM Free Bitmap in Pages
