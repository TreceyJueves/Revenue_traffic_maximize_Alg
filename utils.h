
const int USER = 40000, ITEM = 900000;
const int N=2800000;
//double tot=0, tot1=0;
#define next nexttt
#define FR first
#define SC second
#define MP make_pair
typedef pair<double, double> PDD;
struct example
{
	int user_id, item_id;
	double finish, like;
};


//---------------------------------
PDD item_feature[num_bidders];
example data[N];
int next[N];int last[USER];//record last data of a certain user
int user_size, item_size;
map<int, PDD> uis[USER];// 存储user与ad(<num_bidders)的pair

PDD operator +(PDD a, PDD b){return PDD(a.FR+b.FR, a.SC+b.SC);}
PDD operator -(PDD a, PDD b){return PDD(a.FR-b.FR, a.SC-b.SC);}
PDD operator /(PDD a, double b){return PDD(a.FR/b, a.SC/b);}

void preprocess(example *data, int data_size)
{/*{{{*/
	/*
	 * 把item和user重编号，freq越大编号越小
	 * */
//	cout << "data preprocessing..."<<endl;
	int* idx= new int[ITEM];
	unordered_map<int,int> idx_map;
	unordered_map<int,int> freqs;
	int size;
	// user_id map
	freqs.clear();idx_map.clear();size=0;
	for (int i=0;i<data_size;i++)
	{
		freqs[data[i].user_id%40000]++;
		if (freqs[data[i].user_id%40000]==1)
			idx[size++]=data[i].user_id%40000; 
	}
	sort(idx, idx+size, [&](int i,int j){ return freqs[i]>freqs[j];});
	
//	puts("first user_id");for (int i=0;i<5;i++) printf("%d ", idx[i]);puts("");
	user_size = size;//num of different users
	for (int i=0;i<size;i++) 
		idx_map[idx[i]]=i;
	for (int i=0;i<data_size;i++)
		data[i].user_id = idx_map[data[i].user_id%40000];//user重命名
	for (int i=0;i<size;i++) 
		idx[i]=0;
//	cout <<"user sorted, size: "<<size <<endl;
	// item_id map
	freqs.clear();idx_map.clear();size=0;
	for (int i=0;i<data_size;i++)
	{
		freqs[data[i].item_id]++;
		if (freqs[data[i].item_id]==1)
			idx[size++]=data[i].item_id;
	}
	sort(idx, idx+size, [&](int i,int j){ return freqs[i]>freqs[j];});
	
//	puts("first item_id");for (int i=0;i<5;i++) printf("%d ", idx[i]);puts("");
	item_size = size;//item重命名

	for (int i=0;i<size;i++) 
		idx_map[idx[i]]=i;
	for (int i=0;i<data_size;i++) 
		data[i].item_id = idx_map[data[i].item_id];
	for (int i=0;i<size;i++) 
		idx[i]=0;
	delete []idx;
//	cout <<"item sorted, size: "<<size<<endl;
}/*}}}*/
void cha_b(double*b){
	for(int i = 0; i < num_bidders/2;i++)
		b[i]/=20;
	
}
int simulator(Strategy* strategy, int * vis, int ad_per_video = 7)
{
	double * b=new double[num_bidders];
	int num_auction=0;
	double b1=100,b2=-1,bsum=0;

	int kth=0;
	for (int user_id=0;user_id<USER;user_id++)
	{
	//	if(user_id%10000==0)
	//		cout << "processing user_id: " << user_id << endl; 
		for (int j=last[user_id],cnt=0;j!=-1;j=next[j])//trace?
		{
			cnt++;
			if (cnt==ad_per_video)//is ad？这段完全没看懂
			{
				cnt=0;
				
		//		if (!vis[kth++]) continue;//这在干啥
		//		cout<<"ads found"<<endl;
		//		tot = 0;
				for (int bidder_id=0;bidder_id<num_bidders;bidder_id++){//price according to user behavior or avg behavior
		//			if(bidder_id%500==0)
		//				cout << "bidder 500"<<endl;
					if (uis[user_id].count(bidder_id))
						b[bidder_id] = data[j].finish * uis[user_id][bidder_id].FR;
				/*
				 * 广告点击率*位置点击率 finish
				 * */
					else
						b[bidder_id] = data[j].finish * item_feature[bidder_id].FR;
	//				tot+=b[bidder_id];
					
				}
		//		tot/=1000;
		//		tot1+=tot;
			//	cout << "b[0] ="<<b[0]<<endl;
			//	cout << "num_auction: "<<num_auction<<endl;
				if(user_id %2==0){
					cha_b(b);
				}
				int ok=strategy->auction(b);
		//		cout << "ok = "<<ok<<endl;
				num_auction++;
	//			if(num_auction % 1000==0)
	//				cout <<"checkpoint "<<num_auction<<endl; 
				if (ok) return num_auction;
				if (num_auction == auction_limit) break;
			}
		}
		
		if (num_auction == auction_limit) break;
	}
//	cout << "num_auction:"<<num_auction<<endl;
	return num_auction;
	//printf("auction num: %d\n",num_auction);
}
void process(const char* file_name) 
{/*{{{*/
	freopen(file_name, "r", stdin);
	char s[100];
	scanf("%s",s);
	int data_size = 0;
	while (1)
	{
		int user_id, item_id;
		double finish, like;
		int u=scanf("%d,%d,%lf,%lf",&user_id, &item_id, &finish, &like);
		if (u<=0) break;
		if (finish<=1 && like<=1) 
		{
			data[data_size]=(example){user_id, item_id, finish, like};
			data_size++;
		}
		if(data_size > 2000000)
			break;
	}
//	printf("data size: %d\n", data_size);
	preprocess(data, data_size);
//	cout << "data preprocessed"<<endl;
	// get user_trace
	int* item_count = new int[num_bidders];
	for (int i=0;i<num_bidders;i++) item_count[i]=0;
	for (int i=0;i<USER;i++) last[i]=-1, uis[i].clear();
	for (int i=0;i<data_size;i++)
	if (data[i].item_id<num_bidders)//ads
	{
		int item_id = data[i].item_id;
		int user_id = data[i].user_id;
		item_feature[item_id] = item_feature[item_id] + MP(data[i].finish, data[i].like);
		item_count[item_id]++;
		uis[user_id][item_id]= MP(data[i].finish, data[i].like);
	}
	else
	{
		int user_id = data[i].user_id;
		next[i] = last[user_id];
		last[user_id] = i;
	}
//	printf("user size: %d\n", user_size);
//	printf("item size: %d\n", item_size);
	for (int i=0;i<num_bidders;i++)
	{
		item_feature[i] = item_feature[i] / item_count[i];
/*		assert(item_feature[i].FR > 0.44);
		assert(item_feature[i].SC > 0.43);
		assert(item_feature[i].FR < 0.55);
		assert(item_feature[i].SC < 0.55);
*/	}
	delete [] item_count;
}/*}}}*/
