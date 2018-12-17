#include <stdio.h>
#include <vector>
#include <boost/mpi.hpp>
#include "repast_hpc/AgentId.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/initialize_random.h"

#include "model.h"

#include <stdlib.h>
#include <math.h>


AgentPackageProvider::AgentPackageProvider(repast::SharedContext<Agentclass>* agentPtr): agents(agentPtr){ }

double gaussrand(double E, double V) // E = Mean, V = Var = sigma^2; standard gaussrand(0, 1)
{
    static double V1, V2, S;
    static int phase = 0;
    double X;
	X = X * V + E;
	
    if ( phase == 0 ) {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;
             
            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);
         
        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);
         
    phase = 1 - phase;
 
    return X;
}

void AgentPackageProvider::providePackage(Agentclass * agent, std::vector<AgentPackage>& out){
    repast::AgentId id = agent->getId();
    AgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getC(), agent->getTotal());
    out.push_back(package);
}

void AgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out){
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}


AgentPackageReceiver::AgentPackageReceiver(repast::SharedContext<Agentclass>* agentPtr): agents(agentPtr){}

Agentclass * AgentPackageReceiver::createAgent(AgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type, package.currentRank);
    return new Agentclass(id, package.c, package.total);
}

void AgentPackageReceiver::updateAgent(AgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type);
    Agentclass * agent = agents->getAgent(id);
    agent->set(package.currentRank, package.c, package.total);
}


Modelclass::Modelclass(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm){
	props = new repast::Properties(propsFile, argc, argv, comm);
	stopAt = repast::strToInt(props->getProperty("stop.at"));
	countOfAgents = repast::strToInt(props->getProperty("count.of.agents"));
	step = 0.001;
	Scenario = 2;
	for(int i = 0; i < countOfAgents; i++)
		STATE[i] = 0;
	int cycling_number = 0;

 	W1 = repast::strToDouble(props->getProperty("w1"));
	W2 = repast::strToDouble(props->getProperty("w2"));
	W3 = repast::strToDouble(props->getProperty("w3"));
	W4 = repast::strToDouble(props->getProperty("w4"));
	W5 = repast::strToDouble(props->getProperty("w5"));

	std::cout << "Get w1 From Propfile: " << W1 << std::endl;
	std::cout << "Get w2 From Propfile: " << W2 << std::endl;
	std::cout << "Get w3 From Propfile: " << W3 << std::endl;
	std::cout << "Get w4 From Propfile: " << W4 << std::endl;
	std::cout << "Get w5 From Propfile: " << W5 << std::endl;

	std::cout << "Scenario " << Scenario << std::endl;

	initializeRandom(*props, comm);
	if(repast::RepastProcess::instance()->rank() == 0) props->writeToSVFile("./output/record.csv");

	provider = new AgentPackageProvider(&context);
	receiver = new AgentPackageReceiver(&context);
}

Modelclass::~Modelclass(){
	delete props;
	delete provider;
	delete receiver;
}

void Modelclass::init(){
	int rank = repast::RepastProcess::instance()->rank();
	int counter = 0;
	std::vector<Agentclass*> agents;
	txtPointer.open("Result.txt");

	for(int i = 0; i < countOfAgents; i++){
		repast::AgentId id(i, rank, 0);
		id.currentRank(rank);
		Agentclass* agent = new Agentclass(id, W1, W2, W3, W4, W5, P1[i], P2[i], P3[i], P4[i], P5[i], 0, STATE[i]);
		context.addAgent(agent);
//		agent->initset(1);
	}

//  Initialise Age of Agent Population
	for(int z=0;z<100;z++){
		if (z < 22)
			P1[z] = 0.1;
		else if (z < (22+17))
			P1[z] = 0.3;
			else if (z < (22+17+30))
				P1[z] = 0.5;
				else if (z < (22+17+30+18))
					P1[z] = 0.7;
					else P1[z] = 0.9;
	}

	for(int z=0;z<100;z++){
		if (z < 6)   // income 15000-
			P2[z] = 0.9;
		else if (z < (6+18)) // income 15000-30000
			P2[z] = 0.7;
			else if (z < (6+18+15)) // income 30000-40000
				P2[z] = 0.5;
				else if (z < (6+18+15+43)) //income 40000-50000
					P2[z] = 0.3;
					else P2[z] = 0.1; // income 50000+
	}

	context.selectAgents(repast::SharedContext<Agentclass>::LOCAL, countOfAgents, agents);
	std::vector<Agentclass*>::iterator it = agents.begin();
	while(it != agents.end()){
		(*it)->setAgent(W1, W2, W3, W4, W5, P1[counter], P2[counter], P3[counter], P4[counter], P5[counter]);
		it++;
		counter++;
	}
}
void Modelclass::scenario1(){
    if(repast::RepastProcess::instance()->rank() == 0){
    	std::cout << "   ### AT TICK: " << repast::RepastProcess::instance()->getScheduleRunner().currentTick() << " ### " << std::endl;
    }

	repast::AgentId currentAgentId;	
	std::vector<Agentclass*> agents;
	int counter = 0;

	W1 = W1 - step;

    context.selectAgents(repast::SharedContext<Agentclass>::LOCAL, countOfAgents, agents);
	std::vector<Agentclass*>::iterator it = agents.begin();

	while(it != agents.end()){

		if((*it)->decisionmaking(&context))
			cycling_number++;

		(*it)->setw1(W1);

		it++;
		counter++;
    }

    interaction();

    if(repast::RepastProcess::instance()->rank() == 0){
    	std::cout << ">>> Number of cycling agents: " << cycling_number << std::endl;
    	txtPointer << cycling_number << ",";
    	cycling_number = 0;
	}

	repast::RepastProcess::instance()->synchronizeAgentStates<AgentPackage, AgentPackageProvider, AgentPackageReceiver>(*provider, *receiver);
}

