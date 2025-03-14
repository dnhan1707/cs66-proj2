// Nhan Tri Danh 
// Project 2: IDE
// Working


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Constants
const int MEMORY_SIZE = 32;  // Size of the memory array
const int MAX_CODE_LINES = 100;  // Maximum number of instruction lines

// Define operation codes
const int OP_STOP = 0;     // S - Stop
const int OP_GET = 1;      // G - Get
const int OP_PUT = 2;      // P - Put
const int OP_ADD = 3;      // A - Add


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
    int initialValues[MEMORY_SIZE];
    int count; // number of used registers
public:
    //fuction declaration
    RegisterTable();
    bool setRegister(const string& name, int value);
    bool getRegister(const string& name, int& address, int& value);
    bool getRegisterByIndex(int index, int& address, int& value);
    void displayRegisters();
    int getInitialValue(int index);

};

RegisterTable::RegisterTable() : count(0)
{
    // let registers are unused
    for(int i = 0; i < MEMORY_SIZE; i++) {
        registers[i].isUsed = false;
        initialValues[i] = -1; // -1 means no initial value
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

                initialValues[i] = value;

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

int RegisterTable::getInitialValue(int index) {
    if (index >= 0 && index < MEMORY_SIZE) {
        return initialValues[index];
    }
    return -1;
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

bool RegisterTable::getRegisterByIndex(int index, int& address, int& value) {
    if (index >= 0 && index < MEMORY_SIZE && registers[index].isUsed) {
        address = registers[index].address;
        value = registers[index].value;
        return true;
    }
    return false;
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
    void generateDataFile();
    void generateCodeFile(const string& sourceFilename);
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
    // Generate output files
    generateDataFile();
    generateCodeFile("mysource.txt");
    
    cout << "Execution stopped" << endl;
}

void Processor::generateDataFile() {
    ofstream dataFile("data.txt");
    if (!dataFile.is_open()) {
        cerr << "Error: Could not create data.txt" << endl;
        return;
    }

    // Write initial values in address order
    for (int i = 0; i < MEMORY_SIZE; i++) {
        int address, value;
        if (regTable.getRegisterByIndex(i, address, value)) {
            int initialValue = regTable.getInitialValue(i);
            if (initialValue != -1) {
                dataFile << initialValue << endl;
            }
        }
    }
    
    dataFile.close();
}

void Processor::generateCodeFile(const string& sourceFilename) {
    ifstream sourceFile(sourceFilename);
    ofstream codeFile("code.txt");
    
    if (!sourceFile.is_open() || !codeFile.is_open()) {
        cerr << "Error: Could not open source file or create code.txt" << endl;
        return;
    }

    string line;
    while (getline(sourceFile, line)) {
        // Skip empty lines or comments
        if (line.empty() || line[0] == '/') {
            continue;
        }

        istringstream iss(line);
        string command;
        iss >> command;

        int commandCode = -1;
        string regName;
        int address = 0;

        if (command == "G") {
            commandCode = 1; // 001
            iss >> regName;
        } else if (command == "P") {
            commandCode = 2; // 010
            iss >> regName;
        } else if (command == "A") {
            commandCode = 3; // 011
            iss >> regName;
        } else if (command == "S") {
            commandCode = 0; // 000
            // No register for S command
            codeFile << commandCode << " " << 0 << endl;
            continue;
        } else {
            // Skip other commands (like 'd' for declare)
            continue;
        }

        // Get the address for the register
        int value;
        if (regTable.getRegister(regName, address, value)) {
            codeFile << commandCode << " " << address << endl;
        }
    }

    sourceFile.close();
    codeFile.close();
    cout << "Generated code.txt with command codes" << endl;
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


// ---------------------------Simulator-----------------------------

struct Instruction {
    int opcode;
    int address;
};

void readDataFile(int memory[], int memorySize) {
    ifstream dataFile("data.txt");
    if (!dataFile.is_open()) {
        cerr << "Error: Could not open data.txt" << endl;
        return;
    }
    
    // Initialize memory with question marks (-1 represents '?')
    for (int i = 0; i < memorySize; i++) {
        memory[i] = -1;
    }
    
    // Read values into memory
    string line;
    int i = 0;
    while (getline(dataFile, line) && i < memorySize) {
        if (!line.empty()) {
            memory[i] = stoi(line);
            i++;
        }
    }
    
    dataFile.close();
    cout << "Loaded " << i << " values into memory" << endl;
}

int readCodeFile(Instruction instructions[], int maxInstructions) {
    ifstream codeFile("code.txt");
    if (!codeFile.is_open()) {
        cerr << "Error: Could not open code.txt" << endl;
        return 0;
    }
    
    int i = 0;
    string line;
    while (getline(codeFile, line) && i < maxInstructions) {
        istringstream iss(line);
        if (iss >> instructions[i].opcode >> instructions[i].address) {
            i++;
        }
    }
    
    codeFile.close();
    cout << "Loaded " << i << " instructions" << endl;
    return i;
}

string getOpName(int opcode) {
    switch(opcode) {
        case OP_STOP: return "STOP";
        case OP_GET:  return "GET";
        case OP_PUT:  return "PUT";
        case OP_ADD:  return "ADD";
        default:      return "UNKNOWN";
    }
}

// Modify the displayState function to write to a file
void displayState(int step, const string& opDescription, int reg, const int memory[], int memorySize, ofstream& outFile) {
    outFile << "Step " << step << ": " << opDescription << endl;
    outFile << "Register ";
    if (reg == -1) {
        outFile << "?";
    } else {
        outFile << reg;
    }
    outFile << "   Memory ";
    
    // Show first 10 memory locations
    for (int i = 0; i < 10 && i < memorySize; i++) {
        if (memory[i] == -1) {
            outFile << "? ";
        } else {
            outFile << memory[i] << " ";
        }
    }
    outFile << endl << endl;
}

// Modify executeInstructions to write to a file
void executeInstructions(int memory[], int memorySize, Instruction instructions[], int numInstructions, ofstream& outFile) {
    int reg = -1;  // Start with register as "?"
    
    // Display initial state
    displayState(0, "Initial state", reg, memory, memorySize, outFile);
    
    // Execute each instruction
    for (int i = 0; i < numInstructions; i++) {
        int opcode = instructions[i].opcode;
        int address = instructions[i].address;
        string opName = getOpName(opcode);
        string description = "";
        
        switch(opcode) {
            case OP_GET: // G - Get value from memory to register
                description = opName + " - Loading value from memory[" + to_string(address) + "] into register";
                if (address >= 0 && address < memorySize && memory[address] != -1) {
                    reg = memory[address];
                }
                break;
                
            case OP_PUT: // P - Put register value into memory
                description = opName + " - Storing register value into memory[" + to_string(address) + "]";
                if (address >= 0 && address < memorySize && reg != -1) {
                    memory[address] = reg;
                }
                break;
                
            case OP_ADD: // A - Add memory value to register
                description = opName + " - Adding memory[" + to_string(address) + "] to register";
                if (address >= 0 && address < memorySize && memory[address] != -1 && reg != -1) {
                    reg += memory[address];
                }
                break;
                
            case OP_STOP: // S - Stop execution
                description = opName + " - Program execution stopped";
                displayState(i+1, description, reg, memory, memorySize, outFile);
                return;
        }
        
        // Display state after this instruction
        displayState(i+1, description, reg, memory, memorySize, outFile);
    }
}


// Update the main function to write to sim66.txt
int main() {
    cout << "Nhan Tri Danh, 10642351" << endl;
    cout << endl;

    cout << "-------------------------Debug Output------------------------" << endl;

    Processor processor;
    processor.processFile("mysource.txt");

    cout << endl;
    cout << "Creating simulator output file: sim66.txt" << endl;
    
    // Open the output file
    ofstream simFile("sim66.txt");
    if (!simFile.is_open()) {
        cerr << "Error: Could not create sim66.txt" << endl;
        return 1;
    }
    
    simFile << "SIM66 - Simple Processor Simulator" << endl;
    simFile << "==================================" << endl << endl;
    
    // Declare memory and instruction arrays
    int memory[MEMORY_SIZE];
    Instruction instructions[MAX_CODE_LINES];
    
    // Read data and code files
    readDataFile(memory, MEMORY_SIZE);
    int numInstructions = readCodeFile(instructions, MAX_CODE_LINES);
    
    if (numInstructions == 0) {
        cerr << "No instructions found in code.txt" << endl;
        simFile.close();
        return 1;
    }
    
    simFile << "Loaded " << numInstructions << " instructions" << endl;
    simFile << "\nStarting execution..." << endl;
    simFile << "==================================" << endl << endl;
    
    executeInstructions(memory, MEMORY_SIZE, instructions, numInstructions, simFile);
    
    simFile << "Program finished" << endl;
    simFile.close();
    
    cout << "Simulation complete. Results written to sim66.txt" << endl;
    return 0;
}

