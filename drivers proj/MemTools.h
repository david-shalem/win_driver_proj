#pragma once
#include "ProcTools.h"


#include <Storport.h>

/* ---- PE offsets ---- */

/* pe 0 -> nt offset -> section headers start -> (name, addres, size)
                     -> file header -> (section heaers amount) */

#define PE_SIZEOF_FIRST_HEADER 0x3c + sizeof(long)
#define PE_SIZEOF_NT_HEADER 264 
#define PE_NT_HEADERS_OFFSET_LOC 0x3c
#define PE_SECTION_HEADER_SIZE 40
#define PE_FILE_HEADER_LOC 0x04
#define PE_NUMBER_OF_SECTION_LOC 0x02 // !!! size -> 2 !!!

#define PE_SH_NAME_LOC 0x00
#define PE_SH_SIZE_LOC 0x08
#define PE_SH_OFFSET_LOC 0x0c

#define OFFSET_SIZE sizeof(DWORD32) 
#define IMAGE_SIZEOF_SHORT_NAME 8
/* ---- ---------- ---- */


#define GET_OFFSET_FROM_OFFSET(dst, locoffset) (memcpy(&dst, locoffset, sizeof(DWORD))) 
#define PHYSICAL_TO_ULONG64(physical) (StorPortConvertPhysicalAddressToULong64(physical))
#define READ_FROM_PHYSICAL(base, offset, size, buffer) (PHYSICAL_TO_ULONG64(base))
#define PHYSICAL_WITH_OFFSET(p, offset, new_p) (new_p.QuadPart = offset + p.QuadPart)

typedef struct SecInfo {
	unsigned int size;
	DWORD32 vaddres; 
} SecInfo;




/// <summary>
/// function get important and basic information about a process and keep it for future use
/// </summary>
/// <param name="procname"> the name of the dst process </param>
/// <returns> whether the initilzation secceded or not </returns>
bool initilizeProcessInfo(const char procname[]);


/// <summary>
/// function go through the list of section in the section headers list and find the start addres of a 
/// section with a specific name 
/// </summary>
/// <param name="secName"> the name of the dst section </param>
/// <returns> the start addres of the section in the process memory space and the size of the section </returns>
SecInfo getInfoOfSection(const char secName[]);

/// <summary>
/// read the section memory and search for possible locations of variable (using the var value as filter)
/// </summary>
/// <param name="secName"> the name of the dst section </param>
/// <param name="val"> the value of the dst variable </param>
/// <returns> array with all the possible addreses </returns>
darray searchThroughSection(const char secName[], int val);

/// <summary>
/// read the section memory and see if some of the possible addreses that was found in `searchThroughSection`
/// function has changed to new value (so it can be confirmed that this is actually the addres of the var) 
/// </summary>
/// <param name="secName"> the name of the section the search is in </param>
/// <param name="new_val"> the new value to search </param>
/// <param name="pre_addreses"> the possible addreses that was found in the searching function before </param>
/// <returns> the new possible locations </returns>
//darray searchThroughPreviesAddreses(const char secName[], int new_val, darray pre_addreses);

/// <summary>
/// function writes new value into the memroy of another process
/// </summary>
/// <param name="offset"> the location to write to </param>
/// <param name="new_val"> the data to write </param>
//void writeIntoMemory(int offset, int new_val);

/// <summary>
/// function lists all of the names of the sections in the dst process memory
/// </summary>
/// <param name="buffer"> the result buffer </param>
void listSections(char* buffer);

/// <summary>
/// function read memory from another process's addres space
/// </summary>
/// <param name="p"> the eprocess of the dst process </param>
/// <param name="offset"> the offset of the memory to start reading from </param>
/// <param name="base_addr"> the base addres of the dst process </param>
/// <param name="buffer"> the output buffer to copy the memory to </param>
/// <param name="size"> the amount of bytes to read </param>
bool readFromProcess(PEPROCESS p, long offset, PVOID base_addr, char* buffer, int size);

void checkMemoryReading();

short getNumberOfSection(); 