void Modelclass::scenario2(){

    if(repast::RepastProcess::instance()->rank() == 0){
    	std::cout << "   ### AT TICK: " << repast::RepastProcess::instance()->getScheduleRunner().currentTick() << " ### " << std::endl;
    }

	repast::AgentId currentAgentId;	
	std::vector<Agentclass*> agents;
	int counter = 0;
	double p3temp; // work flex

	W1 = W1 - step;
	W2 = W2 + W2 * step;
	W4 = W4 - W4 * step;
	W5 = W5 + W5 * 3 * step;

    context.selectAgents(repast::SharedContext<Agentclass>::LOCAL, countOfAgents, agents);
	std::vector<Agentclass*>::iterator it = agents.begin();


	while(it != agents.end()){
		p3temp = (*it)->getp3();
		if (p3temp < 0.2){
			if (repast::Random::instance()->nextDouble() < 0.1){
				currentAgentId = (*it)->getId();
				(*it)->setp3(P3[counter]-0.15);
//				std::cout << currentAgentId << " with workflex : " << p3temp << " tend to become potential cycler" << std::endl;
			}
		}

		if((*it)->decisionmaking(&context))
			cycling_number++;

		(*it)->setw1(W1);
		(*it)->setw2(W2);
		(*it)->setw3(W3);
		(*it)->setw4(W4);
		(*it)->setw5(W5);

		it++;
		counter++;
    }

    interaction();

    if(repast::RepastProcess::instance()->rank() == 0){
    	std::cout << ">>> Number of cycling agents: " << cycling_number << std::endl;
    	txtPointer << cycling_number << ",";
    	cycling_number = 0;
	}

	repast::RepastProcess::instance()->synchronizeAgentStates<AgentPackage, AgentPackageProvider, AgentPackageReceiver>(*provider, *receiver);
}

void Modelclass::scenario3(){

    if(repast::RepastProcess::instance()->rank() == 0){
    	std::cout << "   ### AT TICK: " << repast::RepastProcess::instance()->getScheduleRunner().currentTick() << " ### " << std::endl;
    }

	repast::AgentId currentAgentId;	
	std::vector<Agentclass*> agents;
	int counter = 0;
	double p5temp; // eco-friendliness of current agent

	W1 = W1 - step;
	W3 = W3 - W3 * step;
	W5 = W5 + W5 * 4 * step;

//	context.selectAgents(countOfAgents, agents);
    context.selectAgents(repast::SharedContext<Agentclass>::LOCAL, countOfAgents, agents);
	std::vector<Agentclass*>::iterator it = agents.begin();


	while(it != agents.end()){
		if((*it)->decisionmaking(&context))
			cycling_number++;

		p5temp = (*it)->getp5();
		if (p5temp > 0.95){
			if (repast::Random::instance()->nextDouble() < 0.1){
				currentAgentId = (*it)->getId();
				(*it)->setp4(0);
//				std::cout << currentAgentId << " with eco : " << p5temp << " ignored its car ownership" << std::endl;
			}
		}
		
  //   	if (repast::Random::instance()->nextDouble() < 0.05){ // 5% chance to change eco-friendliness 
  //   		(*it)->changep3(p3temp);
  //   		std::cout << (*it)->getId() << " changed its eco-value to: " << p3temp << " from " << currentAgentId << std::endl;
  //   	}

		(*it)->setw1(W1);
		(*it)->setw2(W2);
		(*it)->setw3(W3);
		(*it)->setw4(W4);
		(*it)->setw5(W5);

		// std::cout << "w1:" << (*it)->getw1() << std::endl;
		// std::cout << "p1:" << (*it)->getp1() << std::endl;
		it++;
		counter++;
    }
    	interaction();

    // Print out results
    if(repast::RepastProcess::instance()->rank() == 0){
    	std::cout << ">>> Number of cycling agents: " << cycling_number << std::endl;
    	txtPointer << cycling_number << ",";
    	cycling_number = 0;
    }

    repast::RepastProcess::instance()->synchronizeAgentStates<AgentPackage, AgentPackageProvider, AgentPackageReceiver>(*provider, *receiver);
}

