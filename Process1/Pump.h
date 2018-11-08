#include "../IncludeFiles.h"
#include "Customer.h"
#include "../Shared.h"
#include "\RTExamples\rt.h"

#pragma once
#ifndef __Pump__
#define __Pump__

class Pump : public ActiveClass {
public:
    Pump(SafePrint& safePrint, const std::string pumpName, const int pumpNum, const std::string fuelTankDataPoolStr, const std::string pumpStatusDataPoolStr, const std::string producerSemaphoreName, const std::string consumerSemaphoreString);
    ~Pump();

    int main(void);
    void addCustomer(Customer* newCustomer);

    std::string getPipelineName();
    std::string getSemaphoreName();

private:
    std::string gradeToString(GasGrade grade);

    std::string m_pumpName;
    std::string m_fuelTankDataPoolStr;
    std::string m_pumpStatusDataPoolStr;
    
    int m_pumpNum = 0;
    FuelTankStatus* m_fuelTankStatusPtr = nullptr;
    PumpStatus* m_pumpStatusPtr = nullptr;
    Customer* m_currentCustomer = nullptr;
    SafePrint& m_safePrint;
    std::vector<Customer*> m_customerVec;
    std::unique_ptr<CTypedPipe<CustomerPipelineData>> m_pipelinePtr = nullptr;
    std::unique_ptr<CSemaphore> m_customerSemaphore = nullptr;
    
    CSemaphore m_producerSemaphore;
    CSemaphore m_consumerSemaphore;
    CSemaphore m_signal;
    CSemaphore m_fuelTankSemaphore;
    CSemaphore m_clearSignal;
};


#endif // !__Pump__
