/*
 *
 * Distributed under terms of the MIT license.
 */
const int ad_per_video = 7;
const int auction_limit = 200000;
const int num_bidders = 1000;
const double pi = acos(-1.0);
int toti = 0;
double ad_rev=0; 
int corp;
int slots_corp = 0;
using namespace std;
int random(int n) { return rand()%n;}
int randy(int a, int b){
	return rand()%(b-a)+a;
}
double normal(double l, double range)
{
	double r = l+range;
	double U1 = random(100000)/100000.0;
	double U2 = random(100000)/100000.0;
	double z = sqrt(-2*log(U1))*sin(2*pi*U2);

	double sigma = (r-l)/2/3;
	double mu 	 = (r+l)/2;
	z = z*sigma +mu;
	if (z<l) z=l;
	if (z>r) z=r;
	return z;
}
struct Config
{
	int B_base,B_range;
	int T_base,T_range;
	int r_base,r_range;
	char file[20];
	int ad,bidder;
};

void init(int num_bidders,double* B,int* T,double* r, Config config)
{
	ad_rev = 0;
	
	for (int i = 0;i<num_bidders;i++) 
	{
		double factor;

		B[i] = normal(config.B_base, config.B_range);
		T[i] = normal(config.T_base, config.T_range);
		r[i] = normal(config.r_base, config.r_range)/100.0;

	}
	for(int i = 0; i < num_bidders/2; i++){
			r[i] = 0;
	}

	random_shuffle(r, r + num_bidders);//change

}
// 134605
class Strategy
{
	protected:
 	static const int N=1200;
	static const int BCP_per_auction=200; 
	double profit,R_max,c;
	int n,m;
	int isBA2 = 0;
	int num_auction;
	double P[N], B_remain[N], B[N], x[N], phi[N];
	int T[N], r[N];
	char name[233];
	int BA;
	int isprop=0;
	double track1[N], track2[N], track3[N], track4[N], track5[N];//4 records budge remained after running up impression
	int num_track;
	public:

	Strategy(int num_bidders, double* B_, int * T_, double* P_, const char* name_)
	{
	//	cout <<"stategy initiating"<<endl;
		strcpy(name, name_);
		n = num_bidders;m=0;BA=0;
		assert(n<=N);
		for (int i=0;i<n;i++)
		{
			B[i]=B_remain[i] = B_[i];
			x[i]=0, phi[i]=1;
			T[i]=r[i]=T_[i];
			P[i]=P_[i];
		}
		num_auction = 0;
		R_max  = 0;
		profit = 0;
		c = 1;
		num_track = 0;
	}
	virtual int auction(double *b)
	{
//		if(num_auction%2==0)
//			cha_b(b);
	
		change_bid(b);
		
		int i=0;double tmp=-1;
		num_auction++;	
		toti = num_auction;
	//	cout << num_auction<<endl;
		for (int j=0;j<n;j++)
		{
			double f=cost(b, j);
			if (f>tmp && B_remain[j]>0) i=j, tmp=f;
			R_max = max(R_max, b[j]/B[j]);
		}
		c = pow(1+R_max, 1/R_max);
		if (B_remain[i]<=0) return 1;
		if(P[i]>0){
			slots_corp++;
		}
		if(r[i] && (P[i]>0)){
			if(isBA2)
				ad_rev+=min(B_remain[i],b[i]-P[i])*1.2;
			else if(isprop)
				ad_rev+=min(B_remain[i],b[i])*1.2;
			else
				ad_rev+=min(B_remain[i],b[i]);
		}
		else{
			ad_rev+=min(B_remain[i],b[i]);
		}
		update(b, i);
	//	cout <<"updated"<<endl;
		profit 		+= min(B_remain[i], b[i]);
		
	//	if (r[i]) profit += P[i];//change
		B_remain[i] -= min(B_remain[i], b[i]);
		if (num_auction%BCP_per_auction==0) // to be modified
		{
			i = num_track++;
			if (i>1000)
			{
				puts("too many checkpoint, BCP_per_auction should be larger");
				exit(0);
			}
			double budget1 = 0, all1=0,budget2=0,all2=0;
			int CF = 0;
		/*	for (int i=0;i<n/10;i++) budget += B_remain[i], all+=B[i], CF+=r[i]==0;
			track1[i] = (all-budget)/all;
			track3[i] = 1.0*CF/(n/10);*/

			budget1 = 0;all1=0;CF=0;budget2=0;all2=0;
			for (int i=0;i<num_bidders;i++){
				if(P[i]>0){
					budget1+=B_remain[i];
					all1+=B[i];
				}
			 	else{
			 		budget2+=B_remain[i];
			 		all2+=B[i];
				 }
			 CF+=r[i]==0;}
			track2[i] = (all1-budget1)/all1;
			track4[i] = 1.0*CF/(n/10*9);
			track3[i] = (all2-budget2)/all2;
		/*	for (int k = 0; k < num_bidders; k++)
			{
				budget += B_remain[k]; all += B[k]; if(T[k]!=0) CF += (r[k] == 0);
			}
//			cout << B[0]<<" "<<B_remain[0]<<endl;	
			track1[i] = (all - budget) / all;
			track2[i] = 2.0*CF / num_bidders;
			track3[i] = ad_rev;
		//	cout <<track3[i]<<" ";
			track5[i] = profit;*/
		}
		return 0;
	}

