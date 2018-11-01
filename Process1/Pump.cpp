#include "Pump.h"

Pump::Pump(SafePrint& safePrint, const std::string pumpName, const int pumpNum, const std::string fuelTankDataPoolStr, const std::string pumpStatusDataPoolStr, const std::string producerSemaphoreName, const std::string consumerSemaphoreString)
    : m_safePrint(safePrint)
    , m_pumpName(pumpName)
    , m_pumpNum(pumpNum)
    , m_fuelTankDataPoolStr(fuelTankDataPoolStr)
    , m_pumpStatusDataPoolStr(pumpStatusDataPoolStr)
    , m_producerSemaphore(producerSemaphoreName, 0, 1)
    , m_consumerSemaphore(consumerSemaphoreString, 1, 1)
    , m_signal(pumpName + "Signal", 0, 1)
    , m_clearSignal(pumpName + "ClearSignal", 0, 1)
    , m_fuelTankSemaphore(FUEL_TANK_SEMAPHORE_STR, 1, 1)
{
    // Seed the random function generator
    srand(static_cast<unsigned int>(time(NULL)));

    // Create the title for each pump, center within the container
    m_safePrint.sPrint(m_pumpName, safePrint.getColumnSize() / 8 - m_pumpName.length()/2 + safePrint.getColumnSize() / 4 * (m_pumpNum - 1), 2);
}

Pump::~Pump() {
    // Delete all the remaining customer objects
    for (auto customer : m_customerVec) {
        delete customer;
    }
}

