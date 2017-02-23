
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import processing.core.PApplet;

public class test_kf extends PApplet{  
	
	int xspacing = 16;   // How far apart should each horizontal location be spaced
	int w;              // Width of entire wave

	double theta = 0.0;  // Start angle at 0
	double amplitude = 4.0;  // Height of wave
	double period = 500.0;  // How many pixels before the wave repeats
	double dx;  // Value for incrementing X, a function of period and xspacing
	
	Queue<Double> yvals = new ConcurrentLinkedQueue<Double>();
	Queue<Double> kf_yvals = new ConcurrentLinkedQueue<Double>();
	static Queue<Double> queue = new ConcurrentLinkedQueue<Double>();
	static Queue<Double> kf_queue = new ConcurrentLinkedQueue<Double>();
	
	public static void main(String[] args) {
		
		server ser = new server(queue,kf_queue);
		Thread server_thread = new Thread(ser);
		server_thread.start();
        PApplet.main("test_kf");
    }
	
	public void settings(){
		  size(1000, 1000);
	}

	public void setup() {
	  w = width+16;
	  dx = (TWO_PI / period) * xspacing;
	}

	public void draw() {
	  background(0);
	  calcWave();
	  renderWave();
	}

	public void calcWave() {

	  int max = w/xspacing;
	  
	  if(queue.size()<max && yvals.size()==0) return;
	  
	  if(yvals.size()==max)
	  {
		  if(!queue.isEmpty()){
			  yvals.remove();
			  yvals.add(this.queue.remove());
		  }
	  }else{
		  for(int ct = 0 ; ct<max; ct++)
		  {
			  yvals.add(this.queue.remove());
		  }
	  }
	  
	  if(kf_yvals.size()==max)
	  {
		  if(!queue.isEmpty()){
			  kf_yvals.remove();
			  kf_yvals.add(this.kf_queue.remove());
		  }
	  }else{
		  for(int ct = 0 ; ct<max; ct++)
		  {
			  kf_yvals.add(this.kf_queue.remove());
		  }
	  }
	  
	}

	public void renderWave() {
	  noStroke();
	  fill(255);
	  // A simple way to draw the wave with an ellipse at each location
	  for (int x = 0; x < yvals.size(); x++) {
		  Double temp = yvals.remove();
		  yvals.add(temp);
		  fill(255);
		  ellipse(x*xspacing, (float) (height/2+temp*this.amplitude), 8, 8);
		  
		  Double temp2 = this.kf_yvals.remove();
		  kf_yvals.add(temp2);
		  fill(100);
		  ellipse(x*xspacing, (float) (height/2+temp2*this.amplitude), 8, 8);
	  }
	}
}


class server implements Runnable{
	
	private  int port = 9999;	//the port that recieve cmds
	private  Socket client = null;
	private  ServerSocket serverSock = null;
	private Queue queue = null;
	private Queue kf_q = null;
	
	public server(Queue q, Queue q2)
	{
		this.queue = q;
		this.kf_q = q2;
		try{
			serverSock = new ServerSocket(port);
			
		}catch(Exception e)
		{
			e.printStackTrace();
			return;
		}
	}
	
	public void run()
	{
		while(true){
			listen();
		}
	}
	
	private  void listen()
	{
		try{
			System.out.println("Start listening on port "+port);
			client = serverSock.accept();
			Connection con = new Connection(client,queue, this.kf_q);
			con.run();
		}catch(Exception e){
			e.printStackTrace();
		}
	}
}

class Connection{
	private Socket socket = null;
	private OutputStream outputStream = null;
	private InputStream inputStream = null;
	private KalmanFilter filter = null;
	private Queue<Double> queue = null;
	private Queue<Double> kf_queue= null;
	
	public Connection(Socket socket, Queue q, Queue q2){
		System.out.println("Get incomming connection from IP: "
				+socket.getRemoteSocketAddress().toString());
		this.socket = socket;
		this.queue = q;
		this.kf_queue = q2;
		filter = new KalmanFilter(0.01, 3.0, null, null, null);
	}
	
	public void run()
	{
		try{
			this.outputStream = socket.getOutputStream();
			this.inputStream = socket.getInputStream();

			while(true)
			{
				Thread.sleep(100);
				byte buf[] =  new byte[1024];
				int len = 0;
				if(inputStream.available()>0){
					len = inputStream.read(buf);
				}
				if(len< 30){
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
						Double val =Double.parseDouble(infos[1]);
						this.queue.add(val);
						Double val_f = filter.filter(val, 0.0);
						this.kf_queue.add(val_f);
						System.out.println(val+"..."+val_f);
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
			e.printStackTrace();
			return;
		}
	}
}

class KalmanFilter{

	private Double R = 1.0;	//process noise
	private Double Q = 1.0;	//measurement noise
	private Double A = 1.0;	//state vector
	private Double B = 1.0;	//Control vector
	private Double C = 1.0; //measurement vector
	private Double x = null;	//estimated signal without noise	
	private Double cov = null;

	public KalmanFilter(Double R, Double Q, 
			Double A, Double B, Double C)
	{
		if(R!=null) this.R = R;
		if(Q!=null) this.Q = Q;
		if(A!=null) this.A = A;
		if(B!=null) this.B = B;
		if(C!=null) this.C = C;
	}

	/*
	 * @param z measurement
	 * @param u control vector
	 */
	public Double filter(Double z, Double u){

		if(u == null) u =1.0;
		if(this.x == null){
			this.x = (1/this.C) * z;
			this.cov = (1 / this.C) * this.Q * (1 / this.C);
		}else{

		      // Compute prediction
		      Double predX = (this.A * this.x) + (this.B * u);
		      Double predCov = ((this.A * this.cov) * this.A) + this.R;

		      // Kalman gain
		      Double K = predCov * this.C * (1 / ((this.C * predCov * this.C) + this.Q));

		      // Correction
		      this.x = predX + K * (z - (this.C * predX));
		      this.cov = predCov - (K * this.C * predCov);
		}

		return this.x;
	}

	public Double getLastMeasureMent()
	{
		return this.x;
	}

	public void setMeasurementNoise(Double noise)
	{
		if(noise!=null) this.Q = noise;
	}

	public void setProcessNoise(Double noise)
	{
		if(noise!=null) this.R = noise;
	}
}