	virtual double get_profit(int profit_only = 0)
	{

		double ret=profit;
		double res=0;
		double contract_remain = 0;
		int complete_num = 0;
		for (int i=0;i<n;i++)
		{
			if (!BA)
			{
				ret+=(T[i]-r[i])*P[i];
				contract_remain += r[i] * P[i];
				if (r[i]==0) complete_num++;
			}
			res+=B_remain[i];
		}

		if (profit_only) return ret;
//		puts("");
		
/*		printf("contract CR      : %.2lf\n",1.0*complete_num/n);
		printf("contract remained: %.2lf\n",contract_remain);
		printf("budget   remained: %.2lf\n",res);
		printf("auction profit   : %.2lf\n",profit);*/

/*		puts("Advertiser Budget Remain");
		for (int i=0;i<num_track;i++) printf("%.2lf ", track1[i]);
		puts("\n");

		puts("Advertiser Average Impression Completion Rate");
		for (int i=0;i<num_track;i++) printf("%.2lf ", track2[i]);
		puts("\n");

		puts("Total Advertiser Profit");
		for (int i=0;i<num_track;i++) printf("%.2lf ", track3[i]);
		puts("\n");

		puts("Platform Profit");
		for (int i = 0; i < num_track; i++) printf("%.2lf ", track5[i]);
		puts("\n");*/ 
/*		puts("CR_small track");
		for (int i=0;i<num_track;i++) printf("%.2lf ", track4[i]);
		puts("\n");*///change

		for(int i = 0; i < num_track; i++){
			cout << track2[i]<<" ";
		}
		cout << endl;
		for(int i= 0; i <num_track; i++){
			cout <<track3[i]<<" ";
		}
		cout << endl;
		return ret;
	}
	virtual void change_bid(double *b)//changed
	{

	}

	virtual double cost(double *b, int j) = 0;
	virtual void update(double *b, int i) = 0;
};
class BasicPrimal_Dual_ver1:public Strategy
{/*{{{*/
	public:
	BasicPrimal_Dual_ver1(int num_bidders, double* B_, int * T_, double* P_, const char* name_)
		:Strategy(num_bidders, B_, T_, P_, name_)
	{
		BA=1;
		for (int i=0;i<n;i++) 
		{
			B[i]=B_remain[i] = B_[i]+ T_[i]*P_[i];
		}
	}
	double cost(double *b, int j)
	{
		return b[j]*(1-x[j]);
	}
	void update(double *b, int i)
	{
		if (r[i] > 0)
		{
			r[i]--;
			if (r[i] == 0) track4[i] = B_remain[i];
		}
		x[i] = x[i]*(1+b[i]/B[i])+b[i]/(c-1)/B[i];
	}
};/*}}}*/
class BasicPrimal_Dual_ver2:public BasicPrimal_Dual_ver1
{/*{{{*/
	public:
	BasicPrimal_Dual_ver2(int num_bidders, double* B_, int * T_, double* P_, const char* name_)
		:BasicPrimal_Dual_ver1(num_bidders, B_, T_, P_, name_){isBA2=1;}

	void change_bid(double *b)
	{
	//	for (int i=0;i<n/10;i++) b[i] *= 2; // 
//		for (int i=0;i<num_bidders;i++) b[i] *= B_remain[i]/B[i];
	/*	for (int i=0;i<num_bidders;i++)
		{
			if(i%2==0) 
				b[i] *= 0.3; // modified
		} */
	//	for (int i=0;i<num_bidders/2;i++){ if b[i] /= 20}; // modified
		for (int i=0;i<num_bidders;i++)
		if (r[i]>0)
			b[i] += P[i];
	}
	double cost(double *b, int j)
	{
		return b[j]*(1-x[j]);
	}
};/*}}}*/


class BasicPrimal_Dual_ver3:public BasicPrimal_Dual_ver1
{/*{{{*/
	public:
	BasicPrimal_Dual_ver3(int num_bidders, double* B_, int * T_, double* P_, const char* name_)
		:BasicPrimal_Dual_ver1(num_bidders, B_, T_, P_, name_)
	{
	}
	void change_bid(double *b)
	{
		for (int i=0;i<n/10;i++) b[i] *= 2; // modified
		for (int i=0;i<num_bidders;i++) b[i] *= B_remain[i]/B[i];
		
		for (int i=0;i<n;i++) b[i] += P[i];
	}
	double cost(double *b, int j)
	{
		return b[j]*(1-x[j]);
	}
};/*}}}*/

