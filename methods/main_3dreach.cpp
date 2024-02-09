#include "../def_global.h"
#include "../containers/graph.h"
#include "../containers/labeling.h"
#include "../containers/rtree_statistics.hpp"



//#define STATS
#define RTREE_BULKLOAD


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
    size_t numQueries = 0, numTrues = 0;
    Timer tim;
    double timeIndexing, timeQuerying, qtime;
    size_t numTests = 0, tests;

    
    // Step 1: load graph
    GeosocialDAGraph G(prefix);
//    G.print();
        
//     Uncomment the following to generate BFL files
//    cout << "graph_for_greach" << endl;
//    cout << G.numComponents << endl;
//    for (const Component &c: G.components)
//    {
//        cout << c.id << ":";
//        for (const ID acid: G.cadjs[c.id])
//            cout << " " << acid;
//        cout << "#" << endl;
//    }
//    return 1;

    
    // STEP 2: load interval scheme
    Labeling L(prefix, false);
//    L.print();
        

    // STEP 3: build 3D R-tree
    tim.start();
#ifdef RTREE_BULKLOAD
    vector<pair<bg::model::point<float, 3, bg::cs::cartesian>, ID>> cloud;
    
    for (const Node &n: G.nodes)
    {
        if (n.isSpatial)
            cloud.push_back(make_pair(bg::model::point<float, 3, bg::cs::cartesian>(n.geometry.x, n.geometry.y, n.postnum), n.id));
    }

    bgi::rtree<pair<bg::model::point<float, 3, bg::cs::cartesian>, ID>, bgi::quadratic<16>> rtree(cloud);
#else
    bgi::rtree<pair<bg::model::point<float, 3, bg::cs::cartesian>, ID>, bgi::quadratic<16>> rtree;

    for (const Node &n: G.nodes)
    {
        if (n.isSpatial)
        {
#ifdef BOOST_GEOMETRIES
            rtree.insert(make_pair(bg::model::point<float, 3, bg::cs::cartesian>(bg::get<0>(n.geometry), n.geometry.get<1>(), n.postnum), n.id));
#else
            rtree.insert(make_pair(bg::model::point<float, 3, bg::cs::cartesian>(n.geometry.x, n.geometry.y, n.postnum), n.id));
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
    cout << "query-node\tquery-area\tquery-node-out-degree\tresult\tquery-time\ttests" << endl;
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
//        printf("%d %.10lf %.10lf %.10lf %.10lf\t%d\t", nid, xlow, ylow, xhigh, yhigh, G.nodes[nid].outdegree);
#endif
        
        bool qres = false;
        
        // Execute range queries
        tim.start();
#ifdef STATS
                tests = 0;
#endif

            for (const Interval &i: L.intervals[G.nodes[nid].postnum])
            {
#ifdef DEBUG
                cout << "\tuse [" << i.start << "," << i.end << "]" << endl;
#endif
            
#ifdef STATS
                tests++;
#endif
                
                bg::model::box<bg::model::point<float, 3, bg::cs::cartesian>> qbox(bg::model::point<float, 3, bg::cs::cartesian>(xlow, ylow, i.start), bg::model::point<float, 3, bg::cs::cartesian>(xhigh, yhigh, i.end));
                if (rtree.qbegin(bgi::intersects(qbox)) != rtree.qend())
//                for (auto it = rtree.qbegin(bgi::intersects(qbox)); it != rtree.qend(); ++it)
                {
////                    if (it->second != nid)
////                    {
//#ifdef DEBUG
//                        cout << "\t\tfound N" << it->second << endl;
//#endif
//
                        qres = true;
                        break;
////                    }
                }
            }
        qtime = tim.stop();
        timeQuerying += qtime;
        numQueries++;

        if(qres)
            numTrues++;

#ifdef STATS
        numTests += tests;
#endif

            
#ifdef DEBUG
        cout << "\t" << ((qres)? "TRUE": "FALSE") << endl;
        cout << "\t" << tests << endl;
#else
//        cout << ((qres)? "TRUE": "FALSE") << "\t";
#ifdef STATS
        printf("%lf\t%d\n", qtime/numRuns, tests);
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
    cout << "Method                    : 3DReach" << endl << endl;
    printf("R-tree builing time [secs]: %.10lf\n", timeIndexing);
    cout << "Index size [Bytes]        : " << get<6>(stats)+L.getSize() << endl << endl;
    cout << "Num of queries            : " << numQueries << endl;
    cout << "Num of runs per query     : " << numRuns << endl;
    cout << "Num of true results       : " << numTrues << endl;
    printf("Avg query time [secs]     : %.10lf\n", (timeQuerying/numRuns)/numQueries);
    cout << endl;


    return 0;
}
