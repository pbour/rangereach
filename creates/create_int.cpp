#include "../def_global.h"
#include "../containers/graph.h"
#include "../containers/labeling.h"
#include <unordered_set>



inline void propagate(Labeling &L, vector<vector<ID> > &A, Component currc, Component &adjc)
{
//    cout << "A(" << currc.postnum << "):";
    for (ID &postnum: A[currc.postnum])
    {
//        cout << " " << postnum;
        IntervalList &l = L.intervals[postnum];

//        for (Interval &i: L.intervals[adjc.postnum])
//            l.emplace_back(i.start, i.end);

        l.insert(l.end(), L.intervals[adjc.postnum].begin(), L.intervals[adjc.postnum].end());
        A[adjc.postnum].push_back(postnum);
    }
//    cout << endl;
}


class mycomparison
{
public:
    bool operator() (const Component& lhs, const Component& rhs) const
    {
//        cout << "\t\tcompare C" << lhs.postnum << " (" << lhs.indegree << ") to C" << rhs.postnum << "(" << rhs.indegree << ")" << endl;
//        if (lhs.indegree != rhs.indegree)
            return (lhs.indegree > rhs.indegree);
//        else
//            return (lhs.postnum < rhs.postnum);
    }
};


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << endl;
        cerr << "Usage: " << argv[0] << " INPUT_PREFIX [reverse]" << endl;
        cerr << endl;
        
        return 1;
    }
    string prefix = string(argv[1]);
    bool reverse  = ((argc == 3) && (string(argv[2]) == "reverse")? true: false);
    string fileInts = prefix + ((reverse) ? ".rints": ".ints");
    Timer tim;
    Labeling L;
    
    
    // STEP 1: load graph
    cout << endl << "Load geosocial DA graph ..." << endl;
    GeosocialDAGraph G(prefix);
//    G.print();
    cout << "\tdone" << endl;
    
    
    // STEP 2: reverse graph edges if requested
    if (reverse)
    {
        cout << endl << "Reverse edges ..." << endl;
        G.reverse();
//        G.print();
        cout << "\tdone" << endl;
    }
    
    
    // STEP 2: compute spanning forest and create its labels
    cout << endl << "Compute spanning forest and create its interval labels ..." << endl;
    vector<pair<ID, ID> > nonSpanningEdges;
    bool *C = new bool[G.numComponents];
    priority_queue<Component, std::vector<Component>, mycomparison> Q;
    vector<vector<ID> > A(G.numComponents);

    memset(C, 0, G.numComponents*sizeof(bool));
    L.numPostnums = G.numComponents;
    L.intervals.resize(L.numPostnums);

    tim.start();
    for (const Component &c: G.components)
    {
        L.intervals[c.postnum].emplace_back(c.postnum, c.postnum);
        A[c.postnum].push_back(c.postnum);
        if (c.indegree == 0)
            Q.push(c);
    }
//    if (Q.size() == 0)
//        Q.push();
    
    while (!Q.empty())
    {
        Component currc = Q.top();
        Q.pop();
        
//        cout << "Component " << currc.id << " (" << currc.postnum << ")" << endl;
        for (const ID acid: G.cadjs[currc.id])
        {
            if (!C[acid])
            {
                ID apostnum = G.components[acid].postnum;
//                cout << "\tadjacent Component " << acid << endl;
                // Propagate acid labels to all its ancestors, including currc
                
                if (A[currc.postnum].size() == 1)
                {
                    L.intervals[currc.postnum].insert(L.intervals[currc.postnum].end(), L.intervals[apostnum].begin(), L.intervals[apostnum].end());
                    A[apostnum].push_back(currc.postnum);
                }
                else
                    propagate(L, A, currc, G.components[acid]);
                
                Q.push(G.components[acid]);
                C[acid] = true;
            }
            else
                nonSpanningEdges.push_back(make_pair<ID, ID>((ID)currc.id, (ID)acid));
        }
    }
    delete[] C;
    cout << "\tdone" << endl;
    
    
    // STEP 3: examine non-spanning edges and create the corresponding labels
    cout << endl << "Examine " << nonSpanningEdges.size() << " non-spanning edges and create the corresponding interval labels ..." << endl;
    sort(nonSpanningEdges.begin(), nonSpanningEdges.end(),
         [](const pair<ID,ID> &lhe, const pair<ID,ID> &rhe) -> bool
         {
            return lhe.first < rhe.first;
         });
    for (pair<ID,ID> &p: nonSpanningEdges)
    {
//        cout << "Non-spanning edge (" << p.first << "," << p.second << ")" << endl;
        propagate(L, A, G.components[p.first], G.components[p.second]);
    }
    cout << "\tdone" << endl;
    
//    L.print();
    
    cout << endl << "Compress labels ..." << endl;
    ofstream out(fileInts);
    out << L.numPostnums << endl;
    for (auto cid = 0; cid < L.numPostnums; cid++)
    {
        IntervalList &l = L.intervals[cid];
        
        sort(l.begin(), l.end(),
             [](const Interval &lhi, const Interval &rhi) -> bool
             {
                return lhi.start < rhi.start;
             });
        
        ID from = l[0].start, to = l[0].start;
        for (auto j = 1; j < l.size(); j++)
        {
            if (l[j].start == to)
                continue;
            else if (l[j].start == to+1)
                to = l[j].start;
            else
            {
                out << cid << " " << from << " " << to << endl;
                from = to = l[j].start;
            }
        }
        out << cid << " " << from << " " << to << endl;
    }
    out.close();
    cout << "\tdone" << endl;
    
    cout << endl << "Total time elapsed [secs]: " << tim.stop() << endl << endl;

    
    return 0;
}
