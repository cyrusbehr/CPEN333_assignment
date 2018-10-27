#include "Customer.h"

Customer::Customer() 
{
    m_name = generateName();
    m_cardNum = generateCCNum();
}

int Customer::main(void) {
    return 0;
}

void Customer::createPipeline(const std::string pipelineName) {
    // Connect to the pump pipeline
    m_pipelinePtr = std::make_unique<CPipe>(pipelineName, PIPE_SIZE);
}

std::string Customer::generateName() {
    // Randomly generate a customer name using our vector of predefined names
    int randNum = rand() % (namesVec.size()+ 1);
    return namesVec.at(randNum);
}

void Customer::pay(int amount) {
    m_money -= amount;
}

void Customer::purchaseGas() {
    // Ensure that the pipelinePtr and the pipelineSemaphore are not nullptr
    assert(m_pipelinePtr);
    assert(m_pipelineSemaphore);

    // Determine how much gas to purchase and which grade
    int liters = getRandNum(0, MAX_GAS_LITERS);
}

GasGrade Customer::generateGasGrade() {
    // We will make it 4 times as likely that they will get the cheapest grade
    int gradeNum = getRandNum(0, 6);
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