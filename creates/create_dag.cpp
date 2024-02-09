#include "../def_global.h"
#include "../containers/graph.h"
#include "../containers/labeling.h"
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <cmath>
using boost::make_iterator_range;



bool contains(vector<ID> &adj, ID qid)
{
    for (const ID id: adj)
    {
        if (id == qid)
            return true;
    }
    
    return false;
}


void postorder(vector<Component> &components, vector<vector<ID>> &cadjs, Component &c, bool *V, ID &postnum)
{
//    cout << "\tpostorder(" << c.id << ")" << endl;
    V[c.id] = true;
    for (const ID acid: cadjs[c.id])
    {
        if (!V[acid])
            postorder(components, cadjs, components[acid], V, postnum);
    }
//    cout << "\tvisited " << c.id << endl;
    c.postnum = postnum;
    postnum++;
}


void computePostorderNumbers(vector<Component> &components, vector<vector<ID>> &cadjs)
{
    ID postnum = 0, rindex = 0;
    auto numComponents = components.size();
    vector<Component> tmpComponents = components;
    bool *V = new bool[numComponents];
    

    memset(V, 0, numComponents*sizeof(bool));
    
    // STEP 1: sort nodes by in-degree, to determine roots of traversal
    sort(tmpComponents.begin(), tmpComponents.end(),
         [](const Component &lhd, const Component &rhd) -> bool
         {
            if (lhd.indegree != rhd.indegree)
                return lhd.indegree < rhd.indegree;
            else
                return lhd.id < rhd.id;
         });

    // STEP 2: traverse in post-order
    while (postnum < numComponents)
    {
        // Select a root
        Component &root = components[tmpComponents[rindex].id];
     
//        cout << "\troot: " << root.id << endl;
        
        // Traverse graph
        postorder(components, cadjs, root, V, postnum);
        
        // Prepare for next root, if needed
        rindex++;
    }
    tmpComponents.clear();
    delete[] V;
}


void postorderN(vector<Node> &nodes, vector<vector<ID>> &adjs, Node &n, bool *V, ID &postnum)
{
    cout << "\tpostorder(" << n.id << ")" << endl;
    V[n.id] = true;
    for (const ID anid: adjs[n.id])
    {
        if (!V[anid])
            postorderN(nodes, adjs, nodes[anid], V, postnum);
    }
    cout << "\tvisited " << n.id << endl;
    n.postnum = postnum;
    postnum++;
}


void computePostorderNumbersN(vector<Node> &nodes, vector<vector<ID>> &adjs)
{
    ID postnum = 0, rindex = 0;
    auto numNodes = nodes.size();
    vector<Node> tmpNodes = nodes;
    bool V[numNodes];


    memset(V, 0, numNodes*sizeof(bool));
    
    // STEP 1: sort nodes by in-degree, to determine roots of traversal
    sort(tmpNodes.begin(), tmpNodes.end(),
         [](const Node &lhd, const Node &rhd) -> bool
         {
            if (lhd.indegree != rhd.indegree)
                return lhd.indegree < rhd.indegree;
            else
                return lhd.id < rhd.id;
         });
    

    // STEP 2: traverse in post-order
    while (postnum < numNodes)
    {
        // Select a root
        Node &root = nodes[tmpNodes[rindex].id];
     
        cout << "\troot: " << root.id << endl;
        
        // Traverse graph
        postorderN(nodes, adjs, root, V, postnum);
        
        // Prepare for next root, if needed
        rindex++;
    }
    tmpNodes.clear();
}



