package com.adamhzp.core.Service;

public class KalmanFilter{

	private Double Q = 1.0;	//process noise
	private Double R = 1.0;	//measurement noise
	private Double A = 1.0;	//state vector
	private Double B = 1.0;	//Control vector
	private Double H = 1.0; //measurement vector
	private Double x = null;	//estimated signal without noise	
	private Double cov = null;

	public KalmanFilter(Double Q, Double R, 
			Double A, Double B, Double H)
	{
		if(R!=null) this.R = R;
		if(Q!=null) this.Q = Q;
		if(A!=null) this.A = A;
		if(B!=null) this.B = B;
		if(H!=null) this.H = H;
	}

	/**
	 * @param z measurement
	 * @param u control vector
	 */
	public Double filter(Double z, Double u){

		if(u == null) u =1.0;
		if(this.x == null){
			this.x = (1/this.H) * z;
			this.cov = (1 / this.H) * this.R * (1 / this.H);
		}else{
		      // Compute prediction
		      Double predX = (this.A * this.x) + (this.B * u);
		      Double predCov = ((this.A * this.cov) * this.A) + this.Q;

		      // Kalman gain
		      Double K = predCov * this.H * (1 / ((this.H * predCov * this.H) + this.R));

		      // Correction
		      this.x = predX + K * (z - (this.H * predX));
		      this.cov = predCov - (K * this.H * predCov);
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