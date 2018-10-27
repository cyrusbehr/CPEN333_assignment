#include "Pump.h"

Pump::Pump(const std::string pumpName, const int pumpNum, const std::string fuelTankDataPoolStr, const std::string pumpStatusDataPoolStr, const std::string producerSemaphoreName, const std::string consumerSemaphoreString)
    :m_pumpName(pumpName)
    , m_pumpNum(pumpNum)
    , m_fuelTankDataPoolStr(fuelTankDataPoolStr)
    , m_pumpStatusDataPoolStr(pumpStatusDataPoolStr)
    , m_producerSemaphore(producerSemaphoreName, 0, 1)
    , m_consumerSemaphore(consumerSemaphoreString, 1, 1)
{
    // Connect to the fuel tank data pool
    CDataPool fuelTankDataPool(m_fuelTankDataPoolStr, sizeof(FuelTankStatus));
    m_fuelTankStatusPtr = static_cast<FuelTankStatus*>(fuelTankDataPool.LinkDataPool());

    // Connect to pump status data pool
    CDataPool pumpDataPool(m_pumpStatusDataPoolStr, sizeof(PumpStatus));
    m_pumpStatusPtr = static_cast<PumpStatus*>(pumpDataPool.LinkDataPool());

    // Create the pump pipeline for communication with customers
    m_pipelinePtr = std::make_unique<CPipe>(getPipelineName(), PIPE_SIZE);
    
    // Create the mutex to protect the customer pipeline
    m_customerSemaphore = std::make_unique<CSemaphore>(getSemaphoreName(), 1, 1);

    // TODO create the producer consumer cemaphors
}

Pump::~Pump() {
    // Delete all the remaining customer objects
    for (auto customer : m_customerVec) {
        delete customer;
    }
}

int Pump::main(void) {
    // Check if we have a customer in the queue, if so, make them the current customer and remove them from the outstanding queue
    if (m_customerVec.size()) {
        m_currentCustomer = m_customerVec.at(0);
        m_customerVec.erase(m_customerVec.begin());
    }

    // If we have a customer at the pump...
    if (m_currentCustomer) {
        // Initialize the semaphore and pipeline on the customer end, display the prices to the customer
        m_currentCustomer->createSemaphore(getSemaphoreName());
        m_currentCustomer->createPipeline(getPipelineName());
        m_currentCustomer->setPrices(m_fuelTankStatusPtr->m_priceMap);

        // Trigger the customer to purchase gas
        m_currentCustomer->purchaseGas();

        // TODO not sure if we actually require a semaphore in the read command, or if it will hand until we read it anyways
        
        // Read the information that the customer has sent from the pipeline
        CustomerPipelineData customerData;
        m_pipelinePtr->Read(&customerData, sizeof(customerData));

        // Ensure that there is enough gas in the tank
        if (customerData.m_liters <= m_fuelTankStatusPtr->m_gasVec[m_pumpNum - 1]) {
            
        }

        // TODO send the transaction information to the gas station computer for them to approve

        // TODO once we receive the gas up signal, then we can charge the customer and update the pump display to show how much we are gassing up

        // Delete the current customer
        m_currentCustomer->driveAway();
        delete m_currentCustomer;
        m_currentCustomer = nullptr;
    }

    return 0;
}

std::string Pump::getPipelineName() {
    return m_pumpName + "Pipeline";
}

// Add a new customer to pump customer queue, pass unique_ptr by reference
void Pump::addCustomer(Customer* newCustomer) {
    m_customerVec.push_back(newCustomer);
}

std::string Pump::getSemaphoreName() {
    return m_pumpName + "Semaphore";
}