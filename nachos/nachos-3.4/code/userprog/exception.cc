// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#define MAX 200
// increase program counter

void IncreasePC()
{
	int tmp = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, tmp);
	//increase
	tmp = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, tmp);

	machine->WriteRegister(NextPCReg, tmp + 4);	//A MIPS instruction is 32 bits or 4 bytes (always).
}

//int ReadInt()
void SysCall_ReadInt()
{
	//read number that user input
	//INPUT: None
	//OUTPUT: tra ve gia tri la so nguyen, neu khong phai so nguyen se tra ve 0
	char *buffer = new char[MAX + 1];
	int numbytes = gSynchConsole->Read(buffer, MAX+1);
	int cur_pos = 0;
	bool isNegative = false;
	bool isValid = true;
	bool dot = false;
	int firstIndex = 0;
	int result = 0;
	// kiem tra so am/so duong
	if (buffer[0] == '-')
	{
		isNegative = true;
		firstIndex++;
		cur_pos++;
	}
	else if (buffer[0] == '+')
	{
		firstIndex++;
		cur_pos++;
	}
	// kiem tra co phai la so nguyen hay khong?
	for (int i = cur_pos; i < numbytes; i++)
	{
		if (buffer[i] == '.')
		{
			dot = true;
			cur_pos = i + 1;
			break;
		}
	}

	if (dot)
	{
		for (int i = cur_pos; i < numbytes; i++)
		{
			if (buffer[i] != '0')
			{
				isValid = false;
				break;
			}
		}
	}
	if (!isValid)
	{
		printf("\n\n The number is invalid");
		DEBUG('a', "\n The number is invalid");
		machine->WriteRegister(2, 0);
	}
	else
	{
		for (int i = firstIndex; i < numbytes; i++)
		{
			if (buffer[i] == '.')
				break;
			result = result *10 + (int)(buffer[i] - 48);
		}

		machine->WriteRegister(2, result);
	}
	delete buffer;
}
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void
ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	switch (which)
	{
		case NoException:
			return;

		case PageFaultException:
			DEBUG('a', "\n No valid translation found");
			printf("\n\nNo valid translation found");
			interrupt->Halt();
			break;

		case ReadOnlyException:
			DEBUG('a', "\n Write attempted to page marked read-only");
			printf("\n\nWrite attempted to page marked read-only");
			interrupt->Halt();
			break;

		case BusErrorException:
			DEBUG('a', "\n Translation resulted invalid physical address");
			printf("\n\nTranslation resulted invalid physical address");
			interrupt->Halt();
			break;

		case AddressErrorException:
			DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
			printf("\n\nUnaligned reference or one that was beyond the end of the address space");
			interrupt->Halt();
			break;

		case OverflowException:
			DEBUG('a', "\nInteger overflow in add or sub.");
			printf("\n\nInteger overflow in add or sub.");
			interrupt->Halt();
			break;

		case IllegalInstrException:
			DEBUG('a', "\n Unimplemented or reserved instr.");
			printf("\n\nUnimplemented or reserved instr.");
			interrupt->Halt();
			break;

		case NumExceptionTypes:
			DEBUG('a', "\n Number exception types");
			printf("\n\nNumber exception types");
			interrupt->Halt();
			break;
		case SyscallException:
			switch (type)
			{
				case SC_Halt:
					DEBUG('a', "Shutdown, initiated by user program.\n");
					printf("\n\nShutdown, initiated by user program.\n");
					interrupt->Halt();
				case SC_ReadInt:
					SysCall_ReadInt();
					IncreasePC();
					break;
			}
			break;
		default:
			printf("Unexpected user mode exception %d %d\n", which, type);
			ASSERT(FALSE);
	}
}
