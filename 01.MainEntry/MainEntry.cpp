#include "../03.Library/Common.h"
#include "../03.Library/Console.h"
#include "../03.Library/Util.h"

#include "../03.Library/OStream.h"
#include "../03.Library/IStream.h"
#include "../03.Library/printf.h"

#include "../02.Hardware/HardwareSys.h"
#include "../02.Hardware/RTC.h"
#include "../04.Memory/MemoryMgr.h"

#include "../05.Task/TaskManager.h"

extern "C"		//This overrides the normal G++ name mangling, making sure we can call it cleanly from ASM
{
	void LoadConstructors();
	//void Main(uint esp, multibootInfo *mBootPtr);
	void Main(uint esp, multiboot_header_t* pBoot, multibootInfo *mbootPtr);
}

void LoadConstructors()
{
	extern int (*firstConstructor)();		//This function pointer is defined in the link script. It basically refers to the first global constructor
	extern int (*lastConstructor)();		//Same as the above, but refers to the end of the constructor list
	int (**constructor)();					//This represents a pointer to an individual constructor

	for(constructor = &firstConstructor; constructor < &lastConstructor; constructor++)	//Iterate from FirstConstructor to LastConstructor, incrementing every time
		(*constructor)();					//Dereference the pointer and invoke
}



Console OrangeConsole;
HardwareSys HardwareSystem;
Orange::TaskManager TaskMgr;


using std::cout; //so we only have to write cout.
using std::endl;
using std::cin;

void Main(uint esp, multiboot_header_t* pBoot, multibootInfo *mbootPtr)//The structure pointer is initially contained in EBX, and is passed to us by the bootloader
{
	disable();

	SetUpMemory();
	HardwareSystem.InitializeSystem();
	enable();

	TaskMgr.Initialize();

	OrangeConsole.Clear();

	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);
	printf("\nReal Time Clock Settings %x-%x-%x\n",stLocalTime.wDay,stLocalTime.wMonth,stLocalTime.wYear);



	char buffer[256];	//Max input from cin is 256 chars including '\0' char.

	cout << "Hello, what is your name? ";
	cin >> buffer;

	cout << "\nHi " << buffer <<"!\n Nice to meet you." << endl;
	cout << "i mean that ";
}
