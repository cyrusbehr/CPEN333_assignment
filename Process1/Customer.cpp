#include "Customer.h"

Customer::Customer() 
{
    m_name = generateName();
    m_cardNum = generateCCNum();
    //std::cout << "Created new customer: " << m_name << std::endl;
}

int Customer::main(void) {
    return 0;
}

void Customer::createPipeline(const std::string pipelineName) {
    // Connect to the pump pipeline
    m_pipelinePtr = std::make_unique<CTypedPipe<CustomerPipelineData>>(pipelineName, PIPE_SIZE);
}

std::string Customer::generateName() {
    // Randomly generate a customer name using our vector of predefined names
    int randNum = rand() % (namesVec.size()+ 1);
    return namesVec.at(randNum);
}

void Customer::charge(float amount) {
    m_money -= amount;
}

void Customer::setPrices(GasPrice prices) {
    m_prices = prices;
}

std::string Customer::getName() {
    return m_name;
}

int Customer::getCCNumber() {
    return m_cardNum;
}

void Customer::swipeCreditCard(CustomerPipelineData& data, const float pricePerLiter) {
    // Really don't need a function for this, but doing it for the sake of demonstration...
    data.m_price = data.m_liters * pricePerLiter;
}

void Customer::purchaseGas() {
    srand(time(NULL));
    // Ensure that the pipelinePtr and the pipelineSemaphore are not nullptr
    assert(m_pipelinePtr);
    assert(m_pipelineSemaphore);

    // Determine how much gas to purchase and which grade
    CustomerPipelineData pipelineData;
    pipelineData.m_liters = static_cast<float>(rand() % (MAX_GAS_LITERS + 1));
    pipelineData.m_grade = generateGasGrade();
    strcpy_s(pipelineData.m_name, MAX_NAME_LENGTH, getName().c_str());
    pipelineData.m_ccNum = getCCNumber();

    // Check that we have enough money, if not, reduce liters until it works out
    float pricePerLiter;
    switch (pipelineData.m_grade)
    {
    case GasGrade::G87:
        pricePerLiter = m_prices.m_g87Price;
        break;
    case GasGrade::G89:
        pricePerLiter = m_prices.m_g89Price;
        break;
    case GasGrade::G91:
        pricePerLiter = m_prices.m_g91Price;
        break;
    case GasGrade::G92:
        pricePerLiter = m_prices.m_g92Price;
        break;
    default:
        break;
    }
    while (pricePerLiter * pipelineData.m_liters > m_money) {
        pipelineData.m_liters--;
    }

    // Swipe credit card...
    swipeCreditCard(pipelineData, pricePerLiter);

    // Remove the gas hose...
    removeGasHose();

    // Lock the pipeline semaphore
    m_pipelineSemaphore->Wait(); 

    // Write data into pipeline
    m_pipelinePtr->Write(&pipelineData);

    // Unlock the pipeline semaphore
    m_pipelineSemaphore->Signal();
}

void Customer::setStatus(Status status) {
    m_state = status;
}

std::string Customer::getStatus() {
    if (m_state == Status::PURCHASING) {
        return "Purchasing...       "; 
    }
    else if (m_state == Status::FUELLING) {
        return "Fueling...          ";
    }
    else {
        return "Waiting for confirm";
    }
}

void Customer::dispenseGas(float amount) {
    m_liters += amount;
}

GasGrade Customer::generateGasGrade() {
    // We will make it 4 times as likely that they will get the cheapest grade
    int gradeNum = rand() % (7);
    if (gradeNum <= 3) {
        return GasGrade::G87;
    }
    else if (gradeNum == 4) {
        return GasGrade::G89;
    }
    else if (gradeNum == 5) {
        return GasGrade::G91;
    }
    else {
        return GasGrade::G92;
    }
}

int Customer::generateCCNum() {
    // Randomly generate a 8-digit credit card number
    int res = 0;
    for (int i = 0; i < 8; ++i) {
        res += static_cast<int>(rand() % (10) * pow(10, i));
    }
    return res;
}

void Customer::createSemaphore(const std::string semaphoreName) {
    m_pipelineSemaphore = std::make_unique<CSemaphore>(semaphoreName, 1, 1);
}

int Customer::generateGasAmount() {
    return rand() % (71);
}

