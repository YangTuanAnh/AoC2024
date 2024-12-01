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

void solve(std::ifstream &inputFile, std::ostream* output) {
    std::string line;
    std::vector<int> a, b;

    #pragma omp parallel
    {
        std::vector<int> local_a, local_b;

        #pragma omp single
        {
            while (std::getline(inputFile, line)) {
                std::stringstream ss(line);
                int x, y;
                ss >> x >> y;
                local_a.push_back(x);
                local_b.push_back(y);
            }
        }

        #pragma omp critical
        {
            a.insert(a.end(), local_a.begin(), local_a.end());
            b.insert(b.end(), local_b.begin(), local_b.end());
        }
    }

    #pragma omp parallel sections
    {
        #pragma omp section
        sort(a.begin(), a.end());

        #pragma omp section
        sort(b.begin(), b.end());
    }

    int total = 0;
    #pragma omp parallel for reduction(+:total)
    for (int i = 0; i < a.size(); i++) {
        total += abs(a[i] - b[i]);
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

// g++ ./part_1.cpp -o program ; if ($?) {./program ./input.txt ./output.txt}