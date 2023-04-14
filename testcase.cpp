#include <iostream>
#include <fstream>
#include <random>
#include <string>

int main() {
    const std::string filename = "random.list";
    const uint64_t file_size = 10ULL * 1024ULL * 1024ULL * 102ULL; // 10GB

    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "Failed to open file " << filename << '\n';
        return 1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> length_dist(1, 1024);
    std::uniform_int_distribution<int> char_dist(33, 126); // Printable ASCII characters

    uint64_t bytes_written = 0;
    while (bytes_written < file_size) {
        std::string str;
        const int str_len = length_dist(gen);
        for (int i = 0; i < str_len; ++i) {
            str += static_cast<char>(char_dist(gen));
        }
        str += '\n';

        fout.write(str.c_str(), str.size());
        if (!fout) {
            std::cerr << "Error writing to file " << filename << '\n';
            return 1;
        }

        bytes_written += str.size();
    }

    fout.close();
    std::cout << "File " << filename << " created with size " << bytes_written << " bytes.\n";
    return 0;
}
