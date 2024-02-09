#include "../def_global.h"
#include "../containers/graph.h"



//#define DEBUG
//#define STATS


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << endl;
        cerr << "Usage: " << argv[0] << " INPUT_PREFIX" << endl;
        cerr << endl;
        
        return 1;
    }
    string prefix   = string(argv[1]);
    string ofileBFL = prefix + ".sccs.bfl";
    ofstream out;
    
    
    // Step 1: load graph
    GeosocialDAGraph G(prefix);
    //    G.print();
    

    // Step 2: export components adjacency lists
    out.open(ofileBFL, ofstream::out);
    out << "graph_for_greach" << endl;
    out << G.numComponents << endl;
    for (Component &c: G.components)
    {
        out << c.id << ":";
        for (ID acid: G.cadjs[c.id])
            out << " " << acid;
        if (G.cadjs[c.id].size() == 0)
            out << " ";
        out << "#" << endl;
    }
    out.close();
    

    return 0;
}
