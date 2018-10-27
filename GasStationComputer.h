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
private:
    int displayFuelTankStatus(void* args);
    int displayPumpStatus(void* args);

    std::vector<Transaction> m_transactions;

    // Threads
    std::unique_ptr<ClassThread<GasStationComputer>> m_fuelTankStatusThreadPtr = nullptr;
    std::unique_ptr<ClassThread<GasStationComputer>> m_pump1StatusThreadPtr = nullptr;
    std::unique_ptr<ClassThread<GasStationComputer>> m_pump2StatusThreadPtr = nullptr;
    std::unique_ptr<ClassThread<GasStationComputer>> m_pump3StatusThreadPtr = nullptr;
    std::unique_ptr<ClassThread<GasStationComputer>> m_pump4StatusThreadPtr = nullptr;

    // Data pool vars
    FuelTankStatus* m_fuelTankStatusPtr    = nullptr;
    PumpStatus* m_pump1StatusPtr           = nullptr;
    PumpStatus* m_pump2StatusPtr           = nullptr;
    PumpStatus* m_pump3StatusPtr           = nullptr;
    PumpStatus* m_pump4StatusPtr           = nullptr;

    // Pump Semaphors
    CSemaphore m_pump1ConsumerLock;
    CSemaphore m_pump1ProducerLock;
    CSemaphore m_pump2ConsumerLock;
    CSemaphore m_pump2ProducerLock;
    CSemaphore m_pump3ConsumerLock;
    CSemaphore m_pump3ProducerLock;
    CSemaphore m_pump4ConsumerLock;
    CSemaphore m_pump4ProducerLock;
};

#endif