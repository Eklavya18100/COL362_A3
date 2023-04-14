
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <cstring>
#include <cmath>

using namespace std;
#define INITIALRUNSIZE 209714// 104857 // 104857 

template <typename T>
class List {
private:
    std::vector<T> data;
public:
    List() {}
    ~List() {}
    void push_back(const T &value) {
        data.push_back(value);
    }
    void sort() {
        std::sort(data.begin(), data.end());
    }
    int size() const {
        return data.size();
    }
    T &front() {
        return data.front();
    }
    void pop_front() {
        data.erase(data.begin());
    }
    typename std::vector<T>::iterator begin() {
        return data.begin();
    }
    typename std::vector<T>::iterator end() {
        return data.end();
    }
    typename std::vector<T>::const_iterator begin() const {
        return data.begin();
    }

    typename std::vector<T>::const_iterator end() const {
        return data.end();
    }
};

List<string> runs;

struct Node
{
public:
    string data;
    int line;
    Node(string a, int b)
    {
        data = a;
        line = b;
    }
};

struct compareVal
{
    bool operator()(Node *p1, Node *p2)
    {
        return p1->data > (p2->data); //*ascending order sort
    }
};

void mergeRuns(const List<string> &inputFiles, const string &outputFile)
{
    // Open all the input files in read mode
    vector<ifstream> input_file_ptrs;
    for (const auto &file : inputFiles)
    {
        input_file_ptrs.emplace_back(move(file));
    }
    // Open the output file in write mode
    ofstream output_file_ptr(outputFile);
    // Initialize a priority queue to keep track of the smallest line from each file
    priority_queue<Node *, vector<Node *>, compareVal> pq;
    // Read the first line from each file and store them in the priority queue
    for (int i = 0; i < input_file_ptrs.size(); i++)
    {
        string line;
        getline(input_file_ptrs[i], line);
        pq.emplace(new Node(line, i));
    }
    // Merge the files
    while (!pq.empty())
    {
        auto min_line = pq.top();
        pq.pop();
        output_file_ptr << min_line->data << "\n";
        string line;
        if (getline(input_file_ptrs[min_line->line], line))
        {
            pq.emplace(new Node(line, min_line->line));
        }
    }
    // Close all the input files and the output file
    for (auto &input_file_ptr : input_file_ptrs)
    {
        input_file_ptr.close();
    }
    output_file_ptr.close();
}

void createInitialRuns(const string &inputF, const long key_count)
{
    ifstream inputFile(inputF);
    int runNum = 0;
    int keys = 0;
    while (!inputFile.eof() && keys < key_count)
    {
        string fileName = "temp.0." + to_string(runNum++);
        ofstream outputFile(fileName);
        runs.push_back(fileName);
        deque<string> temp;
        for (int ct = 0; ct < INITIALRUNSIZE && !inputFile.eof() && keys < key_count; ct++)
        {
            string line;
            getline(inputFile, line);
            temp.push_back(move(line)); // use std::move to avoid copying
            keys++;
        }
        // introsort(temp);
        sort(temp.begin(), temp.end());
        int t = temp.size();
        while (t-- != 0)
        {
            outputFile << temp.front() << "\n";
            temp.pop_front();
        }
    }
}

int external_merge_sort_withstop(const char *input, const char *output,
                                 const long key_count, const int k = 2, const int num_merges = 0)
{

    try
    {
        createInitialRuns(input, key_count);
        int numMerges = num_merges;
        if (numMerges == 0)
        {
            numMerges = INT16_MAX;
        }

        int t = runs.size(), totalMergesSoFar = 0;
        int stage = 1;

        if (runs.size() == 1) // only one run, directly output to output file
        {
            ifstream inFile(runs.front());
            ofstream outFile(output);
            outFile << inFile.rdbuf();
            inFile.close();
            outFile.close();
            return 0;
        }

        while (runs.size() != 1)
        {
            int totalRuns = runs.size();
            int runNum = 0;
            int start = 0, end = k - 1;
            while (start < totalRuns)
            {
                // list<string> newVec;
                List<string> newVec;
                string outFile = totalRuns == 2 ? output : "temp." + to_string(stage) + "." + to_string(runNum++);

                for (int i = start; i <= min(end, totalRuns - 1); i++)
                {
                    newVec.push_back(runs.front());
                    runs.pop_front();
                }

                mergeRuns(newVec, outFile);
                totalMergesSoFar++;
                runs.push_back(outFile);
                start = end + 1;
                end = end + k;
                if (totalMergesSoFar > numMerges)
                    return totalMergesSoFar;
            }

            stage++;
        }
        return totalMergesSoFar;
    }
    catch (const std::exception &e)
    {
        return -1;
    }
}

int main()
{
    auto begin = std::chrono::high_resolution_clock::now();
    // int totalMerges = external_merge_sort_withstop("random_10gb.list", "output", 2526350, 8);
    // int totalMerges = external_merge_sort_withstop("englishsubset.txt", "output", 1000000);
    int totalMerges = external_merge_sort_withstop("random.txt", "output", 1000000, 8);
    cout << totalMerges;
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);    
    cout << "\nTime measured: " << elapsed.count() * 1e-9 << " seconds.\n";
}