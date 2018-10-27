#pragma once
#include "..\rt.h"
#include "IncludeFiles.h"
#include "Shared.h"

#ifndef __GasStationComputer__
#define __GasStationComputer__

// Process
// Store a data structure which can be displayed
// Record of transactions
// Customers purchasing gas
// 5 child threads
// 4 to handle communication with each of the pumps (via datapools and semaphors)
// One to check status of fuel in each of the 4 fuel tanks
// Each thread can write to the output window

struct Transaction {
    float m_price = 0;
    float m_liters = 0;
    std::string m_customerName = "";
    Transaction(float price, float liters, std::string name)
        : m_price(price)
        , m_liters(liters)
        , m_customerName(name)
    {}
};

class GasStationComputer {
public:
    GasStationComputer();
    ~GasStationComputer();
private:
    int checkFuelTankStatus(void* args);
    int checkPumpStatus(void* args);

    std::vector<Transaction> m_transactions;

    // Threads
    std::unique_ptr<ClassThread<GasStationComputer>> m_fuelTankStatusThreadPtr = nullptr;
    std::unique_ptr<ClassThread<GasStationComputer>> m_pump1StatusThreadPtr = nullptr;
    std::unique_ptr<ClassThread<GasStationComputer>> m_pump2StatusThreadPtr = nullptr;
    std::unique_ptr<ClassThread<GasStationComputer>> m_pump3StatusThreadPtr = nullptr;
    std::unique_ptr<ClassThread<GasStationComputer>> m_pump4StatusThreadPtr = nullptr;

    // Data pool vars
    FuelTankStatus* m_fuelTankStatusPtr    = nullptr;

    struct PumpStatusPtrLock {
        PumpStatus* m_pumpStatus = nullptr;
        CSemaphore* m_pumpProducerLock = nullptr;
        CSemaphore* m_pumpConsumerLock = nullptr;
        CSemaphore* m_signal = nullptr;
    };

    // Pump Status Objects
    PumpStatusPtrLock m_pump1;
    PumpStatusPtrLock m_pump2;
    PumpStatusPtrLock m_pump3;
    PumpStatusPtrLock m_pump4;

    // FuelTank Semaphor
    CSemaphore m_fuelTankSemaphore;
};

#endif