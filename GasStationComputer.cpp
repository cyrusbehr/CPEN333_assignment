#include "GasStationComputer.h"

Transaction::Transaction() {
    // Sets the timestamp when the transaction object is initialized
    m_currentTime = std::chrono::system_clock::now();
}


// MARK: Initializers

GasStationComputer::GasStationComputer(SafePrint& safePrint)
	:m_safePrint(safePrint)
{}

GasStationComputer::~GasStationComputer() {
	delete m_pump1.m_pumpStatus;
	delete m_pump1.m_pumpConsumerLock;
	delete m_pump1.m_pumpProducerLock;
	delete m_pump1.m_signal;
	delete m_pump1.m_clearSignal;

	delete m_pump2.m_pumpStatus;
	delete m_pump2.m_pumpConsumerLock;
	delete m_pump2.m_pumpProducerLock;
	delete m_pump2.m_signal;
	delete m_pump2.m_clearSignal;

	delete m_pump3.m_pumpStatus;
	delete m_pump3.m_pumpConsumerLock;
	delete m_pump3.m_pumpProducerLock;
	delete m_pump3.m_signal;
	delete m_pump3.m_clearSignal;

	delete m_pump4.m_pumpStatus;
	delete m_pump4.m_pumpConsumerLock;
	delete m_pump4.m_pumpProducerLock;
	delete m_pump4.m_signal;
	delete m_pump4.m_clearSignal;
}

int GasStationComputer::main(void) {
    // Create 4 gas pump datapools to hold the PumpStatus 
    CDataPool pump1DataPool("Pump1DataPool", sizeof(PumpStatus));
    CDataPool pump2DataPool("Pump2DataPool", sizeof(PumpStatus));
    CDataPool pump3DataPool("Pump3DataPool", sizeof(PumpStatus));
    CDataPool pump4DataPool("Pump4DataPool", sizeof(PumpStatus));

	// Create instance of fuel tank monitor 
	m_fuelTankMonitor = new FuelTankMonitor("FuelTankDataPool");

    // Set the initial quantity of gas for each of the 4 gas tanks
	m_fuelTankMonitor->initializeFuelTank();

	// Link pump object's status with the appropriate pump's datapool
    m_pump1.m_pumpStatus = static_cast<PumpStatus*>(pump1DataPool.LinkDataPool());
    m_pump2.m_pumpStatus = static_cast<PumpStatus*>(pump2DataPool.LinkDataPool());
    m_pump3.m_pumpStatus = static_cast<PumpStatus*>(pump3DataPool.LinkDataPool());
    m_pump4.m_pumpStatus = static_cast<PumpStatus*>(pump4DataPool.LinkDataPool());

    // Create producer-consumer semaphores for each of the pump objects
    m_pump1.m_pumpProducerLock = new CSemaphore(PUMP1_P_STR, 0, 1);
    m_pump1.m_pumpConsumerLock = new CSemaphore(PUMP1_C_STR, 1, 1);
	m_pump1.m_signal = new CSemaphore("Pump1Signal", 0, 1);
	m_pump1.m_clearSignal = new CSemaphore("Pump1ClearSignal", 0, 1);
	m_pump1.m_pumpNum = 1;

    m_pump2.m_pumpProducerLock = new CSemaphore(PUMP2_P_STR, 0, 1);
    m_pump2.m_pumpConsumerLock = new CSemaphore(PUMP2_C_STR, 1, 1);
	m_pump2.m_signal = new CSemaphore("Pump2Signal", 0, 1);
	m_pump2.m_clearSignal = new CSemaphore("Pump2ClearSignal", 0, 1);
	m_pump2.m_pumpNum = 2;

    m_pump3.m_pumpProducerLock = new CSemaphore(PUMP3_P_STR, 0, 1);
    m_pump3.m_pumpConsumerLock = new CSemaphore(PUMP3_C_STR, 1, 1);
	m_pump3.m_signal = new CSemaphore("Pump3Signal", 0, 1);
	m_pump3.m_clearSignal = new CSemaphore("Pump3ClearSignal", 0, 1);
	m_pump3.m_pumpNum = 3;

    m_pump4.m_pumpProducerLock = new CSemaphore(PUMP4_P_STR, 0, 1);
    m_pump4.m_pumpConsumerLock = new CSemaphore(PUMP4_C_STR, 1, 1);
	m_pump4.m_signal = new CSemaphore("Pump4Signal", 0, 1);
    m_pump4.m_clearSignal = new CSemaphore("Pump4ClearSignal", 0, 1);
	m_pump4.m_pumpNum = 4;

	// Create threads to continuously poll for the different datapool
    ClassThread<GasStationComputer> fuelTankStatusThread(this, &GasStationComputer::checkFuelTankStatus, ACTIVE, nullptr);
    ClassThread<GasStationComputer> pump1StatusThread(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump1);
    ClassThread<GasStationComputer> pump2StatusThread(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump2);
    ClassThread<GasStationComputer> pump3StatusThread(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump3);
    ClassThread<GasStationComputer> pump4StatusThread(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump4);
    
    // Create child thread to constantly read keyboard input from DOS Window
    ClassThread<GasStationComputer> keyboardInputThread(this, &GasStationComputer::readFromKeyboard, ACTIVE, nullptr);

    // Create child threads to wait on clear signal
    ClassThread<GasStationComputer> pump1ClearThread(this, &GasStationComputer::waitForClearSignal, ACTIVE, &m_pump1);
    ClassThread<GasStationComputer> pump2ClearThread(this, &GasStationComputer::waitForClearSignal, ACTIVE, &m_pump2);
    ClassThread<GasStationComputer> pump3ClearThread(this, &GasStationComputer::waitForClearSignal, ACTIVE, &m_pump3);
    ClassThread<GasStationComputer> pump4ClearThread(this, &GasStationComputer::waitForClearSignal, ACTIVE, &m_pump4);

    // Launch the child process
    CProcess p1("C:\\Users\\juneha\\Documents\\CPEN333_assignment\\Debug\\Process1.exe",
        NORMAL_PRIORITY_CLASS,
        OWN_WINDOW,
        ACTIVE
    );

	// Wait for the threads to complete 
    fuelTankStatusThread.WaitForThread();
    pump1StatusThread.WaitForThread();
    pump2StatusThread.WaitForThread();
    pump3StatusThread.WaitForThread();
    pump4StatusThread.WaitForThread();
    keyboardInputThread.WaitForThread();

    return 0;
}


