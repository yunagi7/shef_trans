#***************************************************************************
#
#  Makefile of This Project
#
#  By Yunagi
#
#***************************************************************************

include ./env

.PHONY: clean_output_files
clean_output_files:
	rm -f *.csv
	rm -f *.txt
	rm -f ./output/*.csv
	rm -f ./output/*.txt
	rm -f ./logs/*.*

.PHONY: clean_compiled_files
clean_compiled_files:
	rm -f *.exe
	rm -f ./bin/*.exe
	rm -f *.o
	rm -f ./object/*.o

.PHONY: remove_subdirectories
remove_subdirectories:
	rm -fr objects
	rm -fr output
	rm -fr bin
	rm -fr include
	rm -fr src
	rm -fr props
	mkdir objects
	mkdir output
	mkdir bin

.PHONY: clean
clean: clean_compiled_files clean_output_files remove_subdirectories
	rm -f *.cpp
	rm -f ./src/*.cpp
	rm -f *.props
	rm -f ./props/*.props
	rm -f ./include/*.h


#***********Type 'make sheffield' to make whole project***********
.PHONY: sheffield
sheffield: clean_compiled_files
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -w -I./include -c ./src/main.cpp -o ./objects/main.o
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -w -I./include -c ./src/model.cpp -o ./objects/model.o
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -w -I./include -c ./src/agent.cpp -o ./objects/agent.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./main.exe  ./objects/main.o ./objects/model.o ./objects/agent.o $(REPAST_HPC_LIB) $(BOOST_LIBS)

#.PHONY: run
#run: $(MPIRUN) -n 4 ./bin/main.exe config.props model.props
