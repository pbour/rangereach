#include "../def_global.h"



typedef boost::adjacency_list<
boost::vecS,
boost::vecS,
boost::directedS> Graph;


Graph load_network(const string &graph_file) {
    Graph g;

    ifstream inp;
    inp.open(graph_file);
    if (!inp) {
        cerr << endl << "Error: cannot open nodes file \"" << graph_file << "\"" << endl << endl;
        exit(1);
    }

    int numNodes;
    inp >> numNodes;
    string line;

    while (inp >> line) {
        vector<string> result;
        boost::split(result, line, boost::is_any_of(","));

        unsigned int nid = stoi(result[0]);

        for (unsigned int i = 2; i < boost::size(result); i++) {
            unsigned int trg = stoi(result[i]);
            boost::add_edge(vertex(nid, g), vertex(trg, g), g);
        }
    }

    inp.close();

    return g;
}

pair<vector<unsigned int>,int> strongly_connected_components(Graph &g) {
    vector<unsigned int> c(boost::num_vertices(g));
    auto comp_map = make_iterator_property_map(c.begin(), get(boost::vertex_index, g));
    int num_clusters = boost::strong_components(g, comp_map);
    return make_pair(c,num_clusters);
}


int main([[maybe_unused]] int argc, char** argv) {

    if(argc != 3) {
        cout << "[ERROR] Wrong number of arguments" << endl;
    }

    string network_file = argv[1];
    string scc_file = argv[2];

    cout << "[LOG] Input network file: " << network_file << endl;
    cout << "[LOG] Output file: " << scc_file << endl;

    Graph g = load_network(network_file);

    cout << "[LOG] Number of nodes: " << boost::num_vertices(g) << endl;
    cout << "[LOG] Number of edges: " << boost::num_edges(g) << endl;

    vector<unsigned int> c = strongly_connected_components(g).first;

    ofstream my_file (scc_file);
    if (my_file.is_open()) {
        my_file << boost::num_vertices(g) << endl;
        for (size_t id = 0; id < c.size(); ++id) {
            my_file << id << "," << c.at(id) << endl;
        }
        my_file.close();
    }
    else
        cout << "[ERROR] Unable to open output file";
    cout << "[LOG] Writing components to file completed." << endl;
    return 0;
}
