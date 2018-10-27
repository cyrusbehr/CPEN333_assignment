#include "../IncludeFiles.h"
#include "Customer.h"
#include "../Shared.h"
#include "..\..\rt.h"

#pragma once
#ifndef __Pump__
#define __Pump__

class Pump 
: public ActiveClass{
public:
    Pump(const std::string pumpName, const std::string fuelTankDataPoolStr, const std::string pumpStatusDataPoolStr);
    ~Pump();

    int main(void);
    void addCustomer(Customer* newCustomer);

    std::string getPipelineName();
    std::string getSemaphoreName();

private:
    std::string m_pumpName;
    std::string m_fuelTankDataPoolStr;
    std::string m_pumpStatusDataPoolStr;

    FuelTankStatus* m_fuelTankStatusPtr = nullptr;
    PumpStatus* m_pumpStatusPtr = nullptr;
    Customer* m_currentCustomer = nullptr;
    std::vector<Customer*> m_customerVec;
    std::unique_ptr<CPipe> m_pipelinePtr = nullptr;
    std::unique_ptr<CSemaphore> m_customerSemaphore = nullptr;
        
};


#endif // !__Pump__
