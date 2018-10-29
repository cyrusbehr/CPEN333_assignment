#include "IncludeFiles.h"
#include "GasStationComputer.h"
// This is the entry point for the application

int main(void) {
    srand(time(NULL));
    GasStationComputer gasStationComputer;
    gasStationComputer.Resume();
    while (true);
    return 0;
}