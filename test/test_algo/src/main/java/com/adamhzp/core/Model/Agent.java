package com.adamhzp.core.Model;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.adamhzp.core.Service.KalmanFilter;

public class Agent {
	
	private String id;
	private String mac;
	private Queue<Double> rssis;
	private KalmanFilter filter;
	private Double out = null;
	
	public Agent(String id, String mac)
	{
		if(id!=null) this.id = id;
		if(mac!=null) this.mac = mac;

		this.rssis = new ConcurrentLinkedQueue<Double>();
		filter = new KalmanFilter(0.03, 0.3, null, null, null);
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getMac() {
		return mac;
	}

	public void setMac(String mac) {
		this.mac = mac;
	}

	public Queue<Double> getRssis() {
		return rssis;
	}
	
	public void addRssi(double rssi)
	{
		double rssi_kf = filter.filter(rssi, null);
		
		this.add_rssi_to_q(rssi_kf);
	}
	
	private void add_rssi_to_q(Double rssi)
	{
		if(this.rssis.size() >= 4)
		{
			this.rssis.remove();
		}
		
		this.rssis.add(rssi);
		
		setout(rssi);
		
	}
	
	private void setout(Double rssi)
	{
		if(this.out == null){
			this.out = rssi;
			return;
		}
		Double MA = 5.0;
		Double max = -200.0;
		for(int i = 0; i<this.rssis.size(); i++)
		{
			Double temp = this.rssis.remove();
			this.rssis.add(temp);
			if(temp>max) max = temp; 
		}
		
		this.out = (this.out*(MA-1)+max)/MA;
		
	}

}
