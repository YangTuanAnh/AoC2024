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

std::vector<int> kmp(std::string s)
{
    int n = s.length();
    std::vector<int> p(n);
    for (int i = 1; i < n; i++)
    {
        int j = p[i - 1];
        while (j && s[i] != s[j])
            j = p[j - 1];
        if (s[i] == s[j])
            p[i] = j + 1;
    }
    return p;
}
void solve(std::ifstream &inputFile, std::ostream *output)
{
    std::string line;

    std::string pref_mul = "mul(";
    const int MUL_SIZE = pref_mul.size();
    long long total = 0;
    while (std::getline(inputFile, line))
    {
        std::vector<int> p = kmp(pref_mul + line);
        for (int i = 0; i < line.length(); i++)
            if (p[i + MUL_SIZE] == MUL_SIZE)
            {
                long long a = -1, b = -1;
                bool comma = false, found = false;
                i++;
                while (i < line.length() && p[i + MUL_SIZE + 1] != MUL_SIZE)
                {
                    if (line[i] == ',')
                    {
                        if (a == -1)
                            break;
                        comma = true;
                    }
                    else if (line[i] >= '0' && line[i] <= '9')
                    {
                        if (comma == false)
                        {
                            if (a == -1)
                                a = 0;
                            a = a * 10 + line[i] - '0';
                        }
                        else
                        {
                            if (b == -1)
                                b = 0;
                            b = b * 10 + line[i] - '0';
                        }
                    }
                    else if (line[i] == ')')
                    {
                        found = true;
                        break;
                    }
                    else
                        break;
                    i++;
                }
                if (comma && found && a != -1 && b != -1)
                {
                    total += a * b;
                }
            }
    }
    *output << total;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]\n";
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open())
    {
        std::cerr << "Error: Could not open input file " << argv[1] << "\n";
        return 1;
    }

    std::ostream *output;
    std::ofstream outputFile;
    if (argc >= 3)
    {
        outputFile.open(argv[2]);
        if (!outputFile.is_open())
        {
            std::cerr << "Error: Could not open output file " << argv[2] << "\n";
            return 1;
        }
        output = &outputFile;
    }
    else
    {
        output = &std::cout;
    }

    auto start = std::chrono::high_resolution_clock::now();

    solve(inputFile, output);
    *output << '\n';

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;

    *output << "Time spent: " << duration.count() << '\n';

    PROCESS_MEMORY_COUNTERS memInfo;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo)))
    {
        *output << "Memory usage: " << memInfo.WorkingSetSize / 1024 << " KB\n";
    }

    inputFile.close();
    if (outputFile.is_open())
    {
        outputFile.close();
    }
    return 0;
}

// g++ ./part_1.cpp -o program ; if ($?) {./program ./input.txt ./output.txt}