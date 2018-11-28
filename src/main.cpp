#include <boost/mpi.hpp>
#include "repast_hpc/RepastProcess.h"

#include "model.h"


int main(int argc, char** argv){

	std::string configFile = argv[1]; // The name of the configuration file is Arg 1
	std::string propsFile  = argv[2]; // The name of the properties file is Arg 2

	boost::mpi::environment env(argc, argv);
	boost::mpi::communicator world;

	repast::RepastProcess::init(configFile);

	Modelclass* model = new Modelclass(propsFile, argc, argv, &world);
	// retrieves a handle to an object that manages the timing by which events in RepastHPC take place
	repast::ScheduleRunner& runner = repast::RepastProcess::instance()->getScheduleRunner();

	model->init();
	model->initSchedule(runner);

	runner.run();

	delete model;

	repast::RepastProcess::instance()->done();

}
