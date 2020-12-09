#include "console_controller.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <utility>

#include "engine.hpp"
#include "transform.hpp"
#include "transmitter.hpp"
#include "receiver.hpp"

ConsoleController::ConsoleController(Engine * engine): engine_(engine) {

}

void ConsoleController::Run() {
    std::string command;
    bool is_run = true;
    while(is_run){
        std::cout << ">";
        std::cin >> command;
        switch(command[0]){
            case 'h': {
                PrintInstructions();
            }  break;
            case 't': {
                RunTransmitterMode();
            }break;
            case 'r':
                RunReceiverMode();
                break;
            case 'v':
                RunViewMode();
                break;
            case 'e': {
                is_run = false;
            }break;
        }
    }
}

void ConsoleController::ConsoleAddTransmitter() {
    Transform transform;
    float frequency;
    float tx_power;
    std::cout << "Add a transmitter" << std::endl;
    std::cout << "Enter a position (x y z): ";
    std::cin >> transform.position.x;
    std::cin >> transform.position.y;
    std::cin >> transform.position.z;
    std::cout << "Enter a orientation (theta phi): ";
    std::cin >> transform.position.x;
    std::cin >> transform.position.y;
    std::cout << "Transmitting Frequency [Hz] (eg. 2.4e9): ";
    std::cin >> frequency;
    std::cout << "Transmit Power [dBm]:";
    std::cin >> tx_power;

    // Create a transmitter
    Transmitter * tx = new Transmitter(transform, frequency, tx_power, engine_->GetRayTracer());
    // Add the transmitter to engine
    engine_->transmitters_.insert(std::make_pair(tx->GetID(), tx));
}

void ConsoleController::ConsoleAddReceiver() {

}

void ConsoleController::PrintInstructions()
{
    std::cout << "----------------------------------------------------------\n";
    std::cout << "                    Global Console Commands\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << std::setw(15) << "Commands" << std::setw(25) << "Functions" << std::endl;
    std::cout << std::setw(15) << "h" << std::setw(25) << "Display console commands." << std::endl;
    std::cout << std::setw(15) << "t" << std::setw(25) << "Enter transmitter mode." << std::endl;
    std::cout << std::setw(15) << "r" << std::setw(25) << "Enter receiver Mode." << std::endl;
    std::cout << std::setw(15) << "v" << std::setw(25) << "Enter view mode." << std::endl;
    std::cout << std::setw(15) << "e" << std::setw(25) << "Exit the program." << std::endl;

    if (engine_->engine_mode_ == EngineMode::kView) {
        PrintViewModeInstructions();
    } else if (engine_->engine_mode_ == EngineMode::kTransmitter){
        PrintTransmitterModeInstructions();
    } else if (engine_->engine_mode_ == EngineMode::kReceiver){
        PrintReceiverModeInstructions();
    }

}

void ConsoleController::PrintViewModeInstructions(){
    std::cout << "----------------------------------------------------------\n";
    std::cout << "                   View Mode (Current Mode)\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << std::setw(15) << "Key" << std::setw(25) << "Command" << std::endl;
    std::cout << "----------------------------------------------------------\n";
    std::cout << std::setw(15) << "W" << std::setw(25) << "Move Forward" << std::endl;
    std::cout << std::setw(15) << "S" << std::setw(25) << "Move Backward" << std::endl;
    std::cout << std::setw(15) << "A" << std::setw(25) << "Move Left" << std::endl;
    std::cout << std::setw(15) << "D" << std::setw(25) << "Move Right" << std::endl;
    std::cout << std::setw(15) << "E" << std::setw(25) << "Rotate Right" << std::endl;
    std::cout << std::setw(15) << "Q" << std::setw(25) << "Rotate Left" << std::endl;
    std::cout << std::setw(15) << "LShift" << std::setw(25) << "Move Upward" << std::endl;
    std::cout << std::setw(15) << "CTRL" << std::setw(25) << "Move Downward" << std::endl;
    std::cout << std::setw(15) << "Z" << std::setw(25) << "Rotate Upward" << std::endl;
    std::cout << std::setw(15) << "X" << std::setw(25) << "Rotate Downward" << std::endl;
    std::cout << std::setw(15) << "Scroll Up" << std::setw(25) << "increase camera speed" << std::endl;
    std::cout << std::setw(15) << "Scroll Down" << std::setw(25) << "decrease camera speed" << std::endl;
    std::cout << std::setw(15) << "ESC" << std::setw(25) << "Exit" << std::endl;
}


