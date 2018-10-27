#include "Pump.h"

Pump::Pump(const std::string pumpName, const std::string fuelTankDataPoolStr, const std::string pumpStatusDataPoolStr)
    :m_pumpName(pumpName)
    , m_fuelTankDataPoolStr(fuelTankDataPoolStr)
    , m_pumpStatusDataPoolStr(pumpStatusDataPoolStr)
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