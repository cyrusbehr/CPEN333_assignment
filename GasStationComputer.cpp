#include "GasStationComputer.h"

Transaction::Transaction() {
    // Sets the timestamp when the transaction object is initialized
    m_currentTime = std::chrono::system_clock::now();
}

GasStationComputer::GasStationComputer()
    :m_fuelTankSemaphore(FUEL_TANK_SEMAPHORE_STR, 1, 1)
{
    // Create the Fuel Tank data pool
    CDataPool fuelTankDataPool("FuelTankDataPool", sizeof(FuelTankStatus));
    m_fuelTankStatusPtr = static_cast<FuelTankStatus*>(fuelTankDataPool.LinkDataPool());

    // Set the initial quantity of gas for each of the 4 pumps
    for (int i = 0; i < 4; ++i) {
        m_fuelTankStatusPtr->m_gasVec.push_back(MAX_FUELTANK_CAPACITY);
    }

    // Create child thread to display Fuel Tank Data Pool data
    m_fuelTankStatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::checkFuelTankStatus, ACTIVE, nullptr));

    // Create 4 gas pump Data pools
    CDataPool pump1DataPool("Pump1DataPool", sizeof(PumpStatus));
    CDataPool pump2DataPool("Pump2DataPool", sizeof(PumpStatus));
    CDataPool pump3DataPool("Pump3DataPool", sizeof(PumpStatus));
    CDataPool pump4DataPool("Pump4DataPool", sizeof(PumpStatus));

    m_pump1.m_pumpStatus = static_cast<PumpStatus*>(pump1DataPool.LinkDataPool());
    m_pump2.m_pumpStatus = static_cast<PumpStatus*>(pump2DataPool.LinkDataPool());
    m_pump3.m_pumpStatus = static_cast<PumpStatus*>(pump3DataPool.LinkDataPool());
    m_pump4.m_pumpStatus = static_cast<PumpStatus*>(pump4DataPool.LinkDataPool());
    
    // Create Producer Consumer Semaphores
    m_pump1.m_pumpProducerLock = new CSemaphore(PUMP1_P_STR, 0, 1);
    m_pump1.m_pumpConsumerLock = new CSemaphore(PUMP1_C_STR, 1, 1);
    m_pump2.m_pumpProducerLock = new CSemaphore(PUMP2_P_STR, 0, 1);
    m_pump2.m_pumpConsumerLock = new CSemaphore(PUMP2_C_STR, 1, 1);
    m_pump3.m_pumpProducerLock = new CSemaphore(PUMP3_P_STR, 0, 1);
    m_pump3.m_pumpConsumerLock = new CSemaphore(PUMP3_C_STR, 1, 1);
    m_pump4.m_pumpProducerLock = new CSemaphore(PUMP4_P_STR, 0, 1);
    m_pump4.m_pumpConsumerLock = new CSemaphore(PUMP4_C_STR, 1, 1);
    m_pump1.m_signal = new CSemaphore("Pump1Signal", 0, 1);
    m_pump2.m_signal = new CSemaphore("Pump2Signal", 0, 1);
    m_pump3.m_signal = new CSemaphore("Pump3Signal", 0, 1);
    m_pump4.m_signal = new CSemaphore("Pump4Signal", 0, 1);

    // Create 4 child threads to read from Gas Pump Data Pool
    m_pump1StatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump1));
    m_pump2StatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump2));
    m_pump3StatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump3));
    m_pump4StatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump4));


    // TODO we need to launch the child process (pump main function)
    // TODO we need rendevous in all of our child threads!
}

GasStationComputer::~GasStationComputer() {
    delete m_pump1.m_pumpStatus;
    delete m_pump1.m_pumpConsumerLock;
    delete m_pump1.m_pumpProducerLock;
    delete m_pump1.m_signal;

    delete m_pump2.m_pumpStatus;
    delete m_pump2.m_pumpConsumerLock;
    delete m_pump2.m_pumpProducerLock;
    delete m_pump2.m_signal;

    delete m_pump3.m_pumpStatus;
    delete m_pump3.m_pumpConsumerLock;
    delete m_pump3.m_pumpProducerLock;
    delete m_pump3.m_signal;

    delete m_pump4.m_pumpStatus;
    delete m_pump4.m_pumpConsumerLock;
    delete m_pump4.m_pumpProducerLock;
    delete m_pump4.m_signal;

}

int GasStationComputer::checkFuelTankStatus(void* args) {
    // Thread Function
    while (true) {
        // If any of the gas tanks have less than 200 liters, they should flash red
        m_fuelTankSemaphore.Wait();
        if (m_fuelTankStatusPtr->m_gasVec[0] <= 200) {
            std::cout << "Gas tank 1: " << m_fuelTankStatusPtr->m_gasVec[0] << std::endl;
        }
        else {
            // Flash RED
        }

        if (m_fuelTankStatusPtr->m_gasVec[1] <= 200) {
            // Flash RED
        }
        else {
            std::cout << "Gas tank 2: " << m_fuelTankStatusPtr->m_gasVec[1] << std::endl;
        }

        if (m_fuelTankStatusPtr->m_gasVec[2] <= 200) {
            // Flash RED
        }
        else {
            std::cout << "Gas tank 3: " << m_fuelTankStatusPtr->m_gasVec[2] << std::endl;
        }

        if (m_fuelTankStatusPtr->m_gasVec[3] <= 200) {
            // FLash RED
        }
        else {
            std::cout << "Gas tank 4: " << m_fuelTankStatusPtr->m_gasVec[3] << std::endl;
        }

        m_fuelTankSemaphore.Signal();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}

int GasStationComputer::checkPumpStatus(void* args) {
    // Thread Function
    PumpStatusPtrLock* status = static_cast<PumpStatusPtrLock*>(args);
    auto& pStat = status->m_pumpStatus;
    while (true) {
        Transaction newTransaction;
        status->m_pumpProducerLock->Wait();
        // Get new transaction information and add it to a record of all transactions
        newTransaction.m_cardNum = pStat->m_creditCardNum;
        newTransaction.m_customerName = pStat->m_customerName;
        newTransaction.m_grade = pStat->m_grade;
        newTransaction.m_liters = pStat->m_liters;
        newTransaction.m_price = pStat->m_price;
        m_transactionVec.push_back(newTransaction);

        std::cout << "New Customer:" << std::endl;
        std::cout << pStat->m_customerName << std::endl;
        std::cout << pStat->m_liters << " Liters for " << pStat->m_price << std::endl;
        status->m_pumpConsumerLock->Signal();
    }
    return 0;
}
// TODO have a function whose only job is to read the input of the keyboard. if the user specifies the fill command, it sets a bool. This then sends a messaage back to the pump class which checks if there is a customer there. If so, then it fills the tanks. It should be within the listing function in the pump where we check if there is enough gas in the tank, deduct the price from the customer, and finally delete the customer
// should check if there is enough gas in the function which listens to the keyboard inputs