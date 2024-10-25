#include <iostream>
#include <Windows.h>
#include <winnt.h>

int main()
{
	int var = 4;
	const char t[] = "david";

	while (true)
	{
		std::cout << "var value: " << var << std::endl;
		Sleep(1000);
	}

	return 0;
}