class ContractFirstAllocation:public Strategy
{/*{{{*/
	public:
	ContractFirstAllocation(int num_bidders, double* B_, int * T_, double* P_, const char* name_)
		:Strategy(num_bidders, B_, T_, P_, name_)
	{isprop=1;}
	double cost(double *b, int j) 
	{
		double ret = b[j]*(1-x[j])+P[j]*phi[j];
		if (x[j]>1) return ret;
		if (r[j]>0) ret += 10000;
		return ret;
	}
	void update(double *b, int i)
	{
		if (r[i]>0&&B_remain[i]>=b[i]) 
		{
			r[i]--;
			if (r[i] == 0) track4[i] = B_remain[i];
		}
	//	if (x[i]>=1) return;
		x[i] = x[i]*(1+b[i]/B[i])+b[i]/(c-1)/B[i];
		if(T[i]!=0){
		    if (phi[i]>0) phi[i]=phi[i]*(1+1/T[i])-c/(c-1)/T[i];
		}
		
		if (r[i]==0) phi[i]=0;
	}

};/*}}}*/
class GreedyAllocation:public Strategy
{/*{{{*/
	public:
	GreedyAllocation(int num_bidders, double* B_, int * T_, double* P_, const char* name_)
		:Strategy(num_bidders, B_, T_, P_, name_)
	{}
	double cost(double * b, int j)
	{
		double res=min(b[j], B_remain[j]);
		if (r[j]>0&&B_remain[j]>=b[j]) res+=P[j];
		return res;
	}
	void update(double *b, int i)
	{
		if (r[i]>0&&B_remain[i]>=b[i])
		{
			r[i]--;
			if (r[i] == 0) track4[i] = B_remain[i];
		}
		x[i] = x[i]*(1+b[i]/B[i])+b[i]/(c-1)/B[i];
		if(T[i]!=0){
		    if (phi[i]>0) phi[i]=phi[i]*(1+1/T[i])-c/(c-1)/T[i];
		}
		
		if (r[i]==0) phi[i]=0;
	}
};/*}}}*/

class AdAllocation:public Strategy
{
	public:
	AdAllocation(int num_bidders, double* B_, int * T_, double* P_, const char* name_)
		:Strategy(num_bidders, B_, T_, P_, name_)
	{isprop=1;}
	double cost(double * b, int j)
	{
		return b[j]*(1-x[j])+P[j]*phi[j];
	}
	void update(double *b, int i)
	{
//		cout << "update"<<endl;
		if (r[i]>0&&B_remain[i]>=b[i])
		{
			r[i]--;
			if (r[i] == 0) track4[i] = B_remain[i];
		}
		x[i] = x[i]*(1+b[i]/B[i])+b[i]/(c-1)/B[i];
		if(T[i]!=0){
		
		if (phi[i]>0) phi[i]=max(0.0, phi[i]*(1+1/T[i])-c/(c-1)/T[i]);
		}
		if (r[i]==0) phi[i]=0;
	}
};

#define REP(i,n) for (int i=1;i<=(n);i++)
class Simplex:public Strategy
{/*{{{*/
	static const int maxn = 5005,maxm = 5205;
	const double eps = 1e-8,INF = 1e15;
	double a[maxm][maxn],ans[maxn];
	int n,m, verbose;
	double reward;
	int id[maxn << 4];
	//
	// * a[i][j]*x[j]<=a[i][0]
	// * maximize a[0][j]*x[j]
	// * verbose 0/1
	// * 
	public:
	Simplex(int _n, double * B_, int * T_, double * r_, const char* name_)
		:Strategy(_n, B_, T_, r_, name_)
	{
		cout <<"simplex initiating"<<endl;
		m=0;n=_n;
		total_var = n+1;
		total_equ = 3*n+1;
		reward = 0;
		verbose = 0;
		for (int i=0;i<n;i++)
		{
	//		if(i%100==0)
	//			cout << "checkpoint"<<i<<endl;
			a[i+1][0]=B_[i];
			a[i+n+1][0]= 0;//-T_[i];
			a[i+n+1][i+1]= 1;//-1;
			a[0][i+1] = r_[i];//-r[i]
		//	reward += T_[i]*r_[i];
		//
			a[i+2*n+1][i+1]=1;
			a[i+2*n+1][0]=T_[i];
		}
	//	cout << "simplex initiated"<<endl;
	}
	int total_equ, total_var;
	virtual int auction(double *b)
	{
		// the first n vars is h_i
		// var n*m+n
		// equ n+n+m
		change_bid(b);
		// 
		for (int j=0;j<n;j++)
		{
			a[total_equ][j+total_var] = 1;
			a[j+1  ][j+total_var] = b[j];
			a[j+1+n][j+total_var] = -1;
			a[0    ][j+total_var] = b[j];
		}
		a[total_equ][0]=1;
		total_equ++;
		total_var+=n;
		return 0;
	}

