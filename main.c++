// Nhan Tri Danh 
// Project 2: IDE
// 


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;


// Constant Variables
const int MEMORY_SIZE = 32;


// ---------------------------Resgister Table Class-----------------------------
struct Register 
{
    string name;
    int address;
    int value;
    bool isUsed;
};

class RegisterTable
{
private:
    Register registers[MEMORY_SIZE];
    int count; // number of used registers
public:
    //fuction declaration
    RegisterTable();
    bool setRegister(const string& name, int value);
    bool getRegister(const string& name, int& address, int& value);
    void displayRegisters();
};

RegisterTable::RegisterTable() : count(0)
{
    // let registers are unused
    for(int i = 0; i < MEMORY_SIZE; i++) {
        registers[i].isUsed = false;
    }
}

bool RegisterTable::setRegister(const string& name, int value) {
    for(int i = 0; i < MEMORY_SIZE; i++) {
        if(registers[i].isUsed && registers[i].name == name) {
            // registers[i].address = address;
            registers[i].value = value;
            return true;
        }
    }

    if(count < MEMORY_SIZE) {
        for (int i = 0; i < MEMORY_SIZE; i++) {
            if(!registers[i].isUsed) {
                registers[i].name = name;
                registers[i].address = i;
                registers[i].value = value;
                registers[i].isUsed = true;
                count++;
                return true;
            }
        }
    }

    return false; // this tell us table is full
}

bool RegisterTable::getRegister(const string& name, int& address, int& value) {
    for(int i = 0; i < MEMORY_SIZE; i++) {
        if(registers[i].name == name) {
            address = registers[i].address;
            value = registers[i].value;
            return true;
        }
    }
    return false;
}

void RegisterTable::displayRegisters() {
    cout << "Register Table:" << endl;
    cout << "---------------------------" << endl;
    cout << "Name\tAddress\tValue" << endl;
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (registers[i].isUsed) {
            cout << registers[i].name << "\t" 
                 << registers[i].address << "\t"
                 << registers[i].value << endl;
        }
    }
    cout << "---------------------------" << endl;
}


// ---------------------------Source Processor Class-----------------------------
// Let's start with declare command (d)
// ex: d x 0
// ex: d y 12

class Processor
{
private:
    RegisterTable regTable;
    int workingRegister;
    bool running;
    bool comparisonResult; // Stores the result of the most recent comparison

public:
    Processor(/* args */);
    void processCommand(const string& line);
    void handleDeclare(const string& name, int address);
    void processFile(const string& filename);
    void handleGet(const string& name);      // G command
    void handleAdd(const string& name);      // A command
    void handlePut(const string& name);      // P command
    void handleCompare(const string& name);  // C command
    void handleStop();                       // S command

};

Processor::Processor() : workingRegister(0), running(true), comparisonResult(false)
{
}

void Processor::processCommand(const string& line) {
    if(line.empty()) {
        return;
    }

    istringstream iss(line);
    string command;
    iss >> command; //This will help taking the first token vand put in command variable

    if(command == "d") {
        string name;
        int value;
        if(iss >> name >> value) {
            handleDeclare(name, value);
            regTable.displayRegisters();
        } else {
            cout << "Error: Invalid declare command format" << endl;
        } 
    } else if (command == "G") { // move value of the 'name' to the 'register'    
        string name;
        if(iss >> name) {
            handleGet(name);
        } else {
            cout << "Error: Invalid G command format" << endl;
        }
    } else if (command == "A") {
        string name;
        if(iss >> name) {
            handleAdd(name);
        } else {
            cout << "Error: Invalid A command format" << endl;
        }
    } else if (command == "P") {
        string name;
        if(iss >> name) {
            handlePut(name);
        } else {
            cout << "Error: Invalid P command format" << endl;
        }
    } else if (command == "S") { 
        handleStop();
    } else {
        cout << "Unknown command: " << command << endl;
    }

}

void Processor::handleDeclare(const string& name, int value) {
    if (regTable.setRegister(name, value)) {
        cout << "Declared register " << name << endl;
    } else {
        cout << "Failed to declare register " << name << " (register table full)" << endl;
    }
}

void Processor::handleGet(const string& name) {
    int address, value;
    if(regTable.getRegister(name, address, value)) {
        workingRegister = value;
        cout << "Loaded value " << value << " from register " << name << " into working register" << endl;
    } else {
        cout << "Error: Register " << name << " not found" << endl;
    }
}

void Processor::handlePut(const string& name) {
    if (regTable.setRegister(name, workingRegister)) {
        cout << "Stored working register value " << workingRegister << " to register " << name << endl;
    } else {
        cout << "Error: Failed to store working register value to register " << name << endl;
    }
}

void Processor::handleAdd(const string& name) {
    int address, value;
    if(regTable.getRegister(name, address, value)) {
        workingRegister += value;
        cout << "Added " << value << " from register " << name << " to working register. Result: " << workingRegister << endl;
    } else {
        cout << "Error: Register " << name << " not found" << endl;
    }
}

void Processor::handleCompare(const string& name) {
    int address, value;
    if(regTable.getRegister(name, address, value)) {
        // Compare: is workingRegister <= value?
        comparisonResult = (workingRegister <= value);
        cout << "Compared working register (" << workingRegister << ") with " 
             << name << " (" << value << "). Result: " 
             << (comparisonResult ? "true" : "false") << endl;
    } else {
        cout << "Error: Register " << name << " not found" << endl;
    }
}

void Processor::handleStop() {
    running = false;
    regTable.displayRegisters();
    cout << "Execution stopped" << endl;
}

void Processor::processFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        cout << "Processing: " << line << endl;
        processCommand(line);
    }

    file.close();
}



// Main
int main() {
    Processor processor;
    processor.processFile("mysource.txt");
    return 0;
}
