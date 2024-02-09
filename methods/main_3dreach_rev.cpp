#include "../def_global.h"
#include "../containers/graph.h"
#include "../containers/labeling.h"
#include "../containers/rtree_statistics.hpp"



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

    
    // Step 1: load graph
    GeosocialDAGraph G(prefix);
//    G.print();
        

    // STEP 2: load interval scheme
    Labeling L(prefix, true);
//    L.print();
    

    // STEP 3: build 3D R-tree
    tim.start();
#ifdef RTREE_BULKLOAD
    vector<pair<bg::model::segment<bg::model::point<float, 3, bg::cs::cartesian>>, ID>> cloud;
    
    for (const Node &n: G.nodes)
    {
        if (n.isSpatial)
        {
            for (const Interval &i: L.intervals[n.postnum])
                cloud.push_back(make_pair(bg::model::segment<bg::model::point<float, 3, bg::cs::cartesian>>(bg::model::point<float, 3, bg::cs::cartesian>(n.geometry.x, n.geometry.y, float(i.start)), bg::model::point<float, 3, bg::cs::cartesian>(n.geometry.x, n.geometry.y, float(i.end))), n.id));
        }
    }

    bgi::rtree<pair<bg::model::segment<bg::model::point<float, 3, bg::cs::cartesian>>, ID>, bgi::quadratic<16>> rtree(cloud);
#else
    bgi::rtree<pair<bg::model::segment<bg::model::point<float, 3, bg::cs::cartesian>>, ID>, bgi:quadraticrstar<16>> rtree;

    for (const Node &n: G.nodes)
    {
        if (n.isSpatial)
        {
            for (const Interval &i: L.intervals[n.postnum])
            {
#ifdef BOOST_GEOMETRIES
                rtree.insert(make_pair(bg::model::segment<bg::model::point<float, 3, bg::cs::cartesian>>(bg::model::point<float, 3, bg::cs::cartesian>(bg::get<0>(n.geometry), n.geometry.get<1>(), float(i.start)), bg::model::point<float, 3, bg::cs::cartesian>(bg::get<0>(n.geometry), n.geometry.get<1>(), float(i.end))), n.id));
#else
                rtree.insert(make_pair(bg::model::segment<bg::model::point<float, 3, bg::cs::cartesian>>(bg::model::point<float, 3, bg::cs::cartesian>(n.geometry.x, n.geometry.y, float(i.start)), bg::model::point<float, 3, bg::cs::cartesian>(n.geometry.x, n.geometry.y, float(i.end))), n.id));
#endif
            }
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
//    cout << "query-node\tquery-area\tquery-node-out-degree\tresult\tquery-time" << endl;
#endif
    
    while (inp >> nid >> xlow >> ylow >> xhigh >> yhigh)
    {
#ifdef DEBUG
        cout << "Q: N" << nid << " (" << xlow << ", " << ylow << ") -> (" << xhigh << ", " << yhigh << ")" << endl;
#else
//        cout << nid << "\t(" << xlow << "," << ylow << ")->(" << xhigh << "," << yhigh << ")\t" << G.nodes[nid].outdegree << "\t";
#endif

        bool qres = false;
        ID qpostnum = G.nodes[nid].postnum;

        // Execute range queries
        tim.start();
            bg::model::box<bg::model::point<float, 3, bg::cs::cartesian>> qplane(bg::model::point<float, 3, bg::cs::cartesian>(xlow, ylow, qpostnum), bg::model::point<float, 3, bg::cs::cartesian>(xhigh, yhigh, qpostnum));
            if (rtree.qbegin(bgi::intersects(qplane)) != rtree.qend())
//            for (auto it = rtree.qbegin(bgi::intersects(qplane)); it != rtree.qend(); ++it)
//            {
////                if (it->second != nid)
////                {
//#ifdef DEBUG
//                    cout << "\t\tfound N" << it->second << endl;
//#endif
//
                    qres = true;
//                    break;
////                }
//            }
        qtime = tim.stop();
        timeQuerying += qtime;
        numQueries++;

        if(qres)
            numTrues++;

#ifdef DEBUG
        cout << "\t" << ((qres)? "TRUE": "FALSE") << endl << endl;
#else
//        cout << ((qres)? "TRUE": "FALSE") << "\t";
//        printf("%lf\n", qtime/numRuns);
#endif
    }
    inp.close();


    std::tuple<std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t> stats = bgi::detail::rtree::utilities::statistics(rtree);

    cout << endl;
    cout << "Report" << endl;
    cout << "======" << endl << endl;
    cout << "Input prefix              : " << prefix << endl;
    cout << "Query file                : " << fileQueries << endl;
    cout << "Method                    : 3DReach_rev" << endl << endl;
    printf("R-tree builing time [secs]: %.10lf\n", timeIndexing);
    cout << "Index size [Bytes]        : " << get<6>(stats) << endl << endl;
    cout << "Num of queries            : " << numQueries << endl;
    cout << "Num of runs per query     : " << numRuns << endl;
    cout << "Num of true results       : " << numTrues << endl;
    printf("Avg query time [secs]     : %.10lf\n", (timeQuerying/numRuns)/numQueries);
    cout << endl;


    return 0;
}
