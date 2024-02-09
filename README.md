# Fast Geosocial Reachability Queries

Source code for the "Fast Geosocial Reachability Queries" paper. For the GeoReach method, the code can be found in the authors repository https://github.com/DataSystemsLab/GeoGraphDB--Neo4j.

## Dependencies
- g++/gcc
- Boost Library 

## Compile
Compile the source code using ```make all``` or simple ```make```. 

## Setting up
- Download the archives for the input files from  
- Extract inside the inputs directory


## Creates
The following commands create a directed acyclic graph from a input geosocial network, reachability labeling schemes or necessary input files to them. We use the ```yelp``` dataset as example.

### Directed Acyclic Graph
```
$ ./scripts/create_dag.sh inputs/yelp
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
The following commands execute an evaluation method over a query file. For the SpaReach and the 3DReach methods the creation of the 2D R-tree and the 3D R-tree is included, respectively. For the SpaReach-BFL, the create of the BFL labeling scheme is also included. We use the ```yelp``` dataset as example.

### SpaReach-INT
Non-MBR variant
```
$ ./run_spareach_int.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

MBR variant
```
$ ./run_spareach-int_MBR.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

### SpaReach-BFL
```
$ ./run_spareach-bfl.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

### SocReach
```
$ ./run_socreach.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

### 3DReach
```
$ ./run_3dreach.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```

### 3DReach-Rev
```
$ ./run_3dreach_rev.exec inputs/yelp/yelp queries/yelp/queries-range-degree.0.1-2.txt.qry
```



## Experiments

To execute the experiments included in the paper, run the following scripts:

### Vary region extent
```
$ ./scripts/run_vary-region.sh
```

### Vary degree 
```
$ ./scripts/run_vary-degree.sh
```

### Vary spatial selectivity  
```
$ ./scripts/run_vary-selectivity.sh
```
