package com.adamhzp.core.Model;

import java.util.HashMap;
import java.util.Map;

public class Beacon {
	
	private Map<String, Agent> agents;
	private String mac;
	private Position[] positions;
	
	public Beacon(String mac)
	{
		if(mac!=null) this.mac = mac;
		this.agents = new HashMap<String, Agent>();
		this.positions = new Position[10];
	}
	
	public void add_rssi(String agent_mac, double rssi)
	{
		if(agents.get(agent_mac)==null)
		{
			Agent agent = new Agent(null, agent_mac);
			
			agent.addRssi(rssi);
			this.agents.put(agent_mac, agent);
		}else{
			Agent agent = agents.get(agent_mac);
			agent.addRssi(rssi);
		}
		
	}
}
