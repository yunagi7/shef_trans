#include <stdio.h>
#include <vector>
#include <boost/mpi.hpp>
#include "repast_hpc/AgentId.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/initialize_random.h"

#include "model.h"

AgentPackageProvider::AgentPackageProvider(repast::SharedContext<Agentclass>* agentPtr): agents(agentPtr){ }

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
	for(int i = 0; i < countOfAgents; i++){
		P1[i] = repast::Random::instance()->nextDouble();
		P2[i] = repast::Random::instance()->nextDouble();
		P3[i] = repast::Random::instance()->nextDouble();
		STATE[i] = 0;
	}
	STATE[0] = 1;
	S = 0;

 	W1 = repast::strToDouble(props->getProperty("w1"));
	W2 = repast::strToDouble(props->getProperty("w2"));
	W3 = repast::strToDouble(props->getProperty("w3"));

	std::cout << "getProperty w1: " << W1 << std::endl;
	std::cout << "getProperty w2: " << W2 << std::endl;
	std::cout << "getProperty w3: " << W3 << std::endl;

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
	for(int i = 0; i < countOfAgents; i++){
		repast::AgentId id(i, rank, 0);
		id.currentRank(rank);
		Agentclass* agent = new Agentclass(id, W1, W2, W3, P1[i], P2[i], P3[i], S, STATE[i]);
		context.addAgent(agent);
//		agent->initset(1);
	}
}

void Modelclass::doSomething(){
    if(repast::RepastProcess::instance()->rank() == 0) std::cout << "DOING SOMETHING at tick " << repast::RepastProcess::instance()->getScheduleRunner().currentTick() << std::endl;
	std::vector<Agentclass*> agents;
//	context.selectAgents(countOfAgents, agents);
    context.selectAgents(repast::SharedContext<Agentclass>::LOCAL, countOfAgents, agents);
	std::vector<Agentclass*>::iterator it = agents.begin();
	while(it != agents.end()){
//		std::cout << (*it)->getId() << " ";
//      if(repast::RepastProcess::instance()->rank() == 0) std::cout << " Rank " << repast::RepastProcess::instance()->rank() << " agent " << (*it)->getId() << " playing " << std::endl;
		(*it)->play(&context); // W1, W2, W3
		it++;
    }

 /* Print out After play before sync */
 /*
    if(repast::RepastProcess::instance()->rank() == 0){
	std::cout << " AFTER PLAY, BEFORE SYNC: " << std::endl;
	repast::SharedContext<Agentclass>::const_state_aware_iterator local_agents_iter      = context.begin(repast::SharedContext<Agentclass>::LOCAL);
	repast::SharedContext<Agentclass>::const_state_aware_iterator local_agents_end       = context.end(repast::SharedContext<Agentclass>::LOCAL);
	while(local_agents_iter != local_agents_end){
	    Agentclass* agent = (&**local_agents_iter);
            std::cout << agent->getId() << " " << agent->getC() << " " << agent->getTotal() << std::endl;
	    local_agents_iter++;
 	}
        repast::SharedContext<Agentclass>::const_state_aware_iterator non_local_agents_iter  = context.begin(repast::SharedContext<Agentclass>::NON_LOCAL);
        repast::SharedContext<Agentclass>::const_state_aware_iterator non_local_agents_end   = context.end(repast::SharedContext<Agentclass>::NON_LOCAL);
        while(non_local_agents_iter != non_local_agents_end){
	    Agentclass* agent = (&**non_local_agents_iter);
	    std::cout << agent->getId() << " c:" << agent->getC() << " total:" << agent->getTotal() << std::endl;
	    non_local_agents_iter++;
        }
    }
*/


    repast::RepastProcess::instance()->synchronizeAgentStates<AgentPackage, AgentPackageProvider, AgentPackageReceiver>(*provider, *receiver);

 /* Print out After sync */
 /*   
    if(repast::RepastProcess::instance()->rank() == 0){
        std::cout << " AFTER SYNC: " << std::endl;
	repast::SharedContext<Agentclass>::const_state_aware_iterator local_agents_iter      = context.begin(repast::SharedContext<Agentclass>::LOCAL);
	repast::SharedContext<Agentclass>::const_state_aware_iterator local_agents_end       = context.end(repast::SharedContext<Agentclass>::LOCAL);
	while(local_agents_iter != local_agents_end){
	    Agentclass* agent = (&**local_agents_iter);
            std::cout << agent->getId() << " c:" << agent->getC() << " total:" << agent->getTotal() << std::endl;
	    local_agents_iter++;
 	}
        repast::SharedContext<Agentclass>::const_state_aware_iterator non_local_agents_iter  = context.begin(repast::SharedContext<Agentclass>::NON_LOCAL);
        repast::SharedContext<Agentclass>::const_state_aware_iterator non_local_agents_end   = context.end(repast::SharedContext<Agentclass>::NON_LOCAL);
        while(non_local_agents_iter != non_local_agents_end){
	    Agentclass* agent = (&**non_local_agents_iter);
	    std::cout << agent->getId() << " " << agent->getC() << " " << agent->getTotal() << std::endl;
	    non_local_agents_iter++;
         }
    }
*/

}

void Modelclass::initSchedule(repast::ScheduleRunner& runner){
	runner.scheduleEvent(1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<Modelclass> (this, &Modelclass::requestAgents)));
	runner.scheduleEvent(2, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<Modelclass> (this, &Modelclass::doSomething)));
	runner.scheduleEvent(4, repast::Schedule::FunctorPtr(new repast::MethodFunctor<Modelclass> (this, &Modelclass::cancelAgentRequests)));
	runner.scheduleEndEvent(repast::Schedule::FunctorPtr(new repast::MethodFunctor<Modelclass> (this, &Modelclass::recordResults)));
	runner.scheduleStop(stopAt);
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

		props->writeToSVFile("./output/results.csv", keyOrder);
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


