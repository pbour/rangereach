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
The following commands create a directed acyclic graph from a input geosocial network, reachability labeling schemes or necessary input files to them

### Directed Acyclic Graph
```
$ ./create_dag.sh inputs/yelp
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
$ ./create_dag.sh inputs/yelp
```


## Run, includes the creatt

Interval-based Labaeling Scheme

## SpaReach-INT

### Variant using the Interval Labeling scheme

#### Create
```
$ ./spafirst-ints_strict.exec inputs/yelp queries/yelp-0.01-0.qry
```

```
$ ./spafirst-ints_MBR_strict.exec inputs/yelp queries/yelp-0.01-0.qry
```

### Variant using the BFL scheme

#### Strict
```
$ ./spafirst-bfl_strict.exec inputs/yelp queries/yelp-0.01-0.qry
```

```
$ ./spafirst-bfl_MBR_strict.exec inputs/yelp queries/yelp-0.01-0.qry
```


## Running SocReach

#### Strict
```
$ ./socfirst_strict.exec inputs/yelp queries/yelp-0.01-0.qry
```


## Running 3DReach

### Normal variant

```
$ ./3dreach.exec inputs/yelp queries/yelp-0.01-0.qry
```

### Reverse variant
```
$ ./3dreach_rev.exec inputs/yelp queries/yelp-0.01-0.qry
```
