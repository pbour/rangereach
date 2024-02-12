CC      = g++
CFLAGS  = -O3 -mavx -march=native -std=c++14 -w
LDFLAGS =

SOURCES = containers/graph.cpp containers/labeling.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: creates qpoints qmbrs
creates: create_scc create_dag create_bfl create_int
qpoints: spareach_int spareach_bfl socreach 3dreach 3dreach_rev #baseline
qmbrs: spareach_int_mbr #spafirst_ints_mbr_strict spafirst_bfl_mbr_strict 3dreach_mbr 3dreach_rev_mbr
	
create_scc: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) creates/create_scc.cpp -o create_scc.exec $(LDADD)

create_dag: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o creates/create_dag.cpp -o create_dag.exec $(LDADD)

create_bfl: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o creates/create_bfl_input.cpp -o create_bfl_input.exec $(LDADD)

create_int: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o containers/labeling.o creates/create_int.cpp -o create_int.exec $(LDADD)


spareach_int: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o containers/labeling.o methods/main_spareach-int.cpp -o run_spareach-int.exec $(LDADD)

spareach_int_mbr: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o containers/labeling.o methods/main_spareach-int_MBR.cpp -o run_spareach-int_MBR.exec $(LDADD)

spareach_bfl: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o containers/labeling.o methods/main_spareach-bfl.cpp -o run_spareach-bfl.exec $(LDADD)

#baseline: $(OBJECTS)
#	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o methods/main_baseline.cpp -o run_baseline.exec $(LDADD)
#
socreach: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o containers/labeling.o methods/main_socreach.cpp -o run_socreach.exec $(LDADD)

3dreach: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o containers/labeling.o methods/main_3dreach.cpp -o run_3dreach.exec $(LDADD)

3dreach_rev: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) containers/graph.o containers/labeling.o methods/main_3dreach_rev.cpp -o run_3dreach_rev.exec $(LDADD)


.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf containers/*.o
	rm -rf create_*.exec
	rm -rf run_*.exec

