#include <iostream>
#include <string>
#include <vector>

using namespace std;
int maxSubArray2(vector<int> nums);
int getMax(int a, int b);
/**
 *Say you have an array for which the ith element is the price of a given stock on day i.

 If you were only permitted to complete at most one transaction (ie, buy one and sell one share of the stock), 

 design an algorithm to find the maximum profit.
 *
 *
 */
int maxProfit(vector<int>& prices) {

	int res = 0;
	if(prices.size()<=1){
		return res;
	}
	vector<int> nums;
	nums.push_back(0);
	//nums.assign(prices.begin(),prices.end());

	cout<<nums.size()<<endl;
	for(int i = 1; i < prices.size(); i++) {
		nums.push_back(prices[i] - prices[i - 1]);		
	}
	res = maxSubArray2(nums);
	for(int i = 0; i < nums.size(); i++) {
		printf("%d: %d\n",i ,nums[i]);
	}
	return res;
}
int maxSubArray2(vector<int> nums){
	int maxEndingHere = nums[0], maxSoFar = nums[0];
	for (int i = 1; i < nums.size(); i++) {
		maxEndingHere = getMax(maxEndingHere + nums[i], nums[i]);
		maxSoFar = getMax(maxEndingHere, maxSoFar);
	}
	return maxSoFar;
}
int getMax(int a, int b){
	if(a>=b){
		return a;
	}else{
		return b;
	}
}
int main (){
	int n[] = {1, 5, 4, 7, 2} ;
	vector<int> a(&n[0], &n[5]) ;  
	//	cout<<a.size()<<endl;
	//	for(int i = 0; i < a.size(); i++){
	//		cout<<a[i]<<endl;
	//	}
	printf("result: %d\n", maxProfit(a));
	return 0;
}
