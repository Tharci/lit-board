//
// Created by tmarc on 14/07/2021.
//

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>


void ListProcesses()
{
    PROCESSENTRY32 pe32 = {0};
    HANDLE    hSnap;
    int       iDone;
    bool      processFound;

    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    pe32.dwSize = sizeof(PROCESSENTRY32);
    Process32First(hSnap,&pe32);     // Can throw away, never an actual app

    processFound = false;
    iDone = 1;

    while(iDone) {
        iDone = Process32Next(hSnap,&pe32);
        if (strcmp(pe32.szExeFile, "Discord.exe") == 0) {
            processFound = true;
            iDone = 0;
        }
    }

    if(!processFound) {
        std::cout << "Process not found\n";
    }
    else {
        std::cout << "Process found\n";
    }
}


#include "AppIntegration.h"
#include "LitBoardDriver.h"


lbd::comp::AppIntegration::AppIntegration() :
        Component([this] { asyncTaskCycle(); }, true) { };

lbd::comp::ComponentId lbd::comp::AppIntegration::getComponentId() const {
    return ComponentId::AppIntegration;
}

void lbd::comp::AppIntegration::asyncTaskCycle() {
    // auto& driver = LitBoardDriver::getInstance();
    // ListProcesses();
    cycleWait(5000);
}