void ConsoleController::PrintTransmitterModeInstructions() {
    std::cout << "----------------------------------------------------------\n";
    std::cout << "                   Transmitter Mode (Current Mode)\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << std::setw(15) << "Key" << std::setw(25) << "Command" << std::endl;
    std::cout << "----------------------------------------------------------\n";
    std::cout << std::setw(15) << "W" << std::setw(25) << "Move Forward" << std::endl;
    std::cout << std::setw(15) << "S" << std::setw(25) << "Move Backward" << std::endl;
    std::cout << std::setw(15) << "A" << std::setw(25) << "Move Left" << std::endl;
    std::cout << std::setw(15) << "D" << std::setw(25) << "Move Right" << std::endl;
    std::cout << std::setw(15) << "E" << std::setw(25) << "Rotate Right" << std::endl;
    std::cout << std::setw(15) << "Q" << std::setw(25) << "Rotate Left" << std::endl;

}

void ConsoleController::PrintReceiverModeInstructions() {
    std::cout << "----------------------------------------------------------\n";
    std::cout << "                   Receiver Mode (Current Mode)\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << std::setw(15) << "Key" << std::setw(25) << "Command" << std::endl;
    std::cout << "----------------------------------------------------------\n";
    std::cout << std::setw(15) << "W" << std::setw(25) << "Move Forward" << std::endl;
    std::cout << std::setw(15) << "S" << std::setw(25) << "Move Backward" << std::endl;
    std::cout << std::setw(15) << "A" << std::setw(25) << "Move Left" << std::endl;
    std::cout << std::setw(15) << "D" << std::setw(25) << "Move Right" << std::endl;
    std::cout << std::setw(15) << "E" << std::setw(25) << "Rotate Right" << std::endl;
    std::cout << std::setw(15) << "Q" << std::setw(25) << "Rotate Left" << std::endl;
}

void ConsoleController::RunTransmitterMode() {
    std::string command;
    while(true){
    std::cin >> command;
    switch(command[0]){
        case'a':{
            // Feature 1: Add Transmitter
            ConsoleAddTransmitter();
        }break;
        case's':{
            // Feature 2: Select Transmitter
            ConsoleSelectReceiver();
        }break;
        case'r':{
            ConsoleRemoveReceiver();
        }break;
        case'e':{
            return;
        }
    }
    }


    // Feature 3: Remove Transmitter
}

void ConsoleController::RunReceiverMode(){
    std::cout << "Entered to Receiver Mode" << std::endl;
    std::string command;

    while(true){
        std::cout << "rx >";
        std::cin >> command;
        switch(command[0]){
            case 'a':{
                // Feature 1: Add Receiver
                ConsoleAddTransmitter();
            }break;
            case 's':{
                // Feature 2: Select Receiver
                ConsoleSelectTransmitter();
            }break;
            case 'r':{
                // Feature 3: Remove Receiver
                ConsoleRemoveTransmitter();
            }
            case'e':{
                return;
            }
        }
    }



}

void ConsoleController::ConsoleSelectTransmitter() {
    std::cout << "Select Transmitter ID.\n";
    std::cout << "List of IDs: " << engine_->GetTransmittersList() << std::endl;
    unsigned int id = 0;
    bool valid_id = false;
    while(!valid_id){
        std::cout << "Enter Transmitter ID: ";
        std::cin >> id;
        if (engine_->transmitters_.find(id) != engine_->transmitters_.end()) {
            engine_->current_transmitter_ = engine_->transmitters_.find(id)->second;
            valid_id = true;
            std::cout << "Transmitter #" << id << " is selected.\n";
        }
    }
}


void ConsoleController::ConsoleSelectReceiver() {
    std::cout << "Select Receiver ID.\n";
    std::cout << "List of IDs: " << engine_->GetReceiversList() << std::endl;
    unsigned int id = 0;
    // Get Receiver ID.
    while(true){
        std::cout << "Enter Receiver ID (0 to exit): ";
        std::cin >> id;
        if(id == 0) return;
        // Verify if ID exists.
        if (engine_->receivers_.find(id) != engine_->receivers_.end()) {
            engine_->current_receiver_ = engine_->receivers_.find(id)->second;
            std::cout << "Receiver #" << id << " is selected.\n";
            return;
        }
    }
}

