import processing.core.*;

public class kf_test{



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
		this.R = R;
		this.Q = Q;
		this.A = A;
		this.B = B;
		this.C = C;
	}

	/*
	 * @param z measurement
	 * @param u control vector
	 */
	public Double filter(Double z, Double u){

		if(this.x = null){
			this.x = (1/this.C) * z;
			this.cov = (1/this.C) * this.Q * (1/this.C);
		}else{
			//predict
			Double predX = this.A*this.x + this.B*u;
			Double predCov = (this.A * this.cov)*this.A+this.R;
			
			//kalman gain
			Double K = predCov*this.C *(1/(this.C*predCov*this.C)+this.Q);

			//Correction(update)
			this.x = predX +K*(z-this.C*predX);
			this.cov = predCov - (K*this.C*predCov);
		}

		return this.x
	}

	public Double getLastMeasureMent()
	{
		return this.X;
	}

	public void setMeasurementNoise(Double noise)
	{
		this.Q = noise;
	}

	public void setProcessNoise(Double noise)
	{
		this.R = noise;
	}
}