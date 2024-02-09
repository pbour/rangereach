#include "graph.h"
#include <stack>
//#include <algorithm>



Node::Node()
{
    this->isSpatial = false;
//    this->indegree = 0;
//    this->outdegree = 0;
}


void Node::print()
{
    cout << "Node " << this->id << endl;
//    cout << "\tlabel             : " << this->label << endl;
    cout << "\tpost-order number : " << this->postnum << endl;
    cout << "\tgeometry          : ";

    if (this->isSpatial)
    {
#ifdef BOOST_GEOMETRIES
        auto nx = bg::get<0>(this->geometry);
        auto ny = this->geometry.get<1>();
#else
        auto nx = this->geometry.x;
        auto ny = this->geometry.y;
#endif

        cout << "(" << nx << "," << ny << ")";
    }
    cout << endl;
    
    cout << "\tin-degree         : " << this->indegree << endl;
    cout << "\tout-degree        : " << this->outdegree << endl;
}


Node::~Node()
{
}



Component::Component()
{
    this->isSpatial = false;
    this->indegree = 0;
    this->outdegree = 0;
}


void Component::print()
{
    cout << "Component " << this->id << endl;
    cout << "\tpost-order number : " << this->postnum << endl;
    cout << "\tgeometry          : ";

    if (this->isSpatial)
    {
#ifdef BOOST_GEOMETRIES
        auto cxlow = bg::get<bg::min_corner, 0>(this->geometry);
        auto cylow = bg::get<bg::min_corner, 1>(this->geometry);
        auto cxhigh = this->geometry.max_corner().get<0>();
        auto cyhigh = this->geometry.max_corner().get<1>();
        
#else
        auto cxlow = this->geometry.xlow;
        auto cylow = this->geometry.ylow;
        auto cxhigh = this->geometry.xhigh;
        auto cyhigh = this->geometry.yhigh;
#endif

        cout << "(" << cxlow  << "," << cylow  << ")->";
        cout << "(" << cxhigh << "," << cyhigh << ")";
    }
    cout << endl;

    cout << "\tnodes             :";
    for (const Node *n: this->nodes)
        cout << " " << n->id;
    cout << endl;

    cout << "\tin-degree         : " << this->indegree << endl;
    cout << "\tout-degree        : " << this->outdegree << endl;
}


Component::~Component()
{
}




