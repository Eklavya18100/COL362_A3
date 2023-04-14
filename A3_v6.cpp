
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
using namespace std;
#define INITIALRUNSIZE 104857 // 104857

template <typename T>
class List
{
private:
    struct Node
    {
        T data;
        Node *next;
        Node(const T &d) : data(d), next(nullptr) {}
    };
    Node *head;

public:
    class iterator
    {
    private:
        Node *current;

    public:
        iterator(Node *n = nullptr) : current(n) {}
        iterator &operator++()
        {
            current = current->next;
            return *this;
        }
        T &operator*()
        {
            return current->data;
        }
        bool operator==(const iterator &other) const
        {
            return current == other.current;
        }
        bool operator!=(const iterator &other) const
        {
            return !(*this == other);
        }
    };

    List() : head(nullptr) {}
    ~List()
    {
        while (head != nullptr)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
    }

    void push_back(const T &data)
    {
        Node *temp = new Node(data);
        if (head == nullptr)
        {
            head = temp;
        }
        else
        {
            Node *current = head;
            while (current->next != nullptr)
            {
                current = current->next;
            }
            current->next = temp;
        }
    }

    void sort()
    {
        if (head == nullptr || head->next == nullptr)
        {
            return;
        }
        List<T> left, right;
        int size = 0;
        for (Node *current = head; current != nullptr; current = current->next)
        {
            ++size;
        }
        int middle = size / 2;
        Node *current = head;
        for (int i = 0; i < middle; ++i)
        {
            left.push_back(current->data);
            current = current->next;
        }
        for (; current != nullptr; current = current->next)
        {
            right.push_back(current->data);
        }
        left.sort();
        right.sort();
        head = nullptr;
        while (left.head != nullptr && right.head != nullptr)
        {
            if (left.head->data < right.head->data)
            {
                push_back(left.head->data);
                left.head = left.head->next;
            }
            else
            {
                push_back(right.head->data);
                right.head = right.head->next;
            }
        }
        while (left.head != nullptr)
        {
            push_back(left.head->data);
            left.head = left.head->next;
        }
        while (right.head != nullptr)
        {
            push_back(right.head->data);
            right.head = right.head->next;
        }
    }

    int size() const
    {
        int size = 0;
        for (Node *current = head; current != nullptr; current = current->next)
        {
            ++size;
        }
        return size;
    }

    T &front()
    {
        return head->data;
    }

    void pop_front()
    {
        if (head == nullptr)
        {
            return;
        }
        Node *temp = head;
        head = head->next;
        delete temp;
    }

    iterator begin() const
    {
        return iterator(head);
    }

    iterator end() const
    {
        return iterator(nullptr);
    }
};

// list<string> runs;
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

// void mergeRuns(const list<string> &inputFiles, const string &outputFile)
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
        // quicksort(temp, 0, temp.size());
        sort(temp.begin(),temp.end());
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
    // int totalMerges = externalmergesortwithstop("random_10gb.list", "output", 2526350);
    // int totalMerges = externalmergesortwithstop("englishsubset.txt", "output", 1000000);
    int totalMerges = external_merge_sort_withstop("random.list", "output", 1000000);
    cout << totalMerges;
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    cout << "\nTime measured: " << elapsed.count() * 1e-9 << " seconds.\n";
}