#ifndef MODEL_H
#define MODEL_H

#include <boost/mpi.hpp>
#include "repast_hpc/Schedule.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/AgentRequest.h"
#include "repast_hpc/TDataSource.h"
#include "repast_hpc/SVDataSet.h"

#include "agent.h"

#define AGENT_COUNT 100

double gaussrand(double E, double V);

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
	int Scenario;

	ofstream txtPointer; // Print out cycling agents number in Result.txt

	double W1; // Weight 1
	double W2; // Weight 2
	double W3; // Weight 3
	double W4; // Weight 4
	double W5; // Weight 5
	double step; // Step size
	double P1[AGENT_COUNT]; // Property 1
	double P2[AGENT_COUNT]; // Property 2
	double P3[AGENT_COUNT] = {0.87,0.34,0.03,0.91,0.47,0.91,0.33,0.69,0.02,0.51,0.33,0.34,0.95,0.75,0.33,0.63,0.42,0.58,0.96,0.62,0.15,0.84,0.73,0.11,0.16,0.3,0.42,0.53,0.28,0.83,0.79,0.27,0.62,0.1,0.67,0.35,0.67,0.3,0.68,0.39,0.39,0.21,0.72,0.82,0.93,0.51,0.19,0.6,0.2,0.41,0.85,0.43,0.93,0.37,0.7,0.59,0.1,0.41,0.92,0.06,0.38,0.24,0.23,0.36,0.92,0.5,0.8,0.35,0.46,0.95,0.13,0.07,0.71,0.93,0.11,0.65,0.15,0.69,0.63,0.04,0.8,0.99,0.44,0.34,0.05,0.23,0.58,0.6,0.25,0.65,0.06,0.94,0.09,0.33,0.65,0.5,0.53,0.38
							 }; // Property 3
	double P4[AGENT_COUNT] = {1,0,0,0,1,1,1,0,1,0,1,1,1,1,0,0,1,1,0,0,1,1,   // 55% chance to own an automobile
			   1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,0,0,1,1,0,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0};
			   // 77% chance to own an automobile // Property 4
	double P5[AGENT_COUNT] = {0.65,0.8,0.77,0.88,0.34,0.81,0.9,0.48,0.85,0.76,0.82,0.76,0.75,0.64,0.67,0.64,0.69,0.6,0.82,0.64,0.91,0.47,0.65,0.5,0.82,0.77,0.77,0.9,0.65,0.8,0.64,0.72,0.73,0.48,0.72,0.61,0.7,0.98,0.38,0.37,0.54,0.62,0.81,0.72,0.81,0.89,0.41,0.64,0.86,0.44,0.47,0.63,0.59,0.34,0.73,0.96,0.51,0.81,0.83,0.66,0.74,0.67,0.35,0.62,0.38,0.71,0.75,0.62,0.8,0.79,0.43,0.75,0.89,0.75,0.51,0.66,0.51,0.82,0.69,0.71,0.63,0.78,0.57,0.66,0.57,0.95,0.94,0.81,0.77,0.79,0.8,0.95,0.68,0.81,0.68,0.6,0.8,0.48,0.8,0.9}; 
				// Property 5

	double S; // Initial value of score

	int STATE[AGENT_COUNT]; // Initial value of agent state

	repast::Properties* props;
	repast::SharedContext<Agentclass> context;

	repast::SVDataSet* agentValues;
	
	AgentPackageProvider* provider;
	AgentPackageReceiver* receiver;

public:
	Modelclass(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~Modelclass();

	int cycling_number;
	
	void init();

	void requestAgents();
    void cancelAgentRequests();
    void scenario1();
    void scenario2();
    void scenario3();
	void doSomething();
	void interaction();
	void initSchedule(repast::ScheduleRunner& runner);
	void recordResults();
};


#endif // MODEL_H_INCLUDED