void tarjan(const ID nid, const vector<vector<ID>> &adjs, ID &gindex, vector<ID> &index, vector<ID> &lowlink, stack<ID> &S, bool *onstack, vector<Node> &nodes, vector<Component> &components, vector<ID> &node2scc)
{
//    cout<<"N"<<nid<<endl;//getchar();
    index[nid] = gindex;
    lowlink[nid] = gindex;
    gindex++;
    
    S.push(nid);
    onstack[nid] = true;
    
    for (const ID anid: adjs[nid])
    {
//        if (nid == 1224830)
//        {
//            cout << "anid\t\t = "<<anid;getchar();
//        }
        if (index[anid] == -1)
        {
            tarjan(anid, adjs, gindex, index, lowlink, S, onstack, nodes, components, node2scc);
            lowlink[nid] = min(lowlink[nid], lowlink[anid]);
        }
        else if (onstack[anid])
        {
            lowlink[nid] = min(lowlink[nid], index[anid]);
        }
    }
    
    
    if (lowlink[nid] == index[nid])
    {
        ID w;
        Component scc;

        scc.id = components.size();
        do
        {
            w = S.top();
            S.pop();
            onstack[w] = false;
            node2scc[w] = scc.id;
            
            if (nodes[w].isSpatial)
                scc.isSpatial = true;

            scc.nodes.push_back(&nodes[w]);
        }
        while (nid != w);
            
        components.push_back(scc);
        
        
        //        nodes[nid].id = nid;
        //        node2scc[nid] = cid;
        //        components[cid].id = cid;
        //        components[cid].nodes.push_back(&nodes[nid]);
        //
        //        if (nodes[nid].isSpatial)
        //            components[cid].isSpatial = true;

    }
//    cout<<"\tok"<<endl;//getchar();

}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << endl;
        cerr << "Usage: " << argv[0] << " INPUT_PREFIX" << endl;
        cerr << endl;

        return 1;
    }
    string prefix = string(argv[1]);
    string ifileEdges = prefix + ".edges";
    string ifileGeoms = prefix + ".nodes.geoms";
    string ofileNodes = prefix + ".nodes";
    string ofileComponents     = prefix + ".sccs";
    string ifileComponentNodes = ofileComponents + ".nodes";
    string ofileComponentGeoms = ofileComponents + ".geoms";
    string ofileComponentEdges = ofileComponents + ".edges";
    ID dummy, numNodes = 0, numComponents = 0, numComponentEdges = 0, numSpatialComponents = 0, nid, nid2, cid, cid2;
    size_t numEdges = 0, counter = 0, step = 0;
    vector<Node> nodes;
    vector<ID> node2scc;
    vector<Component> components;
    ifstream inp;
    ofstream outC, outN, outG, outE;
    Timer tim;
    double timeDAG;
    vector<vector<ID>> adjs, cadjs, sccs;
    vector<unordered_set<ID>> hcadjs;
    float x,y;
//    unordered_multimap<ID, ID> check;
    
    
    // STEP 1: load original graph
    cout << endl << "Load geosocial graph ..." << endl;

    // Load graph edges
    inp.open(ifileEdges);
    if (!inp)
    {
        cerr << endl << "Error: cannot open edges file \"" << ifileEdges << "\"" << endl << endl;

        return 1;
    }
    
    // Read metadata from file
    inp >> numNodes >> numEdges;
    
    // Read edges from dile
    nodes.resize(numNodes);
    adjs.resize(numNodes);
    node2scc.resize(numNodes);
    while (inp >> nid >> nid2)
    {
        adjs[nid].push_back(nid2);
        nodes[nid].id = nid;
        nodes[nid2].id = nid2;
        nodes[nid].outdegree++;
        nodes[nid2].indegree++;
    }
    inp.close();
    

    // Load geometries
    inp.open(ifileGeoms);
    if (!inp)
    {
        cerr << endl << "Error: cannot open edges file \"" << ifileGeoms << "\"" << endl << endl;

        return 1;
    }

    // Read metadata
    inp >> dummy;

    // Read geometries from file
    while (inp >> nid >> x >> y)
    {
#ifdef BOOST_GEOMETRIES
#else
        nodes[nid].geometry.x = x;
        nodes[nid].geometry.y = y;
#endif

        nodes[nid].isSpatial = true;
    }
    inp.close();
    cout << "\tdone" << endl;
    
    
