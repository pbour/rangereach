#include "../def_global.h"
#include "../containers/graph.h"
#include "../containers/labeling.h"



//#define DEBUG
//#define STATS


int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << endl;
        cerr << "Usage: " << argv[0] << " INPUT_PREFIX QUERY_FILE" << endl;
        cerr << endl;

        return 1;
    }
    string prefix      = string(argv[1]);
    string fileQueries = string(argv[2]);
    unsigned int numRuns = 1;//((argc == 4) ? atoi(argv[3]): 1);
    ID numNodes;
    ID nid, postnum;
    ifstream inp;
    double xlow, ylow, xhigh, yhigh;
    size_t numQueries = 0, numTrues = 0, numGCands = 0;
    Timer tim;
    double timeIndexing, timeQuerying, qtime;

    
    // Step 1: load graph
    GeosocialDAGraph G(prefix);
//    G.print();
        

    // STEP 2: load interval scheme
    Labeling L(prefix, false);
//    L.print();

    
    // STEP 3: execute queries
    inp.open(fileQueries);
    if (!inp)
    {
        cerr << endl << "Error: cannot open queries file \"" << fileQueries << "\"" << endl << endl;
        return 1;
    }

#ifdef DEBUG
    cout << "Queries" << endl;
    cout << "=======" << endl << endl;
#else
#ifdef STATS
    cout << "query-node\tquery-area\tquery-node-out-degree\tresult\tquery-time\tgcands" << endl;
#else
//    cout << "query-node\tquery-area\tquery-node-out-degree\tresult\tquery-time" << endl;
#endif
#endif
    
    while (inp >> nid >> xlow >> ylow >> xhigh >> yhigh)
    {
#ifdef DEBUG
        cout << "Q: N" << nid << " (" << xlow << ", " << ylow << ") -> (" << xhigh << ", " << yhigh << ")" << endl;
#else
//        cout << nid << "\t(" << xlow << "," << ylow << ")->(" << xhigh << "," << yhigh << ")\t" << G.nodes[nid].outdegree << "\t";
#endif

        bool qres = false;

        // Execute range queries
        tim.start();
            vector<ID> cands;
            for (const Interval &i: L.intervals[G.nodes[nid].postnum])
            {
#ifdef DEBUG
                cout << "\tuse interval [" << i.start << "," << i.end << "]" << endl;
#endif
                
                for (ID postnum = i.start; postnum <= i.end; postnum++)
                    cands.push_back(postnum);
            }
            
#ifdef STATS
            numGCands = cands.size();
#endif
            
            for (const ID postnum: cands)
            {
//                if (G.post2id[postnum] != nid)
//                {
                    Component *c = G.postnum2component[postnum];

#ifdef DEBUG
                    cout << "\t\tuse component C" << c->id << " (" << postnum << ")" << endl;
#endif
                        if (c->isSpatial)
                        {
//                            cout << "yes" << endl;
                            for (const Node *n: c->nodes)
                            {
#ifdef BOOST_GEOMETRIES
                                auto nx = bg::get<0>(n->geometry);
                                auto ny = n->geometry.get<1>();
#else
                                auto nx = n->geometry.x;
                                auto ny = n->geometry.y;
                                
#endif
                                if ((nx < xlow) || (nx > xhigh) || (ny < ylow) || (ny > yhigh))
                                    continue;
                                else
                                {
#ifdef DEBUG
                                    cout << "\t\t\tfound N" << n->id << " (" << n->postnum << ")" << endl;
#endif
                                    
                                    qres = true;
                                    break;
                                }
                            }
                        }
                    if (qres)
                        break;
//                }
            }
        qtime = tim.stop();
        timeQuerying += qtime;
        numQueries++;
        
        if(qres)
            numTrues++;

#ifdef DEBUG
        cout << "\t" << ((qres)? "TRUE": "FALSE") << endl << endl;
#else
//        cout << ((qres)? "TRUE": "FALSE") << "\t";
#ifdef STATS
        printf("%lf\t%d\n", qtime/numRuns, numGCands);
#else
//        printf("%lf\n", qtime/numRuns);
#endif
#endif
    }
    inp.close();

    
    cout << endl;
    cout << "Report" << endl;
    cout << "======" << endl << endl;
    cout << "Input prefix             : " << prefix << endl;
    cout << "Query file               : " << fileQueries << endl;
    cout << "Method                   : SocReach" << endl << endl;
    cout << "Indexing time [secs]     : 0" << endl;
    cout << "Index size [Bytes]       : " << L.getSize() << endl << endl;
    cout << "Num of queries           : " << numQueries << endl;
    cout << "Num of runs per query    : " << numRuns << endl;
    cout << "Num of true results      : " << numTrues << endl;
    printf("Avg query time [secs]    : %.10lf\n", (timeQuerying/numRuns)/numQueries);
    cout << endl;


    return 0;
}
