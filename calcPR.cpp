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
#include<ctime>
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

//-----------------------
Config config;
int vis[320000];
int main(int argc, char *argv[])
{
	if (read_config(config, argc, argv)<0) return -1;
	process("data.csv");

//----------------------------------------------------	
		
	double * B=new double[num_bidders];
	int * T=new int[num_bidders];
	double * r=new double[num_bidders];
		puts("parameters:");
		printf("B: (%d, %d)\n", config.B_base, config.B_range);
		printf("T: (%d, %d)\n", config.T_base, config.T_range);
		printf("r: (%d, %d)\n", config.r_base, config.r_range);
		puts("");
//	freopen(config.file, "w", stdout);
	srand(time(0));
	for (int cas=0;cas<1;cas++)
	{
		for (int i=0;i<320000;i++) vis[i] = 0;
		for (int i=0;i<100000 ;i++) vis[i] = 1;
		random_shuffle(vis, vis+300000);


		init(num_bidders, B, T, r, config);

		Strategy * stg[10];
		int stg_cnt = 6;

		stg[0]	= new BasicPrimal_Dual_ver1(num_bidders, B, T, r, "BA1");
		stg[1]	= new BasicPrimal_Dual_ver2(num_bidders, B, T, r, "BA2");
	//	stg[2]	= new BasicPrimal_Dual_ver3(num_bidders, B, T, r, "BA3");

		stg[2]	= new ContractFirstAllocation(num_bidders, B, T, r, "CA");
		stg[3]	= new GreedyAllocation	(num_bidders, B, T, r, "GA");
		stg[4]	= new AdAllocation	 	(num_bidders, B, T, r, "propose");
		stg[5] = new Balance(num_bidders,B,T,r,"Ba") ;
		int k[10];
		printf("num of auctions :");
		for (int i=0;i<stg_cnt;i++)
		{
			k[i] = simulator(stg[i], vis, ad_per_video);
			printf("%d ",k[i]);
		}
		int only = 1;
		for (int i=0;i<stg_cnt;i++)
		{
			double profit = stg[i]->get_profit(only);
			if (only) printf("%.6lf ",profit/50000);
		}
		puts("");
				
	}
}
/*
 * 10 10
 * 8 8
 * --------
 * 10 10
 * 12 10
 * 12 12
 * */
