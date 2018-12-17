	std::vector<Agentclass*> agents;
	int agent_counter = 0;
	context.selectAgents(repast::SharedContext<Agentclass>::LOCAL, countOfAgents, agents);
	std::vector<Agentclass*>::iterator it = agents.begin();
	while(it != agents.end()){
	std::vector<std::string> keyOrder;
	double p1,p2,p3,p4,p5,s;
	int state;
	p1 = (*it)->getp1();
	p2 = (*it)->getp2();
	p3 = (*it)->getp3();
	p4 = (*it)->getp4();
	p5 = (*it)->getp5();

	s = (*it)->gets();
	state = (*it)->getstate();

//	props->putProperty("ID","p1","p2","p3","p4","p5","s","state");
	keyOrder.push_back("agent_counter");	
	keyOrder.push_back("p1");
	keyOrder.push_back("p2");
	keyOrder.push_back("p3");
	keyOrder.push_back("p4");
	keyOrder.push_back("p5");
	keyOrder.push_back("s");
	keyOrder.push_back("state");
	props->writeToSVFile("./output/agents.csv", keyOrder);
	agent_counter++;
	it++;
	}