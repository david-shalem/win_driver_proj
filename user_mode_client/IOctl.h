#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>

#define PROC_LISTP_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, 0, FILE_WRITE_ACCESS) // list the process in the os
#define PROC_HIDE_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, 0, FILE_WRITE_ACCESS) // hide process from th os
#define PROC_SEARCH_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS) // search for var in the memory of a process 
#define PROC_RESEARCH_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS) // search again in previes places in the memory of another process
#define PROC_CHANGE_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, 0, FILE_WRITE_ACCESS) // changge the value of a var in another process
#define PROC_LISTS_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, 0, FILE_WRITE_ACCESS) // list the sections in the memory of another process
#define PROC_INIT_INFO_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, 0, FILE_WRITE_ACCESS) // initilize the info the system hold about a process

class IOctl
{

public:
	IOctl();
	~IOctl();

	// process manager operations
	std::vector<std::string> getProcesess() const;
	void hideProcess(const std::string& proc_name) const; 

	// memory manager operations
	void initProcInfo(const std::string& proc_name) const;
	std::vector<std::string> getSections() const;
	std::vector<std::pair<std::string, std::vector<int>>> searchInProcess(std::vector<std::string> sec_names, int dst_val) const;
	// TODO: std::vector<int> researchInProcess(std::string sec_name, std::vector<int> prev_locations, int new_val) const;
	// TODO: void changeValInProcess(std::string sec_name, int loc, int new_val) const;



private: 
	HANDLE device; 

	std::string reqProcList() const;
	void ioctlResHandle(bool res) const;

};

