#include "../def_global.h"



typedef vector<ID>  AdjacencyList;

struct Point
{
    float x, y;
};


struct MBR
{
    float xlow, ylow, xhigh, yhigh;
};



class Node
{
public:
    ID id;
    ID postnum;
    ID cid;
    ID indegree, outdegree;
    bool isSpatial;
    
#ifdef BOOST_GEOMETRIES
    bg::model::point<float, 2, bg::cs::cartesian> geometry;
#else
    Point geometry;
#endif
    
    Node();
    void print();
    ~Node();
};


class Component
{
public:
    ID id;
    ID postnum;
    bool isSpatial;
    bool isPoint;
    ID indegree, outdegree;
    vector<Node*> nodes;
    
#ifdef BOOST_GEOMETRIES
    bg::model::box<bg::model::point<float, 2, bg::cs::cartesian>> geometry;
#else
    MBR geometry;
#endif

    Component();
    void print();
    ~Component();
};



class GeosocialDAGraph
{
public:
    vector<Node> nodes;
    vector<Component> components;
    vector<AdjacencyList> cadjs;
    vector<Component*> postnum2component;
    
    ID numNodes;
    ID numComponents;
//    ID numGeoms;
    
    GeosocialDAGraph(string prefix);
    void reverse();
    void print();
    ~GeosocialDAGraph();
};
