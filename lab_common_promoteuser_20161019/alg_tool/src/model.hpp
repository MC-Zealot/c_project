//============================================================================
// Name        : model.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<string>
#include<vector>
#include<math.h>
#include<map>
#include<sstream>
#include<fstream>
#include<time.h>
using namespace std;





const int tagnum = 6;
const int scenum = 20;
const int Tcount = 5;



vector<string> split(const std::string& str, std::string str_del)
{
	int save_space = 0;
        int lastSpacePos = 0;
	vector<string> vec;
        for(int i=0;i<str.length();i++)
        {
                //if(str[i] == delimiter)
                 int flag = 0;
                 if(str_del.find(str[i]) != std::string::npos)
                 {
                         std::string subStr = str.substr(lastSpacePos,i-lastSpacePos);
                         if(strcmp(subStr.c_str(),"") != 0 || save_space > 0)
                                vec.push_back(subStr);
                         //else
                         //cout<<"split space"<<endl;
                         lastSpacePos = i + 1;
                         flag = 1;
                 }
                 if(i == str.length()-1 && flag == 0)
                 {
                         std::string subStr = str.substr(lastSpacePos,i+1-lastSpacePos);
                         if(subStr.length() > 0)
                                 vec.push_back(subStr);
                         //return vec;
                 }
        }
        return vec;
}


/*
vector<string> split(string str,string pattern){

	string::size_type pos;
	vector<string> result;
	str += pattern;
	int size = str.size();
	cout <<size<<endl;
	for(int i = 0; i < size; i++){
			pos = str.find(pattern,i);
			if(pos < size){
				string s = str.substr(i,pos-i);
				result.push_back(s);
				i = pos + pattern.size() - 1;
			}
		}
		return result;
}

*/
map<string,vector<float> > readmap(char* filename){

	ifstream input(filename);
	string line;
	vector<string> vec;
	string key;
	string value;
	map<string,vector<float> > mapvalues;
	while(getline(input,line)){
		vec = split(line,"\t");
		vector<float> vecvalue;
		key = vec[0];
		value = vec[1];
		vec = split(value,",");
		for(int i = 0; i < vec.size(); i++){
			vecvalue.push_back(atof(vec[i].c_str()));
		}
		mapvalues[key] = vecvalue;
	}
	input.close();
	return mapvalues;
}

double distancefunction(int distance,double var){
		double result = 0.0;
		result = exp(log(1/(sqrt(2*M_PI*var))) - pow(distance, 2)/(2*var));
		return result;
}

double Gaussfunctiontime(int time,vector<float> arr){


		double mean;
		double var;
		int judge = 0;//0代表以0位中心点，1代表以12点为中心点。

		if(arr[2] <= arr[4]){
			judge = 1;
			mean = arr[1];
			var = arr[2];
		}else{
			judge = 0;
			mean = arr[3];
			var = arr[4];
		}

		double value = 0.0;
		if(var == 0.0)
			return 0.0;

		if(time <= 720){
			value = exp(log(1.0/(sqrt(2*M_PI*var)))-pow(time-mean, 2)/(2*var));
		}else{
				if(judge == 1){
					value = exp(log(1.0/(sqrt(2*M_PI)*sqrt(var)))-pow(time-mean, 2)/(2*var));
				}else{
					value = exp(log(1.0/(sqrt(2*M_PI)*sqrt(var)))-pow(time-1440-mean, 2)/(2*var));
				}
			}
		return value;
	}


string itoa(int n){
    stringstream ss;
    string str;
    ss<<n;
    ss>>str;
    return str;
}

double computetimeprob(vector<float> usevector,string week,string wifi,string poi,int time,int sce){
		int startpois1 = scenum+sce*Tcount+1;
		int startpois2 = scenum+scenum*Tcount + Tcount*5*sce+1;
		float sum = 0.0;
		vector<float> timefloat;
		for(int i = 0; i < Tcount; i++){
			timefloat.clear();
			for(int j = 0; j < 5; j++){
				timefloat.push_back(usevector[startpois2+i*5+j]);
			}
			sum += usevector[startpois1+i]*Gaussfunctiontime(time,timefloat);
		}
		return sum;
}

