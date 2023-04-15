#include <iostream>
#include <fstream>
#include <memory>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <cstring>
#include <cmath>

using namespace std;
#define INITIALRUNSIZE 1619428 

list<string> runs;

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

// void mergeRuns(const List<string> &inputFiles, const string &outputFile)
void mergeRuns(const list<string> &inputFiles, const string &outputFile)
{
    // Open all the input files in read mode
    vector<ifstream> input_file_ptrs;
    for (auto const &file : inputFiles)
    {
        input_file_ptrs.emplace_back(move(file));
    }
    // Open the output file in write mode
    ofstream output_file_ptr(outputFile);
    // Initialize a priority queue to keep track of the smallest line from each file
    priority_queue<Node *, vector<Node *>, compareVal> pq;
    // Read the first line from each file and store them in the priority queue
    string line;
    for (int i = 0; i < input_file_ptrs.size() && getline(input_file_ptrs[i], line); i++)
    {
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
    string line;
    while (getline(inputFile, line)&& keys < key_count)
    {
        string fileName = "temp.0." + to_string(runNum++);
        ofstream outputFile(fileName);
        runs.push_back(fileName);
        deque<string> temp;
        temp.push_back(move(line));
        for (int ct = 0; ct < INITIALRUNSIZE && getline(inputFile, line) && keys < key_count; ct++)
        {
            temp.push_back(move(line)); 
            keys++;
        }
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
                list<string> newVec;
                string outFile = totalRuns <= k ? output : "temp." + to_string(stage) + "." + to_string(runNum++);

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