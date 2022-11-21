all: Entry MainEntry Hardware Library Memory Task DataStructure Kernel.bin 

KernelObject = Entry.o MainEntry.o HardwareSys.o rtc.o pit.o keyboardcontroller.o malloc.o Bitmap.o BitmapFunc.o DescriptorTables.o GDT.o IDT.o InterruptHandler.o MemoryFunc.o MemoryMgr.o PageFunc.o VirtualAlloc.o\
			   IO.o Common.o Console.o Util.o IStream.o OStream.o printf.o Heap.o\
			   Task.o TaskManager.o Thread.o TaskSchedule.o\
			   List.o
			
Entry:
	@echo 
	@echo ============== Build OrangeOS Kernel Entry ===============
	@echo 
	
	make -C 00.KernelEntry

	@echo 
	@echo =============== Build Complete ===============	
	@echo 
	@echo ============== Build CC Main Entry ===============
	@echo 
	
MainEntry:	
	make -C 01.MainEntry

	@echo 
	@echo =============== Build Complete ===============

	@echo 
	@echo ============== Build Hardware ===============
	@echo 
	
Hardware:
	make -C 02.Hardware
Library:	
	make -C 03.Library

	@echo 
	@echo =============== Build Complete ===============
	
	@echo ============== Build Memory ===============
	@echo 
	
Memory:
	make -C 04.Memory

	@echo 
	@echo =============== Build Complete ===============	
	
	@echo ============== Build Task ===============
	@echo 
	
Task:
	make -C 05.Task

	@echo 
	@echo =============== Build Complete ===============	
	
	@echo ============== Build DataStructure ===============
	@echo 
	
DataStructure:
	make -C 06.DataStructure

	@echo 
	@echo =============== Build Complete ===============		

	@echo 
	@echo =========== OrangeOS Kernel Construct Start ===========
	@echo 	

	cp 00.KernelEntry/Entry.o Entry.o
	cp 01.MainEntry/MainEntry.o MainEntry.o
	
	cp 02.Hardware/pit.o pit.o
	cp 02.Hardware/rtc.o rtc.o
	cp 02.Hardware/keyboardcontroller.o keyboardcontroller.o
	cp 02.Hardware/HardwareSys.o HardwareSys.o
	
	cp 03.Library/common.o common.o
	cp 03.Library/console.o console.o
	cp 03.Library/IStream.o IStream.o
	cp 03.Library/OStream.o OStream.o
	cp 03.Library/IO.o IO.o
	cp 03.Library/printf.o printf.o
	cp 03.Library/Util.o Util.o
	
	
	cp 04.Memory/Bitmap.o Bitmap.o
	cp 04.Memory/BitmapFunc.o BitmapFunc.o
	cp 04.Memory/GDT.o GDT.o
	cp 04.Memory/IDT.o IDT.o
	cp 04.Memory/InterruptHandler.o InterruptHandler.o
	cp 04.Memory/MemoryFunc.o MemoryFunc.o
	cp 04.Memory/MemoryMgr.o MemoryMgr.o
	cp 04.Memory/PageFunc.o PageFunc.o
	cp 04.Memory/VirtualAlloc.o VirtualAlloc.o
	cp 04.Memory/DescriptorTables.o DescriptorTables.o
	cp 04.Memory/Heap.o Heap.o
	cp 04.Memory/malloc.o malloc.o
	
	cp 05.Task/Task.o Task.o
	cp 05.Task/TaskManager.o TaskManager.o
	cp 05.Task/Thread.o Thread.o
	cp 05.Task/TaskSchedule.o TaskSchedule.o
	
	cp 06.DataStructure/List.o List.o
	
	@echo 
	@echo ============= All Build Complete =============
	@echo
	
Kernel.bin: $(KernelObject)
	i686-elf-ld -T Link.ld -o Kernel.bin $(KernelObject)
	
clean:
	make -C 00.KernelEntry clean
	make -C 01.MainEntry clean
	make -C 02.Hardware clean
	make -C 03.Library clean
	make -C 04.Memory clean
	make -C 05.Task clean
	make -C 06.DataStructure clean
	rm -f *.o
	rm -f kernel.bin
