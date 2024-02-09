# Fast Geosocial Reachability Queries

Source code for the "Fast Geosocial Reachability Queries" paper. For the GeoReach method, the code can be found in the authors repository https://github.com/DataSystemsLab/GeoGraphDB--Neo4j.

## Dependencies
- g++/gcc
- Boost Library 

## Compile
Compile the source code using ```make all``` or simple ```make```. 

## Setting up
- Download the archives for the input files and extract them inside the inputs directory, foursquare from https://seafile.rlp.net/f/7fc3d8ed95f1452497ba/?dl=1, yelp from https://seafile.rlp.net/f/1e57c936aeff413a9ebd/?dl=1
- Download the query files and place them inside the queries directory


## Creates
The following commands create a directed acyclic graph from a input geosocial network, reachability labeling schemes or necessary input files to them. We use the ```yelp``` dataset as example.

### Directed Acyclic Graph
```
$ ./scripts/create_dag.sh inputs/yelp
```

### Create special input file for the BFL schemes
```
$ ./create_bfl.sh inputs/yelp
```

### Create interval-based labeling scheme
```
$ ./create_int.sh inputs/yelp
```

### Create reverse interval-based labeling scheme 
```
$ ./create_dag.sh inputs/yelp reverse
```


## Runs
The following commands execute an evaluation method over a query file. For the SpaReach and the 3DReach methods the creation of the 2D R-tree and the 3D R-tree is included, respectively. For the SpaReach-BFL, the create of the BFL labeling scheme is also included.  

### SpaReach-INT

