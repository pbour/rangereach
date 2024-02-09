#include "../def_global.h"


struct Interval
{
    ID start, end;
    
    Interval()
    {
        
    };
    
    Interval(ID st, ID e)
    {
        start = st;
        end = e;
    };
};

typedef vector<Interval>    IntervalList;

class Labeling
{
public:
    vector<IntervalList> intervals;
    ID numPostnums;
    
    Labeling();
    Labeling(string prefix, bool isReverse);
    void print();
    size_t getSize();
    ~Labeling();
};
