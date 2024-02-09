#!/bin/bash


echo
echo "Convert files for the $1 dataset"
echo
echo "Create .edges file ..."
numNodes=$(head -n 1 $1-graph.txt | awk '{print $1}')
awk -F',' '{if ($2 > 0) for (i=3; i <= NF; i++) print $1" "$i}' $1-graph.txt > $1-graph.txt.tmp
numEdges=$(wc -l $1-graph.txt.tmp | awk '{print $1}')
echo "$numNodes $numEdges" > $1.edges
cat $1-graph.txt.tmp >> $1.edges
rm -rf $1-graph.txt.tmp
echo "    done"

echo
echo "Create .nodes.geoms file ..."
awk -F',' '{if ($2 == 1) print $1" "$3" "$4}' $1-entity.txt > $1-entity.txt.tmp
numGeoms=$(wc -l $1-entity.txt.tmp | awk '{print $1}')
echo "$numGeoms" > $1.nodes.geoms
cat $1-entity.txt.tmp >> $1.nodes.geoms
rm -rf $1-entity.txt.tmp
echo "    done"

echo
echo "Create .sccs.nodes file ..."
awk -F',' '{if (NF > 1) print $2" "$1}' $1-scc.txt > $1-scc.txt.tmp
numSSCs=$(awk '{print $1}' $1-scc.txt.tmp | sort -u | wc -l | awk '{print $1}')
echo "$numSSCs $numNodes" > $1.sccs.nodes
cat $1-scc.txt.tmp >> $1.sccs.nodes
rm -rf $1-scc.txt.tmp
echo "    done"
echo
