#include "IncludeFiles.h"
#include "GasStationComputer.h"
#include "SafePrint.h"
// This is the entry point for the application

int main(void) {
    // Seed the random timer
    srand(time(NULL));

    // Create instance of safePrint class
    SafePrint safePrint;
    std::string headerStr = "GAS STATION PROCESS";
    safePrint.sPrint(headerStr, safePrint.getColumnSize() / 2 - headerStr.length() / 2, 0);
    
    // Build the grid
    safePrint.drawHorizontalLine(1);
    safePrint.drawHorizontalLine(4);
    safePrint.drawHorizontalLine(11);

    for (int i = 0; i < 4; ++i) {
        // Create the title for each pump, center within the 
        std::string pumpName ("Pump" + std::to_string(i));
        safePrint.sPrint(pumpName, safePrint.getColumnSize() / 8 - pumpName.length() / 2 + safePrint.getColumnSize() / 4 * (i), 2);
        safePrint.drawVerticalLine(safePrint.getColumnSize() / 4 * (i), 5, 11);

    }

    GasStationComputer gasStationComputer(safePrint);
    gasStationComputer.Resume();
    while (true);
    return 0;
}