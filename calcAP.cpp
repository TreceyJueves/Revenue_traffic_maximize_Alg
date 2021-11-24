/*
 *
 * Distributed under terms of the MIT license.
 */
#include<cstdio>
#include<iostream>
#include<fstream>
#include<cmath>
#include<algorithm>
#include<set>
#include<map>
#include<queue>
#include<string>
#include<cstring>
#include<cstdlib>
#include<cassert>
#include<vector>
#include<unordered_map>
#include"simulator.h"
#include"utils.h"
#define rep(i,a,b) for (int i=a;i<=b;i++)
#define pb push_back
#define ws wss
using namespace std;
typedef long long LL;
typedef pair<int,int> PII;
template<class T> void upmax(T &a,T b) { if (a<b) a=b;}
template<class T> void upmin(T &a,T b) { if (a>b) a=b;}
double r_ori[1200];
//-----------------------
Config config;
int vis[320000];
int main(int argc, char *argv[])
{
	if (read_config(config, argc, argv)<0) return -1;
	process("data.csv");
	int s = 0;
//----------------------------------------------------	
	freopen(config.file, "w", stdout);
//	cout << "data processed"<<endl;	
	double * B=new double[num_bidders];
	int    * T=new int[num_bidders];
	double * r=new double[num_bidders];

	srand(233);
//	int num_ad = config.ad;
//	int num_bidders = config.bidder; 			//modified
	int num_ad = 40000;
	
	double avg[10]={0};
	double avgp[10]={0};
	double avgavg[10] ={0};
	int stg_cnt = 1;
	double ob[101]={0};
	slots_corp = 0;
	for (int cas=0;cas<20;cas++)
	{
		slots_corp = 0;
		int sim[10];
		for (int i=0;i<320000;i++) vis[i] = 0;
		for (int i=0;i<num_ad;i++) vis[i] = 1;  //modified
		random_shuffle(vis, vis+300000);
		init(num_bidders, B, T, r, config);
	//	corp = cas/10;
	//	remove_R(r,r_ori);
/*		Config configg;
		configg.ad=config.ad;
		configg.bidder=config.bidder;
		configg.B_base = config.B_base*randy(80000,120000)/100000.0;
		configg.B_range = config.B_range*randy(80000,120000)/100000;
		configg.T_range = config.T_range*randy(80000,120000)/100000;
		configg.T_base = config.T_base*randy(80000,120000)/100000;
		configg.r_range = config.r_range*randy(80000,120000)/100000.0;
		configg.r_base = config.r_base*randy(80000,120000)/100000.0;*/
		
//		cout <<"initiated"<<endl;
		Strategy * stg[10];
		
//		cout << "numbidders:"<<num_bidders<<endl;
		stg[1]	= new BasicPrimal_Dual_ver1(num_bidders, B, T, r, "BA1");
		stg[2]	= new BasicPrimal_Dual_ver2(num_bidders, B, T, r, "BA2");
//		stg[2]	= new BasicPrimal_Dual_ver3(num_bidders, B, T, r, "BA3");

		stg[3]	= new ContractFirstAllocation(num_bidders, B, T, r, "CA");
		stg[4]	= new GreedyAllocation	(num_bidders, B, T, r, "GA");
		stg[0]	= new AdAllocation	 	(num_bidders, B, T, r, "propose");
//		stg[5] = new Balance (num_bidders,B,T,r,"Ba");
//		stg[1]  = new Simplex			(num_bidders, B, T, r, "optimal");
//		cout <<"stategy initiated"<<endl;
		for (int i=0;i<stg_cnt;i++){
			ad_rev = 0;
			sim[i] = simulator(stg[i], vis, ad_per_video);
	//		cout << endl;
//			cout <<"simulation stg "<<i <<" finished,"<<"auction time: " << sim[i] <<endl;
		}
		
/*		puts("parameters:");
		printf("B: (%d, %d)\n", configg.B_base, configg.B_range);
		printf("T: (%d, %d)\n", configg.T_base, configg.T_range);
		printf("r: (%d, %d)\n", configg.r_base, configg.r_range);
		puts("");*/
/*		double p[10];
		for (int i=0;i<stg_cnt;i++)
		{
			p[i] = stg[i]->get_profit(0);
		}*/ 
/*		double tmp = p[stg_cnt-1];
		
		for (int i=0;i<stg_cnt;i++){
			avgp[i]=p[i]/sim[i];
//			cout <<i<<": ";
			printf("%.4lf %.4lf ", p[i],avgp[i]);
		}
		cout << endl;
		corp++;	*/
		ob[cas] = (double)slots_corp/sim[0];
		cout << ob[cas] << ' ' << sim[0]<<endl;
	}
/*	for(int i = 0; i < stg_cnt; i++){
		cout <<avg[i] << " "<<avgavg[i]<<" ";
	}*/
	fclose(stdout);
//	cout << s <<endl;
//	cout <<"finished"<<endl; 
	return 0; 
}
/*
 * 10 10
 * 8 8
 * --------
 * 10 10
 * 12 10
 * 12 12
 * */
