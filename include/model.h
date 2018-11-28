#ifndef MODEL_H
#define MODEL_H

#include <boost/mpi.hpp>
#include "repast_hpc/Schedule.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/AgentRequest.h"

#include "agent.h"

/* Agent Package Provider */
class AgentPackageProvider {
private:
    repast::SharedContext<Agentclass>* agents;
public:
    AgentPackageProvider(repast::SharedContext<Agentclass>* agentPtr);
    void providePackage(Agentclass * agent, std::vector<AgentPackage>& out);
    void provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out);
};

/* Agent Package Receiver */
class AgentPackageReceiver {
private:
    repast::SharedContext<Agentclass>* agents;
public:
    AgentPackageReceiver(repast::SharedContext<Agentclass>* agentPtr);
    Agentclass * createAgent(AgentPackage package);
    void updateAgent(AgentPackage package);
};

class Modelclass{
	int stopAt;
	int countOfAgents;

	double W1; // Weight 1
	double W2; // Weight 2
	double W3; // Weight 3

	double P1[5]; // Property 1
	double P2[5]; // Property 2
	double P3[5]; // Property 3

	double S; // Initial value of score
	int STATE[5]; // Initial value of agent state

	repast::Properties* props;
	repast::SharedContext<Agentclass> context;

	AgentPackageProvider* provider;
	AgentPackageReceiver* receiver;

public:
	Modelclass(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~Modelclass();
	void init();

	void requestAgents();
    void cancelAgentRequests();

	void doSomething();
	void initSchedule(repast::ScheduleRunner& runner);
	void recordResults();
};


#endif // MODEL_H_INCLUDED