int Pump::main(void) {
    // Connect to the fuel tank data pool
    CDataPool fuelTankDataPool(m_fuelTankDataPoolStr, sizeof(FuelTankStatus));
    m_fuelTankStatusPtr = static_cast<FuelTankStatus*>(fuelTankDataPool.LinkDataPool());

    // Connect to pump status data pool
    CDataPool pumpDataPool(m_pumpStatusDataPoolStr, sizeof(PumpStatus));
    m_pumpStatusPtr = static_cast<PumpStatus*>(pumpDataPool.LinkDataPool());

    // Create the pump pipeline for communication with customers
    m_pipelinePtr = std::make_unique<CTypedPipe<CustomerPipelineData>>(getPipelineName(), PIPE_SIZE);

    // Create the mutex to protect the customer pipeline
    m_customerSemaphore = std::make_unique<CSemaphore>(getSemaphoreName(), 1, 1);
   
    // Draw liters in each fuel tank
    m_fuelTankSemaphore.Wait();
    // Ternarary operator for debugging when launching Process1 without Assignmen1
    float gas = m_fuelTankStatusPtr->m_gasVec[0] ? m_fuelTankStatusPtr->m_gasVec[m_pumpNum - 1] : 400.f;
    m_fuelTankSemaphore.Signal();
    std::string gasString = "Liters: " + std::to_string(gas);
    m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1), 3);

    // Draw the grid
    m_safePrint.drawHorizontalLine(4);
    m_safePrint.drawVerticalLine(m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1), 5);
    m_safePrint.drawHorizontalLine(12);
    while (true) {
        // Check if we have a customer in the queue, if so, make them the current customer and remove them from the outstanding queue
        if (m_customerVec.size()) {
            m_currentCustomer = m_customerVec.at(0);
            m_customerVec.erase(m_customerVec.begin());
            int refCount = 13;
            
            // Update the customers in queue
            m_safePrint.clearColumn(m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, refCount);
            for (const auto& customer : m_customerVec) {
                m_safePrint.sPrint(customer->getName(), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, refCount++);
            }
        }

        // If we have a customer at the pump...
        if (m_currentCustomer) {
            //std::cout << "Pump " << m_pumpNum << " has a customer" << std::endl;
            // Initialize the semaphore and pipeline on the customer end, display the prices to the customer
            m_currentCustomer->createSemaphore(getSemaphoreName());
            m_currentCustomer->createPipeline(getPipelineName());

            m_fuelTankSemaphore.Wait();
            // Display the prices to the customer
            m_currentCustomer->setPrices(m_fuelTankStatusPtr->m_prices);
            m_fuelTankSemaphore.Signal();


            // Trigger the customer to purchase gas
            m_currentCustomer->purchaseGas();

            // Read the information that the customer has sent from the pipeline
            CustomerPipelineData customerData;
            m_customerSemaphore->Wait();
            m_pipelinePtr->Read(&customerData);
            m_customerSemaphore->Signal();

            // Convert price and liters from float to string with set precision
            std::ostringstream litersSS;
            litersSS << std::fixed;
            litersSS << std::setprecision(5);
            litersSS << customerData.m_liters;
            std::string litersStr = litersSS.str();

            std::ostringstream priceSS;
            priceSS << std::fixed;
            priceSS << std::setprecision(5);
            priceSS << customerData.m_price;
            std::string priceStr = priceSS.str();

            // Print out the customer credentials
            m_safePrint.sPrint("Name:   " + std::string(customerData.m_name, MAX_NAME_LENGTH), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 5);
            m_safePrint.sPrint("Liters: " + litersStr, m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 6);
            m_safePrint.sPrint("L Disp: " + std::to_string(m_currentCustomer->getLiters()), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 7);
            m_safePrint.sPrint("Price:  " + priceStr, m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 8);
            m_safePrint.sPrint("Grade:  " + gradeToString(customerData.m_grade), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 9);
            m_safePrint.sPrint("CC Num: " + std::to_string(customerData.m_ccNum), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 10);
            m_safePrint.sPrint("Status: " + m_currentCustomer->getStatus(), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 11);


            // Lock the semaphore before modifying the data
            m_consumerSemaphore.Wait();
            m_pumpStatusPtr->m_creditCardNum = customerData.m_ccNum;
            strcpy_s(m_pumpStatusPtr->m_customerNameC, MAX_NAME_LENGTH, customerData.m_name);
            m_pumpStatusPtr->m_grade = customerData.m_grade;
            m_pumpStatusPtr->m_liters = customerData.m_liters;
            m_pumpStatusPtr->m_price = customerData.m_price;
            // Signal the producer semaphore
            m_producerSemaphore.Signal();

            // Change the customer status to waiting
            m_currentCustomer->setStatus(Customer::Status::WAITING);
            m_safePrint.sPrint("Status: " + m_currentCustomer->getStatus(), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 11);

            // Wait for signal from attendant confirming that we can begin fueling
            m_signal.Wait();

            // Charge the customer
            m_currentCustomer->charge(customerData.m_price);

            // Change the customer status to Fueling
            m_currentCustomer->setStatus(Customer::Status::FUELLING);
            m_safePrint.sPrint("Status: " + m_currentCustomer->getStatus(), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 11);

            // Begin the fueling process...
            float litersFilled = 0;
            while (customerData.m_liters - litersFilled > 0) {
                litersFilled += 0.5;
                m_currentCustomer->dispenseGas(0.5);
                m_safePrint.sPrint("L Disp: " + std::to_string(m_currentCustomer->getLiters()), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 7);

                // Decrement the value in the fuel storage by 0.5
                m_fuelTankSemaphore.Wait();
                m_fuelTankStatusPtr->m_gasVec[m_pumpNum - 1] -= 0.5;
                auto gas = m_fuelTankStatusPtr->m_gasVec[m_pumpNum - 1];
                m_fuelTankSemaphore.Signal();
                std::string gasString = "Liters: " + std::to_string(gas);
                m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1), 3);

                // Sleep for 1 seconds
                std::this_thread::sleep_for(std::chrono::milliseconds(GAS_FILL_SLEEP));
            }

            // Delete the current customer
            m_currentCustomer->driveAway();
            delete m_currentCustomer;
            m_currentCustomer = nullptr;

            // Signal that the gasStationComputer should delete the customer
            m_clearSignal.Signal();

            // Clear the display
            m_safePrint.clearSection(m_pumpNum);
        }
    }
    return 0;
}

std::string Pump::gradeToString(GasGrade grade) {
    if (grade == GasGrade::G87) {
        return "G87";
    }
    else if (grade == GasGrade::G89) {
        return "G89";
    }
    else if (grade == GasGrade::G91) {
        return "G91";
    }
    else {
        return "G92";
    }
}


std::string Pump::getPipelineName() {
    return m_pumpName + "Pipeline";
}

// Add a new customer to pump customer queue, pass unique_ptr by reference
void Pump::addCustomer(Customer* newCustomer) {
    m_customerVec.push_back(newCustomer);
    int refCount = 13;
    // Display the customers in queue
    m_safePrint.clearColumn(m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, refCount);
    for (const auto& customer : m_customerVec) {
        m_safePrint.sPrint(customer->getName(), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, refCount++);
    }
}

std::string Pump::getSemaphoreName() {
    return m_pumpName + "Semaphore";
}

