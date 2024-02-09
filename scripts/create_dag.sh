#!/bin/bash


echo
echo "Compute strong connected components ..."
./create_scc.exec $1

echo
bash scripts/convert.sh $1

echo
echo "Create DAG files"
./create_dag.exec $1
echo
