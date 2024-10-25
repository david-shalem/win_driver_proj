#include "MemTools.h"
#include "excp.h"


#define INT_SIZE 4
#define TONEOFMEMORY 1000

#define READ_FROM_PROCESS(offset, buffer, size) (readFromProcess(proc, offset, base_addres, buffer, size))
/* ------- dst process memory info -------*/
char process_name[NAME_SIZE] = "";
PVOID base_addres;
unsigned short number_of_sections;
DWORD32 sec_headers_start_loc;
PEPROCESS proc;
/* ------- ----------------------- -------*/

ULONG tmp_tag1 = 'tar ';
bool initilizeProcessInfo(const char procname[])
{
    // initilizing the name of the dst process
    strncpy(process_name, procname, NAME_SIZE);

    // initilize the dst process object
    proc = findProcessByName(procname, false);
    if (!proc)
        return false; 

    // initilizing the process base addres
    PVOID Pbase_addres = getEproccesMemberAddres(process_name, LOC_BASE_ADDRES);
    memcpy(&base_addres, Pbase_addres, sizeof(PVOID));
    
    // - get the nt offset and initilize the offset of the section header -

    // map the first header in the pe from the phyisical memory
    char buffer[PE_SIZEOF_FIRST_HEADER] = "";
    READ_FROM_PROCESS(PE_NT_HEADERS_OFFSET_LOC, buffer, sizeof(long));
    DWORD32 nt_header_offset;
    memcpy(&nt_header_offset, buffer, sizeof(long));
    sec_headers_start_loc = nt_header_offset + PE_SIZEOF_NT_HEADER;
  
    // - initilize the number of section headers in the pe -

    // read the number of sections
    READ_FROM_PROCESS(nt_header_offset + PE_FILE_HEADER_LOC + PE_NUMBER_OF_SECTION_LOC, 
        (char*) & number_of_sections, 2);

    DbgPrintEx(0, 0, "number of sections: %d\n", number_of_sections);
    return true;
}

SecInfo getInfoOfSection(const char secName[])
{
    SecInfo res = { 0, 0 };

    char buffer[PE_SECTION_HEADER_SIZE];
    char sec_name[IMAGE_SIZEOF_SHORT_NAME];

    for (int i = 0; i < number_of_sections; i++)
    {
        // read the section header
        if (!READ_FROM_PROCESS(sec_headers_start_loc + (i * PE_SECTION_HEADER_SIZE), buffer, PE_SECTION_HEADER_SIZE))
            break;
        
        // if this section header is about the wanted section ->
        strncpy(sec_name, buffer, IMAGE_SIZEOF_SHORT_NAME);
        if (!strcmp(sec_name, secName))
        {
            memcpy(&(res.size), buffer + PE_SH_SIZE_LOC, sizeof(DWORD32));
            memcpy(&(res.vaddres), buffer + PE_SH_OFFSET_LOC, OFFSET_SIZE);
            break;
        }
    }

    DbgPrintEx(0, 0, "secton info found: addres = %u, size = %u", res.vaddres, res.size);
    return res;
}

