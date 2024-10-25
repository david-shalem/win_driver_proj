#include "IOctl.h"


#define LIST_PROCESESS 1
#define HIDE_PROCESS 2
#define MEMORY_MESS_PROCESS 3

#define LIST_SECTIONS 1
#define SEARCH_VAR 2
#define RESEARCH_VAR 3
#define CHANGE_VAR_VAL 4

void handleMemoryOperations(IOctl* ctrl)
{
	int user_choice = 0;
	std::vector<std::pair<std::string, std::vector<int>>> process_last_locations;

	while (true)
	{
		std::cout << "\n\nChoose your action: \n\n1 - List all of the section in the process \n2 - search value of var in the process memory \n3 - research var from the previes search in the process memory \n4 - change value of var in the process memory \n -- to return to the main menu type anything else --\n";
		std::cin >> user_choice;
		switch (user_choice)
		{
		case LIST_SECTIONS: {

			for (std::string name : ctrl->getSections())
				std::cout << name << std::endl;

			break; 
		}
		case SEARCH_VAR: {
			std::vector<std::string> names;
			std::cout << "Enter the names of the sections you want to search in (type 'endq' to stop):\n";

			// collecting the names of the sections
			std::string tmp = "";
			while (true)
			{
				std::cin >> tmp;

				if (tmp == "endq" || tmp == "")
					break;

				names.push_back(tmp);
			}

			if (names.size() == 0)
				continue;

			int dst_val = 0;
			std::cout << "Enter the value you want to search the variable by: ";
			std::cin >> dst_val;

			// getting the locs from the driver
			process_last_locations = ctrl->searchInProcess(names, dst_val);

			// printig the results
			for (std::pair<std::string, std::vector<int>> sec : process_last_locations) {
				std::cout << sec.first << ": \n";
				for (int loc : sec.second)
					std::cout << loc << std::endl;
			}

			break;
		}
		case RESEARCH_VAR: {

			break;
		}
		case CHANGE_VAR_VAL	: {

			break;
		}
		default:
			return;
		}
	}
}

int main()
{
	IOctl ctrl; 

	int user_choice = 0; 

	while (true)
	{
		std::cout << "\n\nChoose your action: \n\n1 - List all of the running procesess in the system \n2 - Hide one of the running procesess from the system \n3 - get into the memory of a process in the system\n -- to exit type anything else --\n";
		std::cin >> user_choice;

		int counter = 0;

		switch (user_choice)
		{
		case LIST_PROCESESS:
			counter = 1;
			std::cout << "names:\n";
			for (std::string name : ctrl.getProcesess())
				std::cout << counter++ << " - " << name << std::endl;

			break;
		case HIDE_PROCESS: {
			std::string proc_name;

			std::cout << "Enter the name of the process you want to hide: ";
			std::cin >> proc_name;

			ctrl.hideProcess(proc_name);

			break;
		}
		case MEMORY_MESS_PROCESS: {
			std::string proc_name;

			std::cout << "Enter the name of the process you want to get into: ";
			std::cin >> proc_name;

			std::cout << "\ninitilizing neccesery information about \'" << proc_name << "\'...\n";
			Sleep(600);

			ctrl.initProcInfo(proc_name);
			Sleep(100);

			handleMemoryOperations(&ctrl);

			break;
		}
		default:
			std::cout << "\nBy By\n";
			Sleep(1000);
			ctrl.~IOctl();
			exit(0);
		}
	}
	return 0;
}