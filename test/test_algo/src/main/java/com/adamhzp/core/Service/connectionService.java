package com.adamhzp.core.Service;

import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class connectionService implements Runnable{
	
	private Socket socket = null;
	private OutputStream outputStream = null;
	private InputStream inputStream = null;
	
	private serverService server = null;
	
	private String mac = null;
	private boolean isRunning;
	
	public String getMac() {
		return mac;
	}

	public connectionService(Socket socket, serverService server)
	{
		System.out.println("Get incomming connection from IP: "
				+socket.getRemoteSocketAddress().toString());
		this.socket = socket;
		this.server = server;
		try{
			this.outputStream = socket.getOutputStream();
			this.inputStream = socket.getInputStream();
		}catch(Exception e){
			e.printStackTrace();
			this.isRunning = false;
			return;
		}
		this.isRunning = true;
	}
	
	public void run()
	{
		try{
			while(true)
			{
				Thread.sleep(100);
				byte buf[] =  new byte[1024];
				int len = 0;
				if(inputStream.available()>0){
					len = inputStream.read(buf);
				}
				if(len< 30){
					this.mac = (new String(buf, 0, len)).split("\\|")[0];
					this.server.add_connection(this);
					System.out.println(this.mac);
					outputStream.write("z".getBytes());
					continue;
				}
				
				String info = new String(buf, 0, len);		
				String lines[] = info.split("\n");
				int i = 0;
				for(String line : lines)
				{
					if(i == 0)
					{
						i++;
						continue;
					}
					
					String infos[] = line.split("\\|");
					try{
						String beacon_mac = infos[0];
						Double val =Double.parseDouble(infos[1]);
						
					}catch(Exception e)
					{
						e.printStackTrace();
						continue;
					}
				}
				
				outputStream.write("z".getBytes());
			}
		}catch(Exception e)
		{
			
		}
	}

}
