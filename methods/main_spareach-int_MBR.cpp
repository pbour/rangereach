#include "../def_global.h"
#include "../containers/graph.h"
#include "../containers/labeling.h"
#include "../containers/rtree_statistics.hpp"



//#define DEBUG
//#define STATS
#define RTREE_BULKLOAD


bool reachable(Labeling &L, ID n1, ID n2)
{
    for (const Interval &i: L.intervals[n1])
    {
        if ((i.start <= n2) && (n2 <= i.end))
            return true;
    }
    
    return false;
}


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
    ID nid, postnum;
    ifstream inp;
    double xlow, ylow, xhigh, yhigh;
    size_t numQueries = 0, numTrues = 0, numSCands = 0;
    Timer tim;
    double timeIndexing, timeQuerying, qtime;
    size_t numReachTests = 0, qreachtests;

    
    // Step 1: load graph
    GeosocialDAGraph G(prefix);
//    G.print();
        

    // STEP 2: load interval scheme
    Labeling L(prefix, false);
//    L.print();

    
    // STEP 3: build R-tree
    tim.start();
#ifdef RTREE_BULKLOAD
    vector<pair<bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>>, ID>> cloud;
    
    for (const Component &c: G.components)
    {
        if (c.isSpatial)
            cloud.push_back(make_pair(bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>>(bg::model::point<float, 2, bg::cs::cartesian>(c.geometry.xlow, c.geometry.ylow), bg::model::point<float, 2, bg::cs::cartesian>(c.geometry.xhigh, c.geometry.yhigh)), c.postnum));
    }

    bgi::rtree<pair<bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>>, ID>, bgi::quadratic<16>> rtree(cloud);
#else
    bgi::rtree<pair<bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>>, ID>, bgi::quadratic<16>> rtree;

    for (const Component &c: G.components)
    {
//        if (c.nodes.size() > 1)
//            cout << c.id << ": " << c.nodes.size() << endl;
//        continue;
        if (c.isSpatial)
        {
#ifdef BOOST_GEOMETRIES
            rtree.insert(make_pair(c.geometry, c.postnum));
#else
            rtree.insert(make_pair(bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>>(bg::model::point<float, 2, bg::cs::cartesian>(c.geometry.xlow, c.geometry.ylow), bg::model::point<float, 2, bg::cs::cartesian>(c.geometry.xhigh, c.geometry.yhigh)), c.postnum));
#endif
        }
    }
#endif
    timeIndexing = tim.stop();


    // STEP 4: execute queries
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
    cout << "query-node\tquery-area\tquery-node-out-degree\tresult\tquery-time\treach-tests\tscands" << endl;
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

        if (nid >= G.numNodes)
        {
            cerr << "Ooops: " << nid << endl;
            return 1;
        }
        
        bool qres = false;
        ID qpostnum = G.nodes[nid].postnum;

        // Execute range queries
        tim.start();
#ifdef STATS
            qreachtests = 0;
#endif

            bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>> qwindow(bg::model::point<float, 2, bg::cs::cartesian>(xlow, ylow), bg::model::point<float, 2, bg::cs::cartesian>(xhigh, yhigh));
            vector<pair<bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>>, ID>> cands;
            rtree.query(bgi::intersects(qwindow), back_inserter(cands));
            for (const pair<bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>>, ID> cand: cands)
            {
                // it->second is a post-order number
                Component *c = G.postnum2component[cand.second];
                
#ifdef DEBUG
                cout << "\tuse component C" << c->id << " (" << c->postnum << "): ";
#ifdef BOOST_GEOMETRIES
                auto cxlow = bg::get<bg::min_corner, 0>(c->geometry);
                auto cylow = bg::get<bg::min_corner, 1>(c->geometry);
                auto cxhigh = c->geometry.max_corner().get<0>();
                auto cyhigh = c->geometry.max_corner().get<1>();
#else
                auto cxlow = c->geometry.xlow;
                auto cylow = c->geometry.ylow;
                auto cxhigh = c->geometry.xhigh;
                auto cyhigh = c->geometry.yhigh;
#endif
                cout << "(" << cxlow  << "," << cylow  << ")->";
                cout << "(" << cxhigh << "," << cyhigh << ")";
                cout << endl;
#endif
                
                if (c->isPoint)
                {
#ifdef STATS
                    qreachtests++;
#endif
                    
                    if (reachable(L, qpostnum, cand.second))
                    {
#ifdef DEBUG
                        cout << "\t\tfound postnum: " << c->postnum << endl;
#endif
                        qres = true;
                        break;
                    }
                }
                else
                {
#ifdef STATS
                    qreachtests++;
#endif

                    // We divert from the spatial-first principle here.
                    if (reachable(L, qpostnum, c->postnum))
                    {
                        for (const Node *n: c->nodes)
                        {
#ifdef BOOST_GEOMETRIES
                            auto nx = bg::get<0>(n->geometry);
                            auto ny = n->geometry.get<1>();
#else
                            auto nx = n->geometry.x;
                            auto ny = n->geometry.y;
#endif
                            
#ifdef DEBUG
                            cout << "\t\tuse node N" << n->id << endl;
#endif
                            
                            if ((nx < xlow) || (nx > xhigh) || (ny < ylow) || (ny > yhigh))
                                continue;
                            else
                            {
                                qres = true;
                                break;
                            }
                        }
                        if (qres)
                            break;
                    }
                }
            }
#ifdef STATS
            numSCands = cands.size();
#endif        
        qtime = tim.stop();
        timeQuerying += qtime;
        numQueries++;

#ifdef STATS
        numReachTests += qreachtests;
#endif
        
        if(qres)
            numTrues++;

#ifdef DEBUG
        cout << "\t" << ((qres)? "TRUE": "FALSE") << endl;
        cout << "\t" << qreachtests << endl;
        cout << "\t" << numSCands << endl;
#else
//        cout << ((qres)? "TRUE": "FALSE") << "\t";
#ifdef STATS
        printf("%lf\t%d\t%d\n", qtime/numRuns, qreachtests, numSCands);
#else
//        printf("%lf\n", qtime/numRuns);
#endif
#endif
    }
    inp.close();

    
    std::tuple<std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t> stats = bgi::detail::rtree::utilities::statistics(rtree);
    
    cout << endl;
    cout << "Report" << endl;
    cout << "======" << endl << endl;
    cout << "Input prefix              : " << prefix << endl;
    cout << "Query file                : " << fileQueries << endl;
    cout << "Method                    : SpaReach-INT, MBR variant" << endl << endl;
    printf("R-tree builing time [secs]: %.10lf\n", timeIndexing);
    cout << "Index size [Bytes]        : " << get<6>(stats)+L.getSize() << endl << endl;
    cout << "Num of queries            : " << numQueries << endl;
    cout << "Num of runs per query     : " << numRuns << endl;
//    printf("Num of reach tests        : %f\n", (numReachTests/(float)numQueries));
    cout << "Num of true results       : " << numTrues << endl;
    printf("Avg query time [secs]     : %.10lf\n", (timeQuerying/numRuns)/numQueries);
    cout << endl;


    return 0;
}
