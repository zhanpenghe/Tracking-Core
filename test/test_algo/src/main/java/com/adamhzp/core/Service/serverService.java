package com.adamhzp.core.Service;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Semaphore;

public class serverService {
	
	private int port = 9999;
	private ServerSocket serverSock = null;
	
	private Semaphore semaphore;
	private Map<String, connectionService> connections;
	
	private boolean isRunning;
	
	public serverService()
	{
		try{
			serverSock = new ServerSocket(port);
			this.connections = new HashMap<String,connectionService>();
			semaphore = new Semaphore(1);
			this.isRunning = true;
		}catch(Exception e)
		{
			e.printStackTrace();
			return;
		}
	}
	
	public void run()
	{
		while(isRunning){
			listen();
		}
	}
	
	private void listen()
	{
		Socket client = null;
		try{
			System.out.println("Start listening on port "+port);
			client = serverSock.accept();
			connectionService con = new connectionService(client, this);
			
			Thread connection_thread = new Thread(con);
			connection_thread.start();
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	public void add_connection(connectionService con)
	{
		if(con.getMac()!=null){
			lock();
			this.connections.put(con.getMac(), con);
			unlock();
		}
	}
	
	private void lock()
	{
		try{
			this.semaphore.acquire();
		}catch(Exception e)
		{
			e.printStackTrace();
			return;
		}
	}
	
	private void unlock()
	{
		try{
			this.semaphore.release();
		}catch(Exception e)
		{
			e.printStackTrace();
			return;
		}
	}
	
	public void shutdown()
	{
		this.isRunning = false;
	}
}
