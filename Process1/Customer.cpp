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

int Customer::generateCCNum() {
    // Randomly generate a 8-digit credit card number
    int res = 0;
    for (int i = 0; i < 8; ++i) {
        res += static_cast<int>(rand() % (10) * pow(10, i));
    }
    return res;
}

int Customer::generateGasAmount() {
    return rand() % (71);
}