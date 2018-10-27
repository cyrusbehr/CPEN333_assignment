#pragma once
#include "..\rt.h"
#include "IncludeFiles.h"
#include "Shared.h"

#ifndef __GasStationComputer__
#define __GasStationComputer__

struct Transaction {
    float m_price = 0;
    float m_liters = 0;
    GasGrade m_grade;
    int m_cardNum;
    std::string m_customerName = "";
    std::chrono::system_clock::time_point m_currentTime;
    Transaction();
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

    // FuelTank Semaphore
    CSemaphore m_fuelTankSemaphore;

    std::vector<Transaction> m_transactionVec;
};

#endif