darray searchThroughSection(const char secName[], int val)
{
    dprint("0\n");
    // initilize the result array
    darray res; 
    arrInitilize(res);
    dprint("1\n");
    // get the information neccesery to read from the section 
    SecInfo sec_info = getInfoOfSection(secName);
    if (sec_info.vaddres == 0) {
        arrFree(res);
        res.a = nullptr;
        return res;
    }

    dprint("2\n");
    // init a buffer to contain the data in the seciton 
    char* buffer = (char*)ExAllocatePool2(NonPagedPoolExecute, sec_info.size, tmp_tag1++);
    dprint("3\n");
    if (!READ_FROM_PROCESS(sec_info.vaddres, buffer, /*sec_info.size*/ 120)) {
        arrFree(res);
        res.a = nullptr;
        return res;
    }
    dprint("4\n");
    int tmp_val = 0;
    for (unsigned int i = 0; i < sec_info.size; i += sizeof(int)) { // running through the memory of the section
        // copy the current 4 bytes to the tmp val
        memcpy(&tmp_val, buffer + i, sizeof(int));
        dprint("5\n");
        DbgPrintEx(0, 0, "current loc: %d | current val: %d\n", i, tmp_val);
        // if the val is the same as searched, save the current location 
        if (tmp_val == val) {
            dprint("\nfound a match!\n\n");
            arrInsert(res, i);
        }
        dprint("6\n");
    }

    return res;
}
/*
darray searchThroughPreviesAddreses(const char secName[], int new_val, darray pre_addreses)
{
    // initilize the result array
    darray res;
    arrInitilize(res);

    // get the information neccesery to read from the section 
    SecInfo sec_info = getInfoOfSection(secName);

    // get the phyisical addres of the section 
    PHYSICAL_ADDRESS section_physicial;
    //PHYSICAL_WITH_OFFSET(p_addres, (int)sec_info.vaddres, section_physicial);

    // map the section memory from the physicial memory
    PVOID section_pointer = MmMapIoSpace(section_physicial, sec_info.size, MmNonCached);

    int buffer = 0;

    for (int i = 0; i < pre_addreses.used; i++) { // going throug hall the locations in the saved locations
        // reading the val in the current location
        memcpy(&buffer, (char*)section_pointer + (int)(((char*)pre_addreses.a)[i]), sizeof(int));

        // if the val is the same as searched, save the current location 
        if (buffer == new_val)
            arrInsert(res, (int)(((char*)pre_addreses.a)[i]));
    }

    return res;
}*/

void listSections(char* buffer)
{
    // create buffer to hold the section headers
    char* tmp_buffer = (char*)ExAllocatePool2(POOL_FLAG_RAISE_ON_FAILURE | POOL_FLAG_NON_PAGED, number_of_sections * PE_SECTION_HEADER_SIZE, tmp_tag1++);
    
    // read the section headers
    READ_FROM_PROCESS(sec_headers_start_loc, tmp_buffer, number_of_sections * PE_SECTION_HEADER_SIZE); 
    
    // innitlize the section amount index in the out buffer
    memcpy(buffer, &number_of_sections, 1);

    dprint("\nsections: \n");
    for (int i = 0; i < number_of_sections; i++)
    {
        // read the section header
        char print_b[IMAGE_SIZEOF_SHORT_NAME] = "";
        strncpy(print_b, tmp_buffer + (i*PE_SECTION_HEADER_SIZE), IMAGE_SIZEOF_SHORT_NAME);
        dprint(print_b);

        // copy the section name to the res buffer
        strncpy(buffer + 1 + (IMAGE_SIZEOF_SHORT_NAME * i), print_b, IMAGE_SIZEOF_SHORT_NAME);
    }
    ExFreePool(tmp_buffer);
}

bool readFromProcess(PEPROCESS p, long offset, PVOID base_addr, char* buffer, int size)
{
    bool transfered = false;
    KAPC_STATE st;

    PVOID pointer = (PVOID)(offset + (char*)base_addr); // adding the offset to the base addres of the process
    DbgPrintEx(0, 0, "\n\nREADING FROM : %p\n\n", pointer);
    __try {
        KeStackAttachProcess(p, &st); // transfer into the dst process memory space
        dprint("a");
        transfered = true;
        memcpy(buffer, pointer, size); // read the wanted memory
        dprint("b");
        KeUnstackDetachProcess(&st); // transfer back into the driver's memory space
        dprint("c");
    }
    __except (handleExcp(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER) // in case something went wrong transfer back into the driver's memory space
    {
        if(transfered) 
            KeUnstackDetachProcess(&st);
        return false;
    }

    return true;
}

void checkMemoryReading()
{
    char name[] = "cmd.exe";
    PEPROCESS p = findProcessByName(name, false);
    PVOID PbaseAddres = getEproccesMemberAddres(name, LOC_BASE_ADDRES);
    PVOID baseAddres;
    memcpy(&baseAddres, PbaseAddres, sizeof(PVOID));
    char buffer[3] = "";
    DbgPrintEx(0, 0, "\n\ncheck base addres: %p\n\n", baseAddres);
    readFromProcess(p, 0, baseAddres, buffer, 2);
    buffer[2] = '\0';
    dprint("\n\nMEGIC: ");
    dprint(buffer);
    dprint("\n");
}

short getNumberOfSection()
{
    return number_of_sections;
}