map<string,double> computeTagProb(map<string,vector<float> > &mapvalues,string week,string wifi,int time,string pois){



		clock_t start,end;
		start = clock();
		map<string,double> tagprobmap;
		map<string,double>  poismap ;
		map<string,double>::iterator mapit;

		if(pois == "" || week == "" || wifi == "" || time >1440 || time < 0){
			return tagprobmap;
		}
		vector<string> poistoken = split(pois,";");
		vector<string> splittokens ;
		vector<string> poitag;
		vector<float> ust;
		map<int,string>tagmap;
		map<string,vector<float> > usemap;
		tagmap[0] = "10905";
		tagmap[3]= "10902";
		tagmap[4]= "10903";
		tagmap[2]= "10904";
		tagmap[1]= "10901";
		tagmap[5]= "10906";
		double tmpvalue;
		string value;
		double prob;

		vector<double> tagprob;
		for(int i = 0; i < tagnum; i++){
			tagprob.push_back(0.0);
		}
		
		string weekwifipoitag = "";

		for(int i = 0; i < poistoken.size(); i++){
			splittokens = split(poistoken[i],",");
			poitag = split(splittokens[1]," ");
			for(int k = 0; k < poitag.size(); k++){
				value = poitag[k];
				prob = distancefunction(atoi(splittokens[2].c_str()),9200);
				if(poismap.find(value) != poismap.end()){
					poismap[value] = poismap[value]+prob;
				}else{
					poismap[value] = prob;
				}
				
				weekwifipoitag = week+"_"+wifi+"_"+value;
				if(mapvalues.find(weekwifipoitag) == mapvalues.end())
					return tagprobmap;
					
				usemap[value] = mapvalues[weekwifipoitag];
			}
		}
		

		double sum = 0.0;
		double tempsum = 0.0;
		string key = "";
		for(int i = 0; i < scenum; i++){
			sum = 0.0;
			tempsum = 0.0;

			for(mapit = poismap.begin(); mapit != poismap.end(); mapit++){
				tempsum = 0.0;
				tempsum += log(mapit->second);
				key = mapit->first;
				tmpvalue = usemap[key][0];
				tempsum += log(tmpvalue);
				tempsum += log(computetimeprob(usemap[key],week,wifi,mapit->first,time,i));
				tmpvalue = usemap[mapit->first][i+1];
				tempsum += log(tmpvalue);
				sum += exp(tempsum);
			}
			ust = mapvalues[itoa(i)];
			for(int j = 0; j < tagnum ; j++){
				tagprob[j] += ust[j]*sum;
			}
			
		}
		
		sum = 0.0;

		for(int i = 0; i < tagnum; i++){
			sum += tagprob[i];	
		}		

		for(int i = 0; i < tagnum; i++){
			tagprobmap[tagmap[i]] = tagprob[i]/sum;
		}

		
		end = clock();
		double dur = (double)(end-start)/CLOCKS_PER_SEC;
		return tagprobmap;
		/*
		string tagprobstr = "";
		double maxvalue = 0.0;
		int index = 0;
		int count = 0;
		int ordernum[tagnum];
		for(int i = 0; i < tagnum; i++){
			ordernum[i] = i;
		}
		int tagcount = tagnum;
		while(count < 3){
			maxvalue = 0.0;
			for(int k = 0; k < tagcount; k++){
				if(tagprob[k] > maxvalue){
					maxvalue = tagprob[k];
					index = k;
				}
			}

			tagprobstr = tagprobstr + tagmap[ordernum[index]]+",";
			tagprob[index] = tagprob[tagcount-1];
			ordernum[index] = tagcount - 1;
			tagcount = tagcount - 1;
			count++;
		}
		return tagprobstr;
		*/	
	}
/*
int main(int argc, char* argv[]) {
	
//	map<string,vector<float> > mapvalues = readmap(argv[1]);
	string week = "1";
	string wifi = "1";
	int newtime= 660;
	
	string pois= "1234,64 78,50;2345,194,50";
	map<string,double> res = computeTagProb(mapvalues,week,wifi,newtime,pois);
	map<string,double>::iterator mapit;
	for(mapit = res.begin(); mapit != res.end(); mapit++){
		cout <<mapit->first<<","<<mapit->second<<endl;
	}
	string poi = "";
	vector<string> vec = split(poi,";");
	cout << vec.size()<<endl;
	for(int i = 0; i < vec.size(); i++){
		cout <<vec[i]<<endl;
	}
	return 0;
}
*/
