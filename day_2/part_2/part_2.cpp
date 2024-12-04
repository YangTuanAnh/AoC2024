#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <windows.h>
#include <psapi.h>
#include <omp.h>
#include <vector>

bool isSafe(std::vector<int> &num) {
    if (num.size() < 3) return true;
    bool increasing = num[0] < num[1];
    for (int i=1; i<num.size(); i++)
        if (num[i]==num[i-1] || abs(num[i]-num[i-1]) > 3 || (increasing ^ (num[i] > num[i-1])))
            return false;
    return true;
}
void solve(std::ifstream &inputFile, std::ostream* output) {
    std::string line;
    
    int total = 0;
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        int x;
        std::vector<int> num;

        while (ss >> x) {
            num.push_back(x);
        }

        bool found = isSafe(num);

        if (!found) {
            int n = num.size();
            for (int i=0; i<n; i++) {
                std::vector<int> temp = num;
                temp.erase(temp.begin()+i);
                if (isSafe(temp)) {
                    found = true;
                    break;
                }
            }
        }
        total += found;
    }
    *output << total;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]\n";
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file " << argv[1] << "\n";
        return 1;
    }

    std::ostream* output;
    std::ofstream outputFile;
    if (argc >= 3) {
        outputFile.open(argv[2]);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open output file " << argv[2] << "\n";
            return 1;
        }
        output = &outputFile;
    } else {
        output = &std::cout;
    }

    auto start = std::chrono::high_resolution_clock::now();

    solve(inputFile, output);
    *output << '\n';

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end-start;

    *output << "Time spent: " << duration.count() << '\n';

    PROCESS_MEMORY_COUNTERS memInfo;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo))) {
        *output << "Memory usage: " << memInfo.WorkingSetSize / 1024 << " KB\n";
    }
    
    inputFile.close();
    if (outputFile.is_open()) {
        outputFile.close();
    }
    return 0;
}

// g++ ./part_2.cpp -o program ; if ($?) {./program ./input.txt ./output.txt}