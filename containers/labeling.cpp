#include "labeling.h"



Labeling::Labeling()
{
}

Labeling::Labeling(string prefix, bool isReverse)
{
    string fileInts = prefix;
    ifstream inp;
    ID postnum;
    Interval i;
    
    if (isReverse)
        fileInts += ".rints";
    else
        fileInts += ".ints";
    
    inp.open(fileInts);
    if (!inp)
    {
        cerr << endl << "Error: cannot open labeling file \"" << fileInts << "\"" << endl << endl;
        exit(1);
    }

    // Read metadata
    inp >> this->numPostnums;
    
    this->intervals.resize(this->numPostnums);
    while (inp >> postnum >> i.start >> i.end)
    {
        this->intervals[postnum].push_back(i);
    }
    inp.close();
}


void Labeling::print()
{
    cout << endl;
    cout << "Interval-based Labeling" << endl;
    cout << "=======================" << endl;
    for (auto p = 0; p < this->numPostnums; p++)
    {
        cout << "Post-order number " << p << endl;
        cout << "\tintervals:";
        for (Interval i: this->intervals[p])
            cout << " [" << i.start << "," << i.end << "]";
        cout << endl << endl;
    }
}


size_t Labeling::getSize()
{
    size_t size = 0;
    
    for (auto p = 0; p < this->numPostnums; p++)
        size += this->intervals[p].size() * sizeof(Interval);

    return size;
}


Labeling::~Labeling()
{
}
