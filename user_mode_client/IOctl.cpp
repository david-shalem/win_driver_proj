#include "IOctl.h"

#define SHORT_NAME_SIZE 8
#define SHORT_SIZE 2
#define NAME_SIZE 15
#define MAX_PROC_AMOUNT 500
#define MAX_SEC_AMOUNT 100
#define MAX_LOCS_AMOUNT 100
#define BUFFER 300

IOctl::IOctl()
{
    // getting the device object
    device = CreateFile(L"\\\\.\\symbolic_link_device_log", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (device == INVALID_HANDLE_VALUE) {
        std::cerr << "FAILD TO OPEN DEVICE. EXITING... (error code: " << GetLastError() << ")";
        Sleep(2500);
        exit(0);
    }
}

IOctl::~IOctl()
{
    CloseHandle(device);
}

std::vector<std::string> IOctl::getProcesess() const
{
    std::vector<std::string> res; 
    
    std::string driver_res = reqProcList();

    // saperating the names and inserting them to the vector 
    while (driver_res.size() > 0)
    {
        res.push_back(driver_res.substr(0, driver_res.find("|")));
        driver_res = driver_res.substr(driver_res.find("|") + 1);
    }

    return res;
}

void IOctl::hideProcess(const std::string& proc_name) const
{
    DWORD tmp = 0;
    char name_buffer[NAME_SIZE] = "";
    memcpy(name_buffer, proc_name.c_str(), proc_name.size()); // coping the name to the input buffer

    // sending the hiding request to the driver device
    ioctlResHandle(DeviceIoControl(device, PROC_HIDE_CODE, name_buffer, NAME_SIZE,
        name_buffer, NAME_SIZE, &tmp, 0));

}

void IOctl::initProcInfo(const std::string& proc_name) const
{
    char name_buffer[NAME_SIZE] = "";
    memcpy(name_buffer, proc_name.c_str(), proc_name.size()); // coping the name to the input buffer

    // sending the initilizing request to the driver device
    ioctlResHandle(DeviceIoControl(device, PROC_INIT_INFO_CODE, name_buffer, NAME_SIZE, name_buffer, NAME_SIZE, nullptr, 0));
}

std::vector<std::string> IOctl::getSections() const
{
    std::vector<std::string> res;
    char names_buffer[SHORT_NAME_SIZE * MAX_SEC_AMOUNT] = "";

    // sending the listing request to the driver device
    ioctlResHandle(DeviceIoControl(device, PROC_LISTS_CODE, nullptr, 0, names_buffer,
        SHORT_NAME_SIZE * MAX_SEC_AMOUNT, nullptr, 0));

    int sec_amount = names_buffer[0];
    std::string tmp(names_buffer, sec_amount * SHORT_NAME_SIZE);
    for (int i = 0; i < sec_amount; i++)
    {
        res.push_back(tmp.substr(0, SHORT_NAME_SIZE));
        tmp = tmp.substr(SHORT_NAME_SIZE);
    }

    return res; 
}

std::vector<std::pair<std::string, std::vector<int>>> IOctl::searchInProcess(std::vector<std::string> sec_names, int dst_val) const
{
    char outbuffer[MAX_SEC_AMOUNT * (1 + MAX_LOCS_AMOUNT)] = "";
    char inbuffer[6 + MAX_SEC_AMOUNT * SHORT_NAME_SIZE] = "";
  
    // insert the search value and the amount of sections to the ioctl input buffer   
    short sec_amount = sec_names.size();
    memcpy(inbuffer, &dst_val, sizeof(int));
    memcpy(inbuffer + sizeof(int), &sec_amount, SHORT_SIZE);
    std::cout << "secamount: " << sec_amount;
    // insert the names of the buffers to the ioctl input buffer 
    try
    {
        int tc = 0;
        while (tc != sec_amount) {
            memcpy(inbuffer + sizeof(int) + SHORT_SIZE + (SHORT_NAME_SIZE * tc), sec_names[tc].c_str(), sec_names[tc].size());
            tc++;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "error " << e.what();
    }
    std::cout << "3\n";
    // sending the ioctl request
    ioctlResHandle(DeviceIoControl(device, PROC_SEARCH_CODE, inbuffer,
        sizeof(int) + SHORT_SIZE + (SHORT_NAME_SIZE * sec_amount), outbuffer,
        MAX_SEC_AMOUNT * (SHORT_NAME_SIZE + MAX_LOCS_AMOUNT), nullptr, 0));
    std::cout << "4\n";
    // converting the outbuffer to the wanted result form 
    std::vector<std::pair<std::string, std::vector<int>>> res;
    int counter = 0;
    for (int i = 0; i < sec_amount; i++) {

        int locs_amount = outbuffer[counter]; // get the number of locations that were found 
        counter++;

        if (!locs_amount) // none was found 
            continue;

        std::pair<std::string, std::vector<int>> tmp;
        tmp.first = sec_names[i]; // insert the name of the section
        for (int j = 0; j < locs_amount; j++) { // run throug the locs that were found in this section
            int loc = 0;
            memcpy(&loc, outbuffer + counter, sizeof(int));
            tmp.second.push_back(loc);
            counter += sizeof(int);
        }

        res.push_back(tmp);
    }
    return res;
}

std::string IOctl::reqProcList() const
{
    DWORD tmp = 0;
    char name_buffer[NAME_SIZE * MAX_PROC_AMOUNT] = "";

    // sending the listing request to the driver device
    ioctlResHandle(DeviceIoControl(device, PROC_LISTP_CODE, name_buffer, 0, 
                                name_buffer, NAME_SIZE * MAX_PROC_AMOUNT, &tmp, 0));

    return std::string(name_buffer);
}

void IOctl::ioctlResHandle(bool res) const
{
    if (res)
        std::cout << "\nOperation completed succesfully\n";
    else {
        std::cout << "\nOperation faild\n";

        char buffer[BUFFER] = "";
        DWORD error_code = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)buffer, BUFFER, NULL);

        std::cout << "error code: " << error_code << "\nerror message: ";
        puts(buffer);
        std::cout << std::endl;
    }
}
