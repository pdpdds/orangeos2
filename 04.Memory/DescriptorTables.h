#ifndef DESCRIPTORTABLES_H
#define DESCRIPTORTABLES_H

struct GDTEntry
{
	unsigned short LimitLow;	//The lower 16 bits of the limit.
	unsigned short BaseLow;		//The lower 16 bits of the base.
	unsigned char BaseMiddle;	//The middle 8 bits of the base.
	unsigned char Access;		//Access flags.
	unsigned char Granularity;
	unsigned char BaseHigh;		//The last 8 bits of the base.
} __attribute__((packed));

//To save definitions, we'll use this for both the GDT and IDT.
struct DescriptorPointer
{
	unsigned short Limit;		//Total size of the descriptor table.
	unsigned int Address;		//Pointer to the start of the descriptor table.
} __attribute__((packed));

#define INITIAL_GDT_ENTRIES     5       //if you change this also change the BITMAP setting operation
#define GDT_TOTAL_PAGES         17
#define IDT_TOTAL_PAGES         1
#define INITIAL_GDT_ENTRIES 5
#define KERNEL_TMP_VARIABLE_HOLD        (0x19000)               //here the the InitKernelMem structure is placed temporarily
#define KERNEL_TMP_DESCRIPTORPOINTER      (0x19100)               //here the the InitKernelMem structure is placed temporarily

#define KERNEL_BASE_ADDESS        (0x100000)               //Kernel Address determined by Grub
#define VASM_PAGE_SIZE          4096
#define VASM_PAGE_BITMAP_SIZE   32      //32 KB for 4GB virtual Address Space

#define PAGE_ALIGN(addr)    ((addr) & -VASM_PAGE_SIZE)
#define PAGE_ALIGN_UP(addr)    (((addr) + VASM_PAGE_SIZE - 1) & -VASM_PAGE_SIZE)

//#warning check the following and change it in the Kernel32.dll
//#define VASM_MEM_USER       0x00000004
//#define PAGE_READWRITE      0x00000002


#define MEM_COMMIT      1
#define MEM_RESERVE     2
#define MEM_RESET       4
#define MEM_TOP_DOWN    8
#define MEM_DECOMMIT    16
#define MEM_RELEASE     32


#define PAGE_TABLE_SIZE         1024*4
#define PAGE_DIRECTORY_SIZE     1024*4

/*
These are macros that used during the initialization of memory (Phase I).
*/
#define kp_RESERVED_START_PAGE          0
#define kp_RESERVED_TOTAL_PAGES         ( 0x100000 / VASM_PAGE_SIZE )
#define kp_RESERVED_ADDRESS             0                       // These are reserved 1 MB BIOS Memory

//////------------REMOVE THIS ONE-------
#define kp_K_TMP_MAP_StartPage 	        256
#define kp_K_TMP_MAP_TotalPages 	2
#define kp_K_TMP_MAP_Address 		0x100000                //Used in Phase II Initialization
/////------------------------------------

//#define kp_KERNEL_START_PAGE 	        0xC0000 		//Kernel Code, data - (3*256*1024)
#define kp_KERNEL_START_PAGE 	        256 		//Kernel Code, data - (3*256*1024)
#define kp_KERNEL_TOTAL_PAGES 	        ( (dwKernelSize / VASM_PAGE_SIZE ) + 1 )
#define kp_KERNEL_ADDRESS 	        0x100000

#define kp_KERNEL_STACK_START_PAGE 	( kp_KERNEL_START_PAGE+kp_KERNEL_TOTAL_PAGES ) 	        //Kernel stack
#define kp_KERNEL_STACK_TOTAL_PAGES 	5                      // 5 Pages (4096*5 enough ring 0 stack space)
#define kp_KERNEL_STACK_ADDRESS 	(kp_KERNEL_ADDRESS + (kp_KERNEL_TOTAL_PAGES * VASM_PAGE_SIZE)) //0x140000                 // predetermined stack address

#define kp_KERNEL_MEM_START 	        (kp_KERNEL_STACK_START_PAGE+kp_KERNEL_STACK_TOTAL_PAGES)
#define kp_KERNEL_MEM_TOTAL_PAGES	( ( (UINT32)lpWorkingMemoryTop - (UINT32)lpKernelWorkingMemory ) / VASM_PAGE_SIZE  + 1)
#define kp_KERNEL_MEM_ADDRESS           ((UINT32)lpKernelWorkingMemory)                         //physical address
#define kp_KERNEL_MEM_LINEAR_ADDRESS    ((UINT32)kp_KERNEL_MEM_START * VASM_PAGE_SIZE )              //Linear address

#define kp_TRANSLATION_PT_START 	((1024*1024)-1024)      //(kp_Kernel_Code_StartPage-1024)
#define kp_TRANSLATION_PT_TOTAL 	1024                    // 1 page table

#define kp_POINTER_TPT_START     	(kp_TRANSLATION_PT_START-1)
#define kp_POINTER_TPT_TOTAL     	1                       // 1 page

#define VASM_TPT    ((void *)((UINT32)kp_TRANSLATION_PT_START*VASM_PAGE_SIZE))
#define VASM_LPTPT  ((void *)((UINT32)(kp_POINTER_TPT_START)*VASM_PAGE_SIZE))


extern "C" void Processor_SetGDT(unsigned int gdt);

//A class which sets up the GDT.
class GDT
{
private:
	GDT();
	~GDT();
	static GDTEntry gdt[INITIAL_GDT_ENTRIES];
	static DescriptorPointer gdtPointer;
	static void setEntry(unsigned char id, unsigned short limitLow, unsigned short baseLow, unsigned short baseMiddle, unsigned char access,
		unsigned char granularity, unsigned char baseHigh);
public:
	static void SetupGDT();
};	
#endif