GeosocialDAGraph::GeosocialDAGraph(string prefix)
{
    string fileNodes     = prefix + ".nodes";
    string fileNodeGeoms = fileNodes + ".geoms";
    string fileEdges     = prefix + ".edges";
    string fileComponents     = prefix + ".sccs";
    string fileComponentNodes = fileComponents + ".nodes";
    string fileComponentGeoms = fileComponents + ".geoms";
    string fileComponentEdges = fileComponents + ".edges";
    ifstream inp;
    ID nid, nid2, cid, cid2, postnum, dummy;
    float x, y, xhigh, yhigh;
    string label;

    
    // STEP 1: load nodes
    inp.open(fileNodes);
    if (!inp)
    {
        cerr << endl << "Error: cannot open nodes file \"" << fileNodes << "\"" << endl << endl;
        exit(1);
    }

    // Read metadata
    inp >> this->numNodes;
    
    // Read nodes
    this->nodes.resize(this->numNodes);
    while (inp >> nid >> postnum)
    {
        this->nodes[nid].id = nid;
        this->nodes[nid].postnum = postnum;
    }
    inp.close();

    
    // STEP 2: load node geometries
    inp.open(fileNodeGeoms);
    if (!inp)
    {
        cerr << endl << "Error: cannot open node geometries file \"" << fileNodeGeoms << "\"" << endl << endl;
        exit(1);
    }

    // Read metadata
    inp >> dummy;

    // Read geometries
    while (inp >> nid >> x >> y)
    {
        this->nodes[nid].isSpatial = true;
        
#ifdef BOOST_GEOMETRIES
        this->nodes[nid].geometry = bg::model::point<float, 2, bg::cs::cartesian>(x, y);
#else
        this->nodes[nid].geometry.x = x;
        this->nodes[nid].geometry.y = y;
#endif
    }
    inp.close();

    
    // STEP 3: load edges
    inp.open(fileEdges);
    if (!inp)
    {
        cerr << endl << "Error: cannot open edges file \"" << fileEdges << "\"" << endl << endl;
        exit(1);
    }

    // Read metadata
    inp >> dummy >> dummy;

    // Read edges
    while (inp >> nid >> nid2)
    {
        this->nodes[nid].outdegree++;
        this->nodes[nid2].indegree++;
    }
    inp.close();

    
    // STEP 4: load components
    inp.open(fileComponents);
    if (!inp)
    {
        cerr << endl << "Error: cannot open components file \"" << fileComponents << "\"" << endl << endl;
        exit(1);
    }

    // Read metadata
    inp >> this->numComponents;
    
    // Read components
    this->components.resize(this->numComponents);
    this->postnum2component.resize(this->numComponents);
    while (inp >> cid >> postnum)
    {
        this->components[cid].id = cid;
        this->components[cid].postnum = postnum;
        this->postnum2component[postnum] = &this->components[cid];
    }
    inp.close();

    
    // STEP 5: load component nodes
    inp.open(fileComponentNodes);
    if (!inp)
    {
        cerr << endl << "Error: cannot open component nodes file \"" << fileComponentNodes << "\"" << endl << endl;
        exit(1);
    }

    // Read metadata
    inp >> dummy >> dummy;

    // Read component - node pairs
    while (inp >> cid >> nid)
    {
        this->nodes[nid].cid = cid;
        this->components[cid].nodes.push_back(&this->nodes[nid]);
    }
    inp.close();

    
    // STEP 6: load component geometries
    inp.open(fileComponentGeoms);
    if (!inp)
    {
        cerr << endl << "Error: cannot open component geometries file \"" << fileComponentGeoms << "\"" << endl << endl;
        exit(1);
    }

    // Read metadata
    inp >> dummy;

    // Read geometries
    while (inp >> cid >> x >> y >> xhigh >> yhigh)
    {
        this->components[cid].isSpatial = true;
        this->components[cid].isPoint = ((x == xhigh) && (y = yhigh)? true: false);

#ifdef BOOST_GEOMETRIES
        this->components[cid].geometry = bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>>(bg::model::point<float, 2, bg::cs::cartesian>(x, y), bg::model::point<float, 2, bg::cs::cartesian>(xhigh, yhigh));
#else
        this->components[cid].geometry.xlow = x;
        this->components[cid].geometry.ylow = y;
        this->components[cid].geometry.xhigh = xhigh;
        this->components[cid].geometry.yhigh = yhigh;
#endif
    }
    inp.close();


    // STEP 7: load component edges
    inp.open(fileComponentEdges);
    if (!inp)
    {
        cerr << endl << "Error: cannot open component edges file \"" << fileComponentEdges << "\"" << endl << endl;
        exit(1);
    }

    // Read metadata
    inp >> dummy >> dummy;

    // Read edges
    this->cadjs.resize(this->numComponents);
    while (inp >> cid >> cid2)
    {
        this->cadjs[cid].push_back(cid2);
        this->components[cid].outdegree++;
        this->components[cid2].indegree++;
    }
    inp.close();
}


void GeosocialDAGraph::reverse()
{
    vector<AdjacencyList> tmpA(this->numComponents);
    
    for (Component &c: this->components)
    {
        auto tmp = c.indegree;
        
        c.indegree = c.outdegree;
        c.outdegree = tmp;
        for (const ID acid: this->cadjs[c.id])
            tmpA[acid].push_back(c.id);
    }
    
    this->cadjs = tmpA;
}


void GeosocialDAGraph::print()
{
    cout << endl;
    cout << "Geo-social DAG" << endl;
    cout << "================" << endl << endl;
    for (Node &n: this->nodes)
        n.print();

    cout << endl;
    for (Component &c: this->components)
    {
        c.print();
        cout << "\tdirect descendants:";
        for (ID acid: this->cadjs[c.id])
            cout << " " << acid;
        cout << endl << endl;
    }
}


GeosocialDAGraph::~GeosocialDAGraph()
{
}