// MARK: Helpers

// Waits for the clear signal by the customer to leave. Clears display for that pump section so that we can see next customer.
int GasStationComputer::waitForClearSignal(void* args) {
    while (true) {
        auto status = static_cast<PumpStatusPtrLock*>(args);
        status->m_clearSignal->Wait();
        m_safePrint.clearSection(status->m_pumpNum);
    }

    return 0;
}

// Checks the status of fuel in each of of the 4 fuel tanks. Flashes red if tank has less than 200 litres
int GasStationComputer::checkFuelTankStatus(void* args) {
    while (true) {
        // m_hasGas used to determine if there is enough gas to begin a gas up procedure
        // Block scope b/c colliding var name, easier than changing var names...

		m_fuelTankMonitor->wait();
        {
			m_hasGas1 = (m_fuelTankMonitor->getFuelQuantityForTank(0) > 200);
            std::string gasString = "Liters: " + std::to_string(m_fuelTankMonitor->getFuelQuantityForTank(0));
            m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * 0, 3, m_fuelTankMonitor->getFuelQuantityForTank(0) <= 200? Color::DARK_RED : Color::GREEN);
            TEXT_COLOUR();
        }
        {
            m_hasGas2 = (m_fuelTankMonitor->getFuelQuantityForTank(1) > 200);
            std::string gasString = "Liters: " + std::to_string(m_fuelTankMonitor->getFuelQuantityForTank(1));
            m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * 1, 3, m_fuelTankMonitor->getFuelQuantityForTank(1) <= 200 ? Color::DARK_RED : Color::GREEN);
            TEXT_COLOUR();
        }
        {
            m_hasGas3 = (m_fuelTankMonitor->getFuelQuantityForTank(2) > 200);
            std::string gasString = "Liters: " + std::to_string(m_fuelTankMonitor->getFuelQuantityForTank(2));
            m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * 2, 3, m_fuelTankMonitor->getFuelQuantityForTank(2) <= 200 ? Color::DARK_RED : Color::GREEN);
            TEXT_COLOUR();
        }
        {
            m_hasGas4 = (m_fuelTankMonitor->getFuelQuantityForTank(3) > 200);
            std::string gasString = "Liters: " + std::to_string(m_fuelTankMonitor->getFuelQuantityForTank(3));
            m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * 3, 3, m_fuelTankMonitor->getFuelQuantityForTank(3) <= 200 ? Color::DARK_RED : Color::GREEN);
            TEXT_COLOUR();
        }

		m_fuelTankMonitor->signal();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}