//    computePostorderNumbersN(nodes, adjs);
//    return 1;


    // STEP 2: create dag
    cout << endl << "Compute geosocial DA graph ..." << endl;

    // Load strongly connected components
    inp.open(ifileComponentNodes);
    if (!inp)
    {
        cerr << endl << "Error: cannot open components nodes file \"" << ifileComponentNodes << "\"" << endl << endl;

        return 1;
    }

    // Read metadata
    inp >> numComponents >> dummy;

    // Read geometries from file
    components.resize(numComponents);
    while (inp >> cid >> nid)
    {
        node2scc[nid] = cid;
        components[cid].nodes.push_back(&nodes[nid]);
        components[cid].id = cid;
        if (nodes[nid].isSpatial)
        {
            components[cid].isSpatial = true;
        }
    }
    inp.close();
    
    if (components.size() != numComponents)
    {
        cerr << endl << "Error: components.size() = " << components.size() << " while numComponents = " << numComponents << endl;
        return 1;
    }

    // Determine component edges
    cadjs.resize(components.size());
    hcadjs.resize(components.size());
    for (ID nid = 0; nid < numNodes; nid++)
    {
        for (const ID anid: adjs[nid])
        {
            cid = node2scc[nid];
            cid2 = node2scc[anid];
//            if ((cid != cid2) && (!contains(cadjs[cid], cid2)))
            if ((cid != cid2) && (hcadjs[cid].find(cid2) == hcadjs[cid].end()))
            {
                cadjs[cid].push_back(cid2);
                hcadjs[cid].insert(cid2);
                components[cid].outdegree++;
                components[cid2].indegree++;
                numComponentEdges++;
//                cout << "N" << nid << " -> N" << anid << " BECOMES C" << cid << " -> C" << cid2 << endl;
            }
        }
//        else
//            cout << endl;
    }

    for (Component &c: components)
    {
        if (c.isSpatial)
        {
            numSpatialComponents++;

            auto csize = c.nodes.size();
            float xlow = c.nodes[0]->geometry.x, ylow = c.nodes[0]->geometry.y, xhigh = c.nodes[0]->geometry.x, yhigh = c.nodes[0]->geometry.y;

            for (auto i = 1; i < csize; i++)
            {
                Node *n = c.nodes[i];

                if (n->isSpatial)
                {
#ifdef BOOST_GEOMETRIES
#else
                    xlow = min(xlow, n->geometry.x);
                    ylow = min(ylow, n->geometry.y);
                    xhigh = max(xhigh, n->geometry.x);
                    yhigh = max(yhigh, n->geometry.y);
#endif
                }
            }

#ifdef BOOST_GEOMETRIES
#else
            c.geometry.xlow = xlow;
            c.geometry.ylow = ylow;
            c.geometry.xhigh = xhigh;
            c.geometry.yhigh = yhigh;
#endif
        }
    }
    timeDAG = tim.stop();
    cout << "\tdone" << endl;

    
    // STEP 3: Write new files
    cout << endl << "Write new files ..." << endl;

    // Traverse the DAG in postorder fashion
    computePostorderNumbers(components, cadjs);

    // Write new nodes file, with postorder numbers
    outN.open(ofileNodes, ofstream::out);
    outN << numNodes << endl;
    for (nid = 0; nid < numNodes; nid++)
        outN << nid << " " << components[node2scc[nid]].postnum << endl;
    outN.close();

    // Write component files
    outC.open(ofileComponents, ofstream::out);
    outG.open(ofileComponentGeoms, ofstream::out);
    outE.open(ofileComponentEdges, ofstream::out);

    // Write metadata
    outC << numComponents << endl;
    outG << numSpatialComponents << endl; // TODO
    outE << numComponents << " " << numComponentEdges << endl;
    for (const Component &c: components)
    {
        outC << c.id << " " << c.postnum << endl;

        if (c.isSpatial)
            outG << setprecision(20) << c.id << " " << c.geometry.xlow << " " << c.geometry.ylow << " " << c.geometry.xhigh << " " << c.geometry.yhigh << endl;

        for (auto acid: cadjs[c.id])
            outE << c.id << " " << acid << endl;
    }
    outC.close();
    outG.close();
    outE.close();
    cout << "\tdone" << endl << endl;

    printf("Time elapsed for DAG files [secs]: %lf\n", timeDAG);


    return 0;
    
