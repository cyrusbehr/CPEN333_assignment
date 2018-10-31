#pragma once
#include "..\rt.h"
#include "IncludeFiles.h"
#include "Shared.h"
#include "SafePrint.h"

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

class GasStationComputer 
    : public ActiveClass
{
public:
    GasStationComputer(SafePrint& safePrint);
    ~GasStationComputer();
    int main(void);
private:
    int checkFuelTankStatus(void* args);
    int checkPumpStatus(void* args);

    std::vector<Transaction> m_transactions;

    // Data pool vars
    FuelTankStatus* m_fuelTankStatusPtr    = nullptr;

    struct PumpStatusPtrLock {
        PumpStatus* m_pumpStatus = nullptr;
        CSemaphore* m_pumpProducerLock = nullptr;
        CSemaphore* m_pumpConsumerLock = nullptr;
        CSemaphore* m_signal = nullptr;
        std::vector<Transaction> m_transactionVec;
        int m_pumpNum;
    };

    // Pump Status Objects
    PumpStatusPtrLock m_pump1;
    PumpStatusPtrLock m_pump2;
    PumpStatusPtrLock m_pump3;
    PumpStatusPtrLock m_pump4;

    // FuelTank Semaphore
    CSemaphore m_fuelTankSemaphore;

    // Reference to safePrint
    SafePrint& m_safePrint;
};

#endif