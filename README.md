# Fast Geosocial Reachability Queries

Source code for the "Fast Geosocial Reachability Queries" paper. For the GeoReach method, the code can be found in the authors repository https://github.com/DataSystemsLab/GeoGraphDB--Neo4j.

<p align="justify">
The proliferation of location-based services and social networks has given rise to <em>geosocial networks</em>, which model not only the social interactions between users but also their spatial activities. We study the efficient computation of a recently proposed query for geosocial networks called <em>Geosocial Reachability</em> query (RangeReach), which comes as a hybrid of the traditional spatial selection (range) query and the graph reachability problem. Intuitively, given a geosocial network <em>G</em>, a vertex <em>v</em>, and a spatial region <em>R</em>, RangeReach(<em>G,v,R</em>) determines whether <em>v</em> can reach any vertex in <em>G</em> with spatial activity inside <em>R</em>. We consider an interval-based labeling scheme proposed in the past for graph reachability to devise two novel solutions for RangeReach. The first takes a social-first approach, prioritizing the graph reachability predicate.  The second treats both predicates at the same time by transforming the problem of answering RangeReach queries into queries over a three-dimensional space that models the spatial and interval-based reachability information in the geosocial network. Our experimental analysis compares our proposed solutions against a baseline spatial-first approach powered by spatial indexing and a graph reachability technique, as well as the state-of-the-art method for RangeReach queries.</p>
<figure>
  <img src="/figures/running+query3.png" width="400" alt="RangeReach example" />
</figure>

## Dependencies
- g++/gcc
- Boost Library 

## Compile
Compile the source code using ```make all``` or simple ```make```. 

## Setting up
- Download the archives for the input files from https://seafile.rlp.net/d/6fa3703e57234b6d9831/
- Extract inside the inputs directory


## Creates
Tha archives already contain the files for the directed acyclic graph of the geosocial networks, the interval-base labeling scheme (original nad reverse) and a .bfl necesary for contructing the BFL scheme. Nevertheless, the following commands will create the above files from scratch. We use the ```yelp``` dataset to exemplify the commands.

### Directed Acyclic Graph
```
$ ./scripts/create_dag.sh inputs/yelp/yelp
```

### Special input file for the BFL schemes
```
$ ./create_bfl.sh inputs/yelp/yelp
```

### Interval-based labeling scheme
```
$ ./create_int.sh inputs/yelp/yelp
```

### Reverse interval-based labeling scheme 
```
$ ./create_dag.sh inputs/yelp/yelp reverse
```


## Runs
The following commands execute the evaluation methods over a query file. For the SpaReach and the 3DReach methods the creation of the 2D R-tree and the 3D R-tree is included, respectively. For the SpaReach-BFL, the creation of the BFL scheme is also included. We use again the ```yelp``` dataset to exemplify the commands.

### SpaReach-INT
Prerequisites: [DAG](#directed-acyclic-graph) and [interval-based labeling scheme](#interval-based-labeling-scheme)

Non-MBR variant
```
$ ./run_spareach_int.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

MBR variant
```
$ ./run_spareach-int_MBR.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

### SpaReach-BFL
Prerequisites: [DAG](#directed-acyclic-graph) and [.bfl input file](#interval-based-labeling-scheme)
```
$ ./run_spareach-bfl.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

### SocReach
Prerequisites: [DAG](#directed-acyclic-graph) and [interval-based labeling scheme](#interval-based-labeling-scheme)
```
$ ./run_socreach.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

### 3DReach
Prerequisites: [DAG](#directed-acyclic-graph) and [interval-based labeling scheme](#interval-based-labeling-scheme)
```
$ ./run_3dreach.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

### 3DReach-Rev
Prerequisites: [DAG](#directed-acyclic-graph) and [reverse interval-based labeling scheme](#reverse-interval-based-labeling-scheme)
```
$ ./run_3dreach_rev.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```



## Experiments
Instructions on how to execute the experiments included in the paper.

### Indexing costs
The index size can be found in the execute queries report of every method.

The index time can be calculated for each method, as follows:
- SpaReach-INT, the [create time for DAG](#directed-acyclic-graph), plus the [create time for the interval-based labeling](#interval-based-labeling-scheme), plus the 2D R-tree building time reported when executing a query file.  
- SpaReach-BFL, the [create time for DAG](#directed-acyclic-graph), plus the building time for the BFL scheme and the 2D R-tree building time both reported when executing a query file.
- SocReach, the [create time for DAG](#directed-acyclic-graph) plus the create time for interval-based labeling.  
- 3DReach, the [create time for DAG](#directed-acyclic-graph), plus the [create time for the interval-based labeling](#interval-based-labeling-scheme), plus the 3D R-tree building time reported when executing a query file.  
- 3DReach, the [create time for DAG](#directed-acyclic-graph), plus the [create time for the reverse interval-based labeling](#reverse-interval-based-labeling-scheme), plus the 3D R-tree building time reported when executing a query file.  
  

### Query performance
Execute the following scripts; output files are automatically created in the ```outputs``` directory.

#### Vary region extent
```
$ ./scripts/run_vary-region.sh
```

#### Vary degree 
```
$ ./scripts/run_vary-degree.sh
```

#### Vary spatial selectivity  
```
$ ./scripts/run_vary-selectivity.sh
```