//
//    // STEP 3: write component nodes
//    cout << endl << "Write components ..." << endl;
//
//    out.open(ofileComponentNodes, ofstream::out);
//    out << numComponents << " " << numNodes << endl;
//    for (const Component &c: components)
//    {
//        for (const Node *n: c.nodes)
//            out << c.id << " " << n->id << endl;
//    }
//    out.close();
//    cout << "\tdone" << endl;
//
//
//    // STEP 4: write components
//    cout << endl << "Write components ..." << endl;
//    for (const Component &c: components)
//    {
//        for (const Node *n: c.nodes)
//        {
//            out << c.id << " " << n->id << endl;
//        }
//    }
//
//
//    // STEP 5: write component edges
//    cout << endl << "Write component edges ..." << endl;
//
//    // Write component edges to file
//    out.open(ofileComponentEdges, ofstream::out);
//    out << numComponents << " " << numEdges << endl;
//    for (auto cid = 0; cid < numComponents; cid++)
//    {
//        for (auto acid: cadjs[cid])
//            out << cid << " " << acid << endl;
//    }
//    out.close();
//    cout << "\tdone" << endl;
//
//
//
//
//    return 0;
//    
//    cout << endl << "Load geosocial base graph ..." << endl;
//    inp.open(fileBaseNodes);
//    if (!inp)
//    {
//        cerr << endl << "Error: cannot open base nodes file \"" << fileBaseNodes << "\"" << endl << endl;
//        exit(1);
//    }
//    
//    // Read metadata
//    inp >> numNodes;
//    
//    // Read nodes
//    nodes.resize(numNodes);
//
//    while (inp >> nid >> label)
//    {
//        nodes[nid].id = nid;
//        nodes[nid].label = label;
//        nodes[nid].postnum = 0;
//    }
//    inp.close();
//    
//
//    // Add edges to boost graph
//    for (const Node &n: G.nodes)
//    {
//        for (const NodeId anid: G.adjs[n.id])
//            
//    }
//    cout << "\tdone" << endl;
//
//    
//    // STEP 2: compute strongly connected components
//    cout << endl << "Compute strongly connnected components ..." << endl;
////    tim.start();
//    vector<int> c(num_vertices(bG));
//    numComponents = strong_components(bG, make_iterator_property_map(c.begin(), idmap, c[0]));
////    timeSCC = tim.stop();
//    //auto l = get(vertex_index, G);
//    bG.clear();
//    cout << "\tdone" << endl;
//    
////    cout << "Total number of components: " << numComponents << endl;
//
//    
//    // STEP 3: write dag's nodes file
//    cout << endl << "Write geosocial DAG's nodes file ..." << endl;
//    vector<NodeId> node2scc(G.numNodes);
//    for (auto i = c.begin(); i != c.end(); ++i)
//        node2scc[i - c.begin()] = *i;
//    
//    ofstream out(fileNodes, ofstream::out);
//    out << G.numNodes << endl;
//    for (const Node &n: G.nodes)
//        out << n.id << " " << n.label << " " << node2scc[n.id] << endl;
//    out.close();
//    cout << "\tdone" << endl;
//
//    
//    // STEP 4: write dag's edges file
////    for (const Node &n: G.nodes)
////    {
////
////        for (const NodeId anid: G.adjs[n.id])
////            boost::add_edge(vertex(n.id, bG), vertex(anid, bG), bG);
////    }
//
////    boost::graph_traits<Graph>::edge_iterator ei, ei_end;
////    for (tie(ei, ei_end) = edges(bG); ei != ei_end; ++ei)
////    {
//////        cout << idmap[source(*ei, G)] << " " << idmap[target(*ei, G)] << endl;
////        nid1 = idmap[source(*ei, bG)];
////        nid2 = idmap[target(*ei, bG)];
////        if (node2scc[nid1] != node2scc[nid2])
////            cout << node2scc[nid1] << " " << node2scc[nid2] << endl;
////    }

    return 0;
}
