#pragma once

// 1. ntddk -> 2. wdm.h


#include <Ntifs.h>
#include <ntddk.h>
#include "DArray.h"

#define dprint(msg) DbgPrintEx(0, 0, msg) 

#define PROC_LISTP_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, 0, FILE_WRITE_ACCESS) // list the process in the os
#define PROC_HIDE_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, 0, FILE_WRITE_ACCESS) // hide process from th os
#define PROC_SEARCH_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS) // search for var in the memory of a process 
#define PROC_RESEARCH_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS) // search again in previes places in the memory of another process
#define PROC_CHANGE_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, 0, FILE_WRITE_ACCESS) // changge the value of a var in another process
#define PROC_LISTS_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, 0, FILE_WRITE_ACCESS) // list the sections in the memory of another process
#define PROC_INIT_INFO_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, 0, FILE_WRITE_ACCESS) // initilize the info the system hold about a process

#define MAX_AMOUNT_OF_SECTIONS 20

/// <summary>
/// function transfer all the chars in a string to lower case letters
/// </summary>
/// <param name="dst"> the string to transform </param>
void lowercase(const char src[], char res[]);

/* ---- these three callbacks are called on connection, lost connection and cleanups with the usermode
        through the device ---- */
NTSTATUS deviceCleanUp(PDEVICE_OBJECT device_obj, PIRP irp);
NTSTATUS deviceClose(PDEVICE_OBJECT device_obj, PIRP irp);
NTSTATUS deviceCreate(PDEVICE_OBJECT device_obj, PIRP irp);

/// <summary>
/// callback that deals with the user-mode client requests
/// </summary>
/// <param name="device"> the device object the the client and the driver are comuunicating through </param>
/// <param name="irp"> the 'irp' package that contain the information about the request and the buffers
///  of the input and output data </param>
/// <returns> success status </returns>
NTSTATUS deviceCtrl(PDEVICE_OBJECT device_obj, PIRP irp);

VOID DriverUnload(PDRIVER_OBJECT driver_obj);