	virtual double get_profit(int only = 0)
	{
		int tmpn=n;
		double ret;
		m = total_equ-1;
		n = total_var-1;
		printf("m,n:%d %d\n",m,n);

	/*	for (int j=0;j<=m;j++)
		for (int i=0;i<=n;i++) 
			printf("%5.2lf%c", a[j][i], i<n?' ':'\n');
*/

		REP(i,n) id[i] = i;
		if (init() && simplex())
		{
			ret = -a[0][0]+reward;
			printf("%s: %.8lf\n", name, ret);
			if (verbose)
			{
				REP(i,m) ans[id[n + i]] = a[i][0];
				REP(i,n) printf("%.8lf ",ans[i]);
			}
		}
		else 
		{
			puts("Error!!!!");
			ret = -1;
		}
		n=tmpn;
		return ret;
	}

	double cost(double *b, int i) { return -1;}
	void update(double *b, int i) {return;}

//------------template---------------
	private:
	void Pivot(int l,int e)
	{
//		puts("Pivot");
		swap(id[n + l],id[e]);
		double t = a[l][e]; a[l][e] = 1;
		for (int j = 0; j <= n; j++) a[l][j] /= t;
		for (int i = 0; i <= m; i++) if (i != l && abs(a[i][e]) > eps)
		{
			t = a[i][e]; a[i][e] = 0;
			for (int j = 0; j <= n; j++) a[i][j] -= a[l][j] * t;
		}
	}
	bool init(){
		while (true){
//			puts("init");
			int e = 0,l = 0;
			for (int i = 1; i <= m; i++) if (a[i][0] < -eps && (!l|| (rand() & 1))) l = i;
			if (!l) break;
			for (int j = 1; j <= n; j++) if (a[l][j] < -eps && (!e || (rand() & 1))) e = j;
			if (!e){puts("Infeasible"); return false;}
			Pivot(l,e);
		}
		return true;
	}
	bool simplex(){
		while (true){
//			puts("simplex");
			int l = 0,e = 0; double mn = INF;
			for (int j = 1; j <= n; j++) if (a[0][j] > eps){e = j; break;}
			if (!e) break;
			for (int i = 1; i <= m; i++) if (a[i][e] > eps && a[i][0] / a[i][e] < mn)
				mn = a[i][0] / a[i][e],l = i;
			if (!l){puts("Unbounded"); return false;}
			Pivot(l,e);
		}
		return true;
	}

};/*}}}*/

class Balance :public Strategy 
{
public:
	Balance(int num_bidders, double* B_, int* T_, double* P_, const char* name_)
		:Strategy(num_bidders, B_, T_, P_, name_)
	{}
	double cost(double * b, int j) 
	{
		double res = B_remain[j] / B[j];
		return res;
	}
	void update(double *b, int i)
	{
		if (r[i] > 0 && B_remain[i] >= b[i])
		{
			r[i]--;
			if (r[i] == 0) track4[i] = B_remain[i];
		}
		//now
	}
};


int read_config(Config &config, int argc, char *argv[])
{/*{{{*/
	config.B_base=config.T_base=config.r_base=-1;
	strcpy(config.file, "a");
	for (int i=1;i<argc;i++)
	{
		if (!strcmp(argv[i], "B")) 
		{
			sscanf(argv[i+1],"%d", &config.B_base);
			sscanf(argv[i+2],"%d", &config.B_range);
			i+=2;
			continue;
		}
		if (!strcmp(argv[i], "T")) 
		{
			sscanf(argv[i+1],"%d", &config.T_base);
			sscanf(argv[i+2],"%d", &config.T_range);
			i+=2;
			continue;
		}
		if (!strcmp(argv[i], "r")) 
		{
			sscanf(argv[i+1],"%d", &config.r_base);
			sscanf(argv[i+2],"%d", &config.r_range);
			i+=2;
			continue;
		}
		if (!strcmp(argv[i], "file"))
		{
			strcpy(config.file, argv[i+1]);
			i++;
			continue;
		}
		if (i==argc-2)
		{
			sscanf(argv[i],"%d", &config.bidder);
			continue;
		}
		if (i==argc-1)
		{
			sscanf(argv[i],"%d", &config.ad);
			continue;
		}
		puts("invalid parameters, expect B, T, r or file");
		return -1;
	}
	if (config.B_base<0||config.T_base<0||config.r_base<0||strlen(config.file)<=1)
	{
		puts("parameters not enough");
		return -1;
	}
	return 0;
}/*}}}*/
