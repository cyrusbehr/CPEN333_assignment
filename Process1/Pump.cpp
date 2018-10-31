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
   
    // Draw the grid
    m_safePrint.drawHorizontalLine(3);
    m_safePrint.drawVerticalLine(m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1), 4);
    m_safePrint.drawHorizontalLine(9);
    while (true) {
        // Check if we have a customer in the queue, if so, make them the current customer and remove them from the outstanding queue
        if (m_customerVec.size()) {
            m_currentCustomer = m_customerVec.at(0);
            m_customerVec.erase(m_customerVec.begin());
            int refCount = 10;
            
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

            // TODO not sure if we actually require a semaphore in the read command, or if it will hand until we read it anyways

            // Read the information that the customer has sent from the pipeline
            CustomerPipelineData customerData;
            m_pipelinePtr->Read(&customerData);

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
            m_safePrint.sPrint("Name:   " + customerData.m_name, m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 4);
            m_safePrint.sPrint("Liters: " + litersStr, m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 5);
            m_safePrint.sPrint("Price:  " + priceStr, m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 6);
            m_safePrint.sPrint("Grade:  " + gradeToString(customerData.m_grade), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 7);
            m_safePrint.sPrint("CC Num: " + std::to_string(customerData.m_ccNum), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, 8);


            // Lock the semaphore before modifying the data
            // TODO also send the pump information
            m_consumerSemaphore.Wait();
            m_pumpStatusPtr->m_creditCardNum = customerData.m_ccNum;
            m_pumpStatusPtr->m_customerName = customerData.m_name;
            m_pumpStatusPtr->m_grade = customerData.m_grade;
            m_pumpStatusPtr->m_liters = customerData.m_liters;
            m_pumpStatusPtr->m_price = customerData.m_price;
            // Signal the producer semaphore
            m_producerSemaphore.Signal();
            //std::cout << "Pump " << m_pumpNum << " has updated pump status" << std::endl;

            // Wait for signal from attendant confirming that we can begin fueling
            m_signal.Wait();

            // Charge the customer
            m_currentCustomer->charge(customerData.m_price);

            // Begin the fueling process...
            float litersFilled = 0;
            //std::cout << "Dispensing fuel..." << std::endl;
            while (customerData.m_liters - litersFilled > 0) {
                //std::cout << "Liters: " << litersFilled << std::endl;
                litersFilled += 0.5;
                m_currentCustomer->dispenseGas(0.5);

                // Decrement the value in the fuel storage by 0.5
                m_fuelTankSemaphore.Wait();
                m_fuelTankStatusPtr->m_gasVec[m_pumpNum - 1] -= 0.5;
                m_fuelTankSemaphore.Signal();

                // Sleep for 1 seconds
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            //std::cout << "Finished!" << std::endl;

            // Delete the current customer
            m_currentCustomer->driveAway();
            delete m_currentCustomer;
            m_currentCustomer = nullptr;
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
    int refCount = 10;
    // Display the customers in queue
    m_safePrint.clearColumn(m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, refCount);
    for (const auto& customer : m_customerVec) {
        m_safePrint.sPrint(customer->getName(), m_safePrint.getColumnSize() / 4 * (m_pumpNum - 1) + 1, refCount++);
    }
}

std::string Pump::getSemaphoreName() {
    return m_pumpName + "Semaphore";
}

