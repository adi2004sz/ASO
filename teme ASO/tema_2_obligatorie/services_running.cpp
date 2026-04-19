#include <windows.h>
#include <iostream>
#include <vector>

int main() {
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    
    if (!hSCM) {
        std::cerr << "Eroare la deschiderea SCM. Asigura-te ca rulezi ca Administrator." << std::endl;
        return 1;
    }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resumeHandle = 0;

    EnumServicesStatusEx(
        hSCM, 
        SC_ENUM_PROCESS_INFO, 
        SERVICE_WIN32,       
        SERVICE_ACTIVE,      
        NULL, 
        0, 
        &bytesNeeded, 
        &servicesReturned, 
        &resumeHandle, 
        NULL
    );

    if (GetLastError() != ERROR_MORE_DATA) {
        std::cerr << "Eroare la interogarea serviciilor." << std::endl;
        CloseServiceHandle(hSCM);
        return 1;
    }

    DWORD bufSize = bytesNeeded;
    std::vector<BYTE> buffer(bufSize);
    LPENUM_SERVICE_STATUS_PROCESS services = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(buffer.data());

    if (EnumServicesStatusEx(
            hSCM, 
            SC_ENUM_PROCESS_INFO, 
            SERVICE_WIN32, 
            SERVICE_ACTIVE,
            buffer.data(), 
            bufSize, 
            &bytesNeeded, 
            &servicesReturned, 
            &resumeHandle, 
            NULL)) 
    {
        std::cout << "--- Servicii Win32 care ruleaza in prezent (" << servicesReturned << " gasite) ---" << std::endl;
        
        for (DWORD i = 0; i < servicesReturned; ++i) {
            std::cout << "Nume: " << services[i].lpServiceName 
                      << " | Display Name: " << services[i].lpDisplayName << std::endl;
        }
    } else {
        std::cerr << "Eroare la preluarea listei de servicii." << std::endl;
    }

    CloseServiceHandle(hSCM);
    
    return 0;
}