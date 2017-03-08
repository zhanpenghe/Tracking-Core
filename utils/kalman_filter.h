/*
 * 1D Kalman Filter Implementation
 * with default settings.
 * @Author Adam Ho
 * 
 *
 * Example usage:
 *
 * kmf_t *filter = (kmf_t *) malloc(sizeof(kmf_t));
 * setup_kmf(filter, 0.03, 0.3);
 * ... use filter to do the filtering
 *
 */

typedef struct kmf{

	float Q;	//Process noise
	float R;	//Measurement noise
	float A;	//State vector
	float B;	//Control vector
	float H;	//Measurement vector
	float x; 	//Estimatied signal without noise
	float cov;	//Covariance matrix
	int8_t isInit;
}kmf_t;

void init_kmf(kmf_t *filter)
{
	if(filter == NULL) return;

	filter->Q = 1.0;
	filter->R = 1.0;
	filter->A = 1.0;
	filter->B = 1.0;
	filter->H = 1.0;
	filter->x = 0.0;
	filter->cov = 0.0;
	filter->isInit = 1;
}

void setup_kmf(kmf_t *filter, float Q, float R)
{
	if(filter == NULL) return;

	init_kmf(filter);
	if(Q>=0) filter->Q = Q;
	if(R>=0) filter->R = R;
}

float filter(kmf_t *filter, float z)
{
	float u = 1.0;	//This should have been input depending on the real situation,
					//but in our case I usually just used 1.0
	float predX, predCov, K;

	if(filter == NULL) return 1.5;	//RSSI is negative, so just return 1.5 for exeption

	if(filter->isInit == 1.0)
	{
		filter->x = (1/filter->H) * z;
		filter->cov = (1/filter->H)*filter->R*(1/filter->H);
		filter->isInit = 0.0;
	}else{
		//predict
		predX = (filter->A*filter->x)+(filter->B*u);
		predCov = ((filter->A*filter->cov)*filter->A)+filter->Q;

		//Kalman gain
		K = predCov*filter->H*(1/((filter->H*predCov*filter->H)+filter->R));

		//Correction
		filter->x = predX+K*(z-(filter->H*predX));
		filter->cov = predCov-(K*filter->H*predCov);
	}

	return filter->x;

}