// Read commands from keyboard inputted by the gas station attendant 
int GasStationComputer::readFromKeyboard(void* args) {
    std::string msg = "Enter a command [1/2/3/4 Dispense gas to pump 1/2/3/4, 5/6/7/8 refill tank 5/6/7/8, print to display all transactions, d1/d2/d3/d4 to disable pumps, set87/set89/set91/set92 to set new price for grades]";
    m_safePrint.sPrint(msg, 0, 13, Color::DARK_YELLOW);

    while (true) {
        std::string cmd;
        std::cin >> cmd;

        if (cmd == "1") {
            // Check that we actually have enough gas in tank
            if (m_hasGas1 && m_isEnabledpump1) {
                m_pump1.m_signal->Signal();
                m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (0) + 1, 10);
            }
        }
        else if (cmd == "2") {
            // Check that we actually have enough gas in tank
            if (m_hasGas2  && m_isEnabledpump2) {
                m_pump2.m_signal->Signal();
                m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (1) + 1, 10);
            }
        }
        else if (cmd == "3") {
            // Check that we actually have enough gas in tank
            if (m_hasGas3  && m_isEnabledpump3) {
                m_pump3.m_signal->Signal();
                m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (2) + 1, 10);
            }
        }
        else if (cmd == "4") {
            // Check that we actually have enough gas in tank
            if (m_hasGas4  && m_isEnabledpump4) {
				m_pump4.m_signal->Signal();
				m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (3) + 1, 10);
            }
        }
         else if (cmd == "print") {
            int i = 0;
            int j = 0;
            for (const auto& trans : m_transactionVec) {
                std::time_t t = std::chrono::system_clock::to_time_t(trans.m_currentTime);
                char timeStr[MAX_NAME_LENGTH];
                ctime_s(timeStr, sizeof(timeStr), &t);
                m_safePrint.sPrint(std::to_string(++j) + ") Name:" + trans.m_customerName, 1, 15 + i++);
                m_safePrint.sPrint("CC: " + std::to_string(trans.m_cardNum), 4, 15 + i++);
                m_safePrint.sPrint("Time: " + std::string(timeStr, MAX_NAME_LENGTH), 4, 15 + i++);
                m_safePrint.sPrint("Grade: " + m_safePrint.gradeToString(trans.m_grade), 4, 15 + i++);
                m_safePrint.sPrint("Liters: " + std::to_string(trans.m_liters), 4, 15 + i++);
                m_safePrint.sPrint("Price: " + std::to_string(trans.m_price), 4, 15 + i++);
                m_safePrint.sPrint(" ", 4, 15 + i++);
            }
        }
         else if (cmd == "5") {
			m_fuelTankMonitor->refillFuelTank(0);
        }
         else if (cmd == "6") {
			m_fuelTankMonitor->refillFuelTank(1);
        }
         else if (cmd == "7") {
			m_fuelTankMonitor->refillFuelTank(2);
        }
         else if (cmd == "8") {
			m_fuelTankMonitor->refillFuelTank(3);
        }
         else if (cmd == "d1") {
            m_isEnabledpump1 = !m_isEnabledpump1;
        }
         else if (cmd == "d2") {
            m_isEnabledpump2 = !m_isEnabledpump2;
        }
         else if (cmd == "d3") {
            m_isEnabledpump3 = !m_isEnabledpump3;
        }
         else if (cmd == "d4") {
            m_isEnabledpump4 = !m_isEnabledpump4;
        }
         else if (cmd == "set87") {
            float newPrice;
            std::cin >> newPrice;
			m_fuelTankMonitor->setPriceForGasGrade(GasGrade::G87, newPrice);
        }
         else if (cmd == "set89") {
            float newPrice;
            std::cin >> newPrice;
			m_fuelTankMonitor->setPriceForGasGrade(GasGrade::G89, newPrice);
        }
         else if (cmd == "set91") {
            float newPrice;
            std::cin >> newPrice;
			m_fuelTankMonitor->setPriceForGasGrade(GasGrade::G91, newPrice);
        }
         else if (cmd == "set92") {
            float newPrice;
            std::cin >> newPrice;
			m_fuelTankMonitor->setPriceForGasGrade(GasGrade::G92, newPrice);
        }

        // Redraw the vertical grid lines in case we draw over it
        for (int i = 0; i < 4; ++i) {
            m_safePrint.drawVerticalLine(m_safePrint.getColumnSize() / 4 * (i), 5, 11, Color::MAGENTA);
        }
    }

    return 0;
}


// Checks the status of each of the pumps for new customer data
int GasStationComputer::checkPumpStatus(void* args) {
    PumpStatusPtrLock* status = static_cast<PumpStatusPtrLock*>(args);
    auto& pStat = status->m_pumpStatus;
    while (true) {
        Transaction newTransaction;

		// Wait for pump producer to be available for datapool consumption
        status->m_pumpProducerLock->Wait();

        // Get new transaction information and add it to a record of all transactions
        newTransaction.m_cardNum = pStat->m_creditCardNum;
        newTransaction.m_customerName = std::string(pStat->m_customerNameC, MAX_NAME_LENGTH);
        newTransaction.m_grade = pStat->m_grade;
        newTransaction.m_liters = pStat->m_liters;
        newTransaction.m_price = pStat->m_price;

        // Store transaction in overall transaction tracker, and tracker for each pump
        status->m_transactionVec.push_back(newTransaction);
        m_transactionVec.push_back(newTransaction);

		// Signal that the pump has consumed the new datapool data and that resource is free to use
        status->m_pumpConsumerLock->Signal();

		// Print out the new customer transaction at the pump
        m_safePrint.sPrint("Name:   " + newTransaction.m_customerName, m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 5);
        m_safePrint.sPrint("Liters: " + std::to_string(newTransaction.m_liters), m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 6);
        m_safePrint.sPrint("Price:  " + std::to_string(newTransaction.m_price), m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 7);
        m_safePrint.sPrint("Grade:  " + m_safePrint.gradeToString(newTransaction.m_grade), m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 8);
        m_safePrint.sPrint("CC Num: " + std::to_string(newTransaction.m_cardNum), m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 9);
        m_safePrint.sPrint("Status: Waiting for approval", m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 10);
    }
    return 0;
}
