#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
template <class T>
/**
 *
 *Given an array of integers, return indices of the two numbers such that they add up to a specific target.

 You may assume that each input would have exactly one solution, and you may not use the same element twice.

Example:
Given nums = [2, 7, 11, 15], target = 9,

Because nums[0] + nums[1] = 2 + 7 = 9,
return [0, 1].
 *
 * 2017.02.27
 * 
 *
 */
int getLength(T& array){
	return (sizeof(array) / sizeof(array[0]));
}
vector<int> twoSum(vector<int>& nums, int target) {
	vector<int> copy;
	copy.assign(nums.begin(), nums.end());

	sort(nums.begin(), nums.end());

	int i = 0;
	int j = nums.size() - 1;	
	vector<int> res;
	while(i < j){
		if(nums[i] + nums[j] == target){
			res.push_back(nums[i]);
			res.push_back(nums[j]);
			break;
		}else if(nums[i] + nums[j] > target){
			j--;		
		}else{
			i++;
		}
	}
	int index = 0;
	vector<int> ret;
	for(int i = 0; i < copy.size() && res.size()>0; i++) {
		if(copy[i] == res[0] || copy[i] == res[1]){
			ret.push_back(i);
		}	
	}
	return ret;
}
vector<int> twoSum2(vector<int>& nums, int target) {
	vector<int> res;
	map<int, int> m;		
	map<int, int>::iterator it;
	for(int i = 0; i < nums.size(); i++) {
		it = m.find(target - nums[i]);
		if(it != m.end()){
			res.push_back(it->second);				
			res.push_back(i);				
			return res;
		}
		m.insert(pair<int, int>(nums[i], i));

	}

	return res;
}
int main(){
	//int input[] = {2,10,7,11,15};
//	int input[] = {2,1,4};
	int input[] = {3, 3};
//	int input[] = {-1,-2,-3,-4,-5};
	vector<int> a(&input[0], &input[getLength(input)]);
	int target = 6;
	vector<int> res = 	twoSum2(a, target);
	printf("%d + %d = %d\n",res[0], res[1], target);
	return 0;
}