void ConsoleController::ConsoleConnectReceiverToTransmitter() {
    std::cout << "Connect a receiver to a transmitter.\n";
    std::cout << "Enter Transmitter ID.\n";
    std::cout << "Transmitter IDs: " << engine_->GetTransmittersList() << std::endl;
    unsigned int tx_id = 0;
    // Get Transmitter ID
    while (true) {
        std::cout << "Enter Transmitter ID (0 to exit): ";
        std::cin >> tx_id;
        if (tx_id == 0) return;
        // Verify if ID exists
        if (engine_->transmitters_.find(tx_id) != engine_->transmitters_.end()) {
            std::cout << "Transmitter #" << tx_id << " is selected.\n";
            break;
        }
    }
    // Enter receiver ID.
    unsigned int rx_id = 0;
    std::cout << "Receiver IDs: " << engine_->GetReceiversList() << std::endl;
    while (true) {
        std::cout << "Enter Receiver ID (0 to exit): ";
        std::cin >> rx_id;
        if (rx_id == 0) return;
        if (engine_->receivers_.find(rx_id) != engine_->receivers_.end()) {
            // Connect the rx to tx.
            engine_->ConnectReceiverToTransmitter(tx_id, rx_id);
            std::cout << "Connected Rx #" << rx_id << " to Tx #" << tx_id << ".\n";
            return;
        }
    }
}

void ConsoleController::ConsoleRemoveTransmitter() {
    std::cout << "Remove a transmitter.\n";
    std::cout << "Transmitter IDs: " << engine_->GetTransmittersList() << std::endl;
    unsigned int tx_id;

    // Enter Transmitter ID.
    while(true){
        std::cout << "Enter Transmitter ID (0 to exit): ";
        std::cin >> tx_id;
        if(tx_id == 0) return;
        // Check if the ID exists
        if(engine_->transmitters_.find(tx_id) != engine_->transmitters_.end()){
            // Remove Transmitter.
            engine_->RemoveTransmitter(tx_id);
            std::cout << "Removed Tx #" << tx_id << std::endl;
            return;
        }
    }
}

void ConsoleController::ConsoleRemoveReceiver() {
    std::cout << "Remove a receiver.\n";
    std::cout << "Receiver IDs: " << engine_->GetReceiversList() << std::endl;
    unsigned int rx_id;

    while(true){
        std::cout << "Enter Receiver ID (0 to exit): ";
        std::cin >> rx_id;
        if(rx_id == 0) return;
        if(engine_->receivers_.find(rx_id) != engine_->receivers_.end()){
            engine_->RemoveReceiver(rx_id);
            std::cout << "Removed Rx #" << rx_id << std::endl;
            return;
        }
    }
}

void ConsoleController::ConsoleDisconnectReceiverFromTransmitter() {
    std::cout << "Disconnect a receiver from a transmitter.\n";
    std::cout << "Enter Transmitter ID.\n";
    std::cout << "Transmitter IDs: " << engine_->GetTransmittersList() << std::endl;
    unsigned int tx_id = 0;
    // Get Transmitter ID
    while (true) {
        std::cout << "Enter Transmitter ID (0 to exit): ";
        std::cin >> tx_id;
        if (tx_id == 0) return;
        // Verify if ID exists
        if (engine_->transmitters_.find(tx_id) != engine_->transmitters_.end()) {
            std::cout << "Transmitter #" << tx_id << " is selected.\n";
            break;
        }
    }
    // Enter receiver ID.
    unsigned int rx_id = 0;
    std::cout << "Receiver IDs: " << engine_->GetReceiversList() << std::endl;
    while (true) {
        std::cout << "Enter Receiver ID (0 to exit): ";
        std::cin >> rx_id;
        if (rx_id == 0) return;
        if (engine_->receivers_.find(rx_id) != engine_->receivers_.end()) {
            // Connect the rx to tx.
            engine_->DisconnectReceiverFromTransmitter(tx_id, rx_id);
            std::cout << "Disconnected Rx #" << rx_id << " to Tx #" << tx_id << ".\n";
            return;
        }
    }
}

void ConsoleController::RunViewMode() {

}