void Modelclass::doSomething(){
	// scenario1();
	// scenario2();
	scenario3();
}


void Modelclass::initSchedule(repast::ScheduleRunner& runner){
	runner.scheduleEvent(0, repast::Schedule::FunctorPtr(new repast::MethodFunctor<Modelclass> (this, &Modelclass::requestAgents)));
	runner.scheduleEvent(1, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<Modelclass> (this, &Modelclass::doSomething)));
	runner.scheduleEvent(4, repast::Schedule::FunctorPtr(new repast::MethodFunctor<Modelclass> (this, &Modelclass::cancelAgentRequests)));
	runner.scheduleEndEvent(repast::Schedule::FunctorPtr(new repast::MethodFunctor<Modelclass> (this, &Modelclass::recordResults)));
	runner.scheduleStop(stopAt);
}

void Modelclass::interaction(){
	repast::AgentId currentAgentId;	
	std::vector<Agentclass*> agents;
	double p5temp; // eco-friendliness of current agent
	while (repast::Random::instance()->nextDouble() < 0.9){
		context.selectAgents(repast::SharedContext<Agentclass>::LOCAL, 2, agents);
		std::vector<Agentclass*>::iterator it = agents.begin();
		currentAgentId = (*it)->getId();
		p5temp = (*it)->getp5();
		it++;
		(*it)->setp5(p5temp);
//		std::cout << currentAgentId << " shared its eco-value : " << p5temp << " to " << (*it)-> getId() << std::endl;
	}
}

void Modelclass::recordResults(){
	if(repast::RepastProcess::instance()->rank() == 0){
		props->putProperty("Result","Passed");
		std::vector<std::string> keyOrder;
		keyOrder.push_back("RunNumber");
		keyOrder.push_back("stop.at");
		keyOrder.push_back("Result");
		
		keyOrder.push_back("w1");
		keyOrder.push_back("w2");
		keyOrder.push_back("w3");
		keyOrder.push_back("w4");
		keyOrder.push_back("w5");
		
		props->writeToSVFile("./output/results.csv", keyOrder);
		txtPointer.close();
   }

}

void Modelclass::requestAgents(){
	int rank = repast::RepastProcess::instance()->rank();
	int worldSize= repast::RepastProcess::instance()->worldSize();
	repast::AgentRequest req(rank);
	for(int i = 0; i < worldSize; i++){                              // For each process
		if(i != rank){                                           // ... except this one
			std::vector<Agentclass*> agents;
			context.selectAgents(5, agents);                 // Choose 5 local agents randomly
			for(size_t j = 0; j < agents.size(); j++){
				repast::AgentId local = agents[j]->getId();          // Transform each local agent's id into a matching non-local one
				repast::AgentId other(local.id(), i, 0);
				other.currentRank(i);
				req.addRequest(other);                      // Add it to the agent request
			}
		}
	}
//	std::cout << " BEFORE: RANK " << rank << " has " << context.size() << " agents." << std::endl;
	repast::RepastProcess::instance()->requestAgents<Agentclass, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, req, *provider, *receiver, *receiver);
//	std::cout << " AFTER:  RANK " << rank << " has " << context.size() << " agents." << std::endl;
}

void Modelclass::cancelAgentRequests(){
	int rank = repast::RepastProcess::instance()->rank();
//	if(rank == 0) std::cout << "CANCELING AGENT REQUESTS" << std::endl;
	repast::AgentRequest req(rank);

	repast::SharedContext<Agentclass>::const_state_aware_iterator non_local_agents_iter  = context.begin(repast::SharedContext<Agentclass>::NON_LOCAL);
	repast::SharedContext<Agentclass>::const_state_aware_iterator non_local_agents_end   = context.end(repast::SharedContext<Agentclass>::NON_LOCAL);
	while(non_local_agents_iter != non_local_agents_end){
		req.addCancellation((*non_local_agents_iter)->getId());
		non_local_agents_iter++;
	}
	repast::RepastProcess::instance()->requestAgents<Agentclass, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, req, *provider, *receiver, *receiver);

	std::vector<repast::AgentId> cancellations = req.cancellations();
	std::vector<repast::AgentId>::iterator idToRemove = cancellations.begin();
	while(idToRemove != cancellations.end()){
		context.importedAgentRemoved(*idToRemove);
		idToRemove++;
	}
}


