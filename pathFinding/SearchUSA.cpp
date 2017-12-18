//Author: Deepak Gupta
//Code implements path finding in a given graph
//by using Astar, Greedy and Uniform search

#include<iostream>
#include<list>
#include<map>
#include<queue>
#include<stack>
#include<math.h>
#include<string.h>
#include<limits.h>

using namespace std;

#define WHITE 0
#define GREY 1
#define BLACK 2

void createGraph();
void addEdge(string u, string v, float distance);
void createLocationMap();
void addLocation(string cityName, float latitude, float longitude);
list<string> astar(string source, string destination);
list<string> greedy(string source, string destination);
list<string> uniform(string source, string destination);
void printOutPath(list<string> &outputPath);
float calculateHeuristic(string source, string destination);

//datastructure to store neighbour info of a node
struct neighbourInfo
{
	string cityName;
	float distance;
};

struct cityLocation
{
	float latitude;
	float longitude;
};

//datastructre to store expanded path and the corresponding
//distance of the last node on the path from the source
//pathCost indicates the cumulative cost which is used
//as a parameter for finding priority in the priority queue
struct pqData
{
	list<string> path;
	float distFromSource;
	float pathCost;
};

//This class helps in deciding the priority of an 
//element in the priority queue. The element with
//minimum pathCost appears on the top of the queue.
class comp
{
public:
    bool operator() (pqData A, pqData B)
    {
        return (A.pathCost > B.pathCost);
    }
};

//graph to store a node and info of its adjacent nodes
map< string, list<neighbourInfo> > graph;

//map to store laitude and logitude of a location
map<string, cityLocation> cityLocationMap;
int totalDistance = 0;


int main(int argc, char *argv[])
{
	//function called to create graph
	createGraph();

	//function called to populate latitude
	//and logitude of a location
	createLocationMap();
	list<string> outputPath;

	if(argc != 4)
	{
		cout<<"Please enter proper number of command line arguments\n";
		return 1;
	}

	if(strcmp(argv[1], "astar") == 0)
	{
		outputPath = astar(argv[2], argv[3]);
		printOutPath(outputPath);
	}
	else if(strcmp(argv[1], "greedy") == 0)
	{
		outputPath = greedy(argv[2], argv[3]);
		printOutPath(outputPath);
	}
	else if(strcmp(argv[1], "uniform") == 0)
	{
		outputPath = uniform(argv[2], argv[3]);
		printOutPath(outputPath);	
	}
	else
	{
		cout<<"Please enter proper search type i.e either astar, greedy or uniform (case sensetive)\n";
		return 2;
	}

	return 0;
}

void printOutPath(list<string> &outputPath)
{
	list<string>::iterator itr;

	if(outputPath.empty())
		cout<<"No path from source to destination"<<endl;
	else
	{
		cout<<"Path from source to destination is: ";
		for(itr=outputPath.begin(); itr!=outputPath.end(); itr++)
			cout<<*itr<<", ";
		cout<<endl<<endl;
		cout<<"Number of nodes in the path = "<<outputPath.size()<<endl<<endl;

		cout<<"Total distance from source to destination is: "<<totalDistance<<endl;
	}	
}

list<string> astar(string source, string destination)
{
	priority_queue<pqData, vector<pqData>, comp> pqueue;
	list<string>::iterator itr;
	list<neighbourInfo>::iterator neighbourItr;
	list<string> emptyPath;
	list<string> nodesExpanded; //variable to track nodes expanded

	//checking if source or destination exists in the graph
	map< string, list<neighbourInfo> >::iterator graphItr;
	graphItr = graph.find(source);
	if(graphItr == graph.end())
		return emptyPath;

	graphItr = graph.find(destination);
	if(graphItr == graph.end())
		return emptyPath;

	//priority queue is initialized with a source
	pqData pqVar;
	pqVar.path.push_back(source);
	pqVar.distFromSource = 0;	
	pqVar.pathCost = calculateHeuristic(source, destination);
	pqueue.push(pqVar);

 	while(!pqueue.empty())
	{
		pqVar = pqueue.top();
		pqueue.pop();
	
		//if last node of the path is equal to destination then return the path
		if((pqVar.path).back() == destination)
		{
			cout<<"Nodes expanded are: ";
			for(itr=nodesExpanded.begin(); itr!=nodesExpanded.end(); itr++)
				cout<<*itr<<", ";
			cout<<endl<<endl;
			cout<<"Number of nodes expanded are: "<<nodesExpanded.size()<<endl<<endl;

			totalDistance = pqVar.distFromSource;
			return (pqVar.path);
		}

		itr = (pqVar.path).end();		
		if((pqVar.path).size() == 1)
		{
			itr--;
		}
		else
		{
			itr--;
			itr--;
		}

		string parentNode = *itr;

		list<string> checkNodesList, finalDeleteNodesList;
		map<string, int> checkNodesMap, finalDeleteNodesMap;

		//if last node of the path is not equal to destination then expand the node
		//in all possible ways
		for(neighbourItr=graph[(pqVar.path).back()].begin(); neighbourItr!=graph[(pqVar.path).back()].end(); neighbourItr++)
		{
			if((*neighbourItr).cityName != parentNode)
			{
				pqData pqSuccVar;
				pqSuccVar.path = pqVar.path;
				pqSuccVar.path.push_back((*neighbourItr).cityName);
				pqSuccVar.distFromSource = pqVar.distFromSource + (*neighbourItr).distance;
				pqSuccVar.pathCost = pqSuccVar.distFromSource + calculateHeuristic((*neighbourItr).cityName, destination);
				pqueue.push(pqSuccVar);
				checkNodesList.push_back((*neighbourItr).cityName);
				checkNodesMap[(*neighbourItr).cityName] = pqSuccVar.pathCost;	
			}
		}

		//after processing is done make the node black and add in expanded nodes list
		nodesExpanded.push_back((pqVar.path).back());		

		//If two or more paths reach the same node, delete all paths except 
		//the one of min cost		
		priority_queue<pqData, vector<pqData>, comp> pqueue1 = pqueue;
		priority_queue<pqData, vector<pqData>, comp> pqueueFinal;

		list<string>::iterator checkNodesListItr;
		for(checkNodesListItr=checkNodesList.begin(); checkNodesListItr!=checkNodesList.end(); checkNodesListItr++)
		{
			int min1 = INT_MAX, min2 = INT_MAX;
			while(!pqueue1.empty())
			{
				pqData pqVarTemp1 = pqueue1.top();
				pqueue1.pop();
				if(((pqVarTemp1.path).back() == *checkNodesListItr) && checkNodesMap[*checkNodesListItr] < min1)
				{
					min2 = min1;
					min1 = checkNodesMap[*checkNodesListItr];
				}
			}

			if((min1 != min2) && min2 != INT_MAX)
			{
				finalDeleteNodesMap[*checkNodesListItr] = min1;
			} 

			pqueue1 = pqueue;
		}

		pqueue1 = pqueue;
		while(!pqueue1.empty())
		{
			pqData pqVarTemp1 = pqueue1.top();
			pqueue1.pop();
			map<string, int>::iterator finalDeleteNodesMapItr;
			finalDeleteNodesMapItr = finalDeleteNodesMap.find((pqVarTemp1.path).back());
			if(finalDeleteNodesMapItr == finalDeleteNodesMap.end())
			{
				pqueueFinal.push(pqVarTemp1);
			}
			else if(finalDeleteNodesMap[(pqVarTemp1.path).back()] == pqVarTemp1.pathCost)
			{
				pqueueFinal.push(pqVarTemp1);
			}
		}		

		pqueue = pqueueFinal;
	}
	
	return emptyPath;
}

list<string> greedy(string source, string destination)
{
	priority_queue<pqData, vector<pqData>, comp> pqueue;
	list<string>::iterator itr;
	list<neighbourInfo>::iterator neighbourItr;
	list<string> emptyPath;
	list<string> nodesExpanded; //variable to track nodes expanded

	//checking if source or destination exists in the graph
	map< string, list<neighbourInfo> >::iterator graphItr;
	graphItr = graph.find(source);
	if(graphItr == graph.end())
		return emptyPath;

	graphItr = graph.find(destination);
	if(graphItr == graph.end())
		return emptyPath;

	//priority queue is initialized with a source
	pqData pqVar;
	pqVar.path.push_back(source);
	pqVar.distFromSource = 0;	
	pqVar.pathCost = calculateHeuristic(source, destination);
	pqueue.push(pqVar);

 	while(!pqueue.empty())
	{
		pqVar = pqueue.top();
		pqueue.pop();

		//if last node of the path is equal to destination then return the path
		if((pqVar.path).back() == destination)
		{
			cout<<"Nodes expanded are: ";
			for(itr=nodesExpanded.begin(); itr!=nodesExpanded.end(); itr++)
				cout<<*itr<<", ";
			cout<<endl<<endl;
			cout<<"Number of nodes expanded are: "<<nodesExpanded.size()<<endl<<endl;

			totalDistance = pqVar.distFromSource;
			return (pqVar.path);
		}
		
		itr = (pqVar.path).end();		
		if((pqVar.path).size() == 1)
		{
			itr--;
		}
		else
		{
			itr--;
			itr--;
		}

		string parentNode = *itr;

		//if last node of the path is not equal to destination then expand the node
		//in all possible ways
		for(neighbourItr=graph[(pqVar.path).back()].begin(); neighbourItr!=graph[(pqVar.path).back()].end(); neighbourItr++)
		{
			if((*neighbourItr).cityName != parentNode)
			{
				pqData pqSuccVar;
				pqSuccVar.path = pqVar.path;
				pqSuccVar.path.push_back((*neighbourItr).cityName);
				pqSuccVar.distFromSource = pqVar.distFromSource + (*neighbourItr).distance;
				pqSuccVar.pathCost = calculateHeuristic((*neighbourItr).cityName, destination);
				pqueue.push(pqSuccVar);
			}
		}

		//after processing is done add the node in the expanded nodes list
		nodesExpanded.push_back((pqVar.path).back());
	}
	
	return emptyPath;
}

list<string> uniform(string source, string destination)
{
	priority_queue<pqData, vector<pqData>, comp> pqueue;
	list<string>::iterator itr;
	list<neighbourInfo>::iterator neighbourItr;
	list<string> emptyPath;
	list<string> nodesExpanded; //variable to track nodes expanded

	//checking if source or destination exists in the graph
	map< string, list<neighbourInfo> >::iterator graphItr;
	graphItr = graph.find(source);
	if(graphItr == graph.end())
		return emptyPath;

	graphItr = graph.find(destination);
	if(graphItr == graph.end())
		return emptyPath;

	//priority queue is initialized with the source
	pqData pqVar;
	pqVar.path.push_back(source);
	pqVar.distFromSource = 0;	
	pqVar.pathCost = 0;
	pqueue.push(pqVar);

 	while(!pqueue.empty())
	{
		pqVar = pqueue.top();
		pqueue.pop();

		//if last node of the path is equal to destination then return the path
		if((pqVar.path).back() == destination)
		{
			cout<<"Nodes expanded are: ";
			for(itr=nodesExpanded.begin(); itr!=nodesExpanded.end(); itr++)
				cout<<*itr<<", ";
			cout<<endl<<endl;
			cout<<"Number of nodes expanded are: "<<nodesExpanded.size()<<endl<<endl;
			
			totalDistance = pqVar.distFromSource;
			return (pqVar.path);
		}
		
		itr = (pqVar.path).end();		
		if((pqVar.path).size() == 1)
		{
			itr--;
		}
		else
		{
			itr--;
			itr--;
		}

		string parentNode = *itr;

		list<string> checkNodesList, finalDeleteNodesList;
		map<string, int> checkNodesMap, finalDeleteNodesMap;

		//if last node of the path is not equal to destination then expand the node
		//in all possible ways
		for(neighbourItr=graph[(pqVar.path).back()].begin(); neighbourItr!=graph[(pqVar.path).back()].end(); neighbourItr++)
		{
			if((*neighbourItr).cityName != parentNode)
			{
				pqData pqSuccVar;
				pqSuccVar.path = pqVar.path;
				pqSuccVar.path.push_back((*neighbourItr).cityName);
				pqSuccVar.distFromSource = pqVar.distFromSource + (*neighbourItr).distance;
				pqSuccVar.pathCost = pqVar.pathCost + (*neighbourItr).distance;
				pqueue.push(pqSuccVar);
			}
		}

		//after processing is done make the node black and add in expanded nodes list
		nodesExpanded.push_back((pqVar.path).back());

		//If two or more paths reach the same node, delete all paths except 
		//the one of min cost		
		priority_queue<pqData, vector<pqData>, comp> pqueue1 = pqueue;
		priority_queue<pqData, vector<pqData>, comp> pqueueFinal;

		list<string>::iterator checkNodesListItr;
		for(checkNodesListItr=checkNodesList.begin(); checkNodesListItr!=checkNodesList.end(); checkNodesListItr++)
		{
			int min1 = INT_MAX, min2 = INT_MAX;
			while(!pqueue1.empty())
			{
				pqData pqVarTemp1 = pqueue1.top();
				pqueue1.pop();
				if(((pqVarTemp1.path).back() == *checkNodesListItr) && checkNodesMap[*checkNodesListItr] < min1)
				{
					min2 = min1;
					min1 = checkNodesMap[*checkNodesListItr];
				}
			}

			if((min1 != min2) && min2 != INT_MAX)
			{
				finalDeleteNodesMap[*checkNodesListItr] = min1;
			} 

			pqueue1 = pqueue;
		}

		pqueue1 = pqueue;
		while(!pqueue1.empty())
		{
			pqData pqVarTemp1 = pqueue1.top();
			pqueue1.pop();
			map<string, int>::iterator finalDeleteNodesMapItr;
			finalDeleteNodesMapItr = finalDeleteNodesMap.find((pqVarTemp1.path).back());
			if(finalDeleteNodesMapItr == finalDeleteNodesMap.end())
			{
				pqueueFinal.push(pqVarTemp1);
			}
			else if(finalDeleteNodesMap[(pqVarTemp1.path).back()] == pqVarTemp1.pathCost)
			{
				pqueueFinal.push(pqVarTemp1);
			}
		}		

		pqueue = pqueueFinal;

	}
	
	return emptyPath;
}

void addEdge(string u, string v, float distance)
{
	neighbourInfo neighbourInfoVar;
	neighbourInfoVar.cityName = v;
	neighbourInfoVar.distance = distance;
	graph[u].push_back(neighbourInfoVar);

	neighbourInfoVar.cityName = u;
	graph[v].push_back(neighbourInfoVar);
}

void addLocation(string cityName, float latitude, float longitude)
{
	cityLocation cityLocationVar;
	cityLocationVar.latitude = latitude;
	cityLocationVar.longitude = longitude;
	cityLocationMap[cityName] = cityLocationVar;
}

float calculateHeuristic(string source, string destination)
{
	float lat1 = cityLocationMap[source].latitude;
	float long1 = cityLocationMap[source].longitude;

	float lat2 = cityLocationMap[destination].latitude;
	float long2 = cityLocationMap[destination].longitude;
	float pi = 3.141593;

	float heuristic = sqrt(pow((69.5 * (lat1 - lat2)), 2.0) 
		+ pow((69.5 * cos((lat1 + lat2)/360.0 * pi) * (long1 - long2)), 2.0));

	if(heuristic < 0)
		heuristic *= -1;
	return heuristic;
}

void createLocationMap()
{
	addLocation("albanyGA",        31.58,  84.17);
	addLocation("albanyNY",        42.66,  73.78);
	addLocation("albuquerque",     35.11, 106.61);
	addLocation("atlanta",         33.76,  84.40);
	addLocation("augusta",         33.43,  82.02);
	addLocation("austin",          30.30,  97.75);
	addLocation("bakersfield",     35.36, 119.03);
	addLocation("baltimore",       39.31,  76.62);
	addLocation("batonRouge",      30.46,  91.14);
	addLocation("beaumont",        30.08,  94.13);
	addLocation("boise",           43.61, 116.24);
	addLocation("boston",          42.32,  71.09);
	addLocation("buffalo",         42.90,  78.85);
	addLocation("calgary",         51.00, 114.00);
	addLocation("charlotte",       35.21,  80.83);
	addLocation("chattanooga",     35.05,  85.27);
	addLocation("chicago",         41.84,  87.68);
	addLocation("cincinnati",      39.14,  84.50);
	addLocation("cleveland",       41.48,  81.67);
	addLocation("coloradoSprings", 38.86, 104.79);
	addLocation("columbus",        39.99,  82.99);
	addLocation("dallas",          32.80,  96.79);
	addLocation("dayton",          39.76,  84.20);
	addLocation("daytonaBeach",    29.21,  81.04);
	addLocation("denver",          39.73, 104.97);
	addLocation("desMoines",       41.59,  93.62);
	addLocation("elPaso",          31.79, 106.42);
	addLocation("eugene",          44.06, 123.11);
	addLocation("europe",          48.87,  -2.33);
	addLocation("ftWorth",         32.74,  97.33);
	addLocation("fresno",          36.78, 119.79);
	addLocation("grandJunction",   39.08, 108.56);
	addLocation("greenBay",        44.51,  88.02);
	addLocation("greensboro",      36.08,  79.82);
	addLocation("houston",         29.76,  95.38);
	addLocation("indianapolis",    39.79,  86.15);
	addLocation("jacksonville",    30.32,  81.66);
	addLocation("japan",           35.68, 220.23);
	addLocation("kansasCity",      39.08,  94.56);
	addLocation("keyWest",         24.56,  81.78);
	addLocation("lafayette",       30.21,  92.03);
	addLocation("lakeCity",        30.19,  82.64);
	addLocation("laredo",          27.52,  99.49);
	addLocation("lasVegas",        36.19, 115.22);
	addLocation("lincoln",         40.81,  96.68);
	addLocation("littleRock",      34.74,  92.33);
	addLocation("losAngeles",      34.03, 118.17);
	addLocation("macon",           32.83,  83.65);
	addLocation("medford",         42.33, 122.86);
	addLocation("memphis",         35.12,  89.97);
	addLocation("mexia",           31.68,  96.48);
	addLocation("mexico",          19.40,  99.12);
	addLocation("miami",           25.79,  80.22);
	addLocation("midland",         43.62,  84.23);
	addLocation("milwaukee",       43.05,  87.96);
	addLocation("minneapolis",     44.96,  93.27);
	addLocation("modesto",         37.66, 120.99);
	addLocation("montreal",        45.50,  73.67);
	addLocation("nashville",       36.15,  86.76);
	addLocation("newHaven",        41.31,  72.92);
	addLocation("newOrleans",      29.97,  90.06);
	addLocation("newYork",         40.70,  73.92);
	addLocation("norfolk",         36.89,  76.26);
	addLocation("oakland",         37.80, 122.23);
	addLocation("oklahomaCity",    35.48,  97.53);
	addLocation("omaha",           41.26,  96.01);
	addLocation("orlando",         28.53,  81.38);
	addLocation("ottawa",          45.42,  75.69);
	addLocation("pensacola",       30.44,  87.21);
	addLocation("philadelphia",    40.72,  76.12);
	addLocation("phoenix",         33.53, 112.08);
	addLocation("pittsburgh",      40.40,  79.84);
	addLocation("pointReyes",      38.07, 122.81);
	addLocation("portland",        45.52, 122.64);
	addLocation("providence",      41.80,  71.36);
	addLocation("provo",           40.24, 111.66);
	addLocation("raleigh",         35.82,  78.64);
	addLocation("redding",         40.58, 122.37);
	addLocation("reno",            39.53, 119.82);
	addLocation("richmond",        37.54,  77.46);
	addLocation("rochester",       43.17,  77.61);
	addLocation("sacramento",      38.56, 121.47);
	addLocation("salem",           44.93, 123.03);
	addLocation("salinas",         36.68, 121.64);
	addLocation("saltLakeCity",    40.75, 111.89);
	addLocation("sanAntonio",      29.45,  98.51);
	addLocation("sanDiego",        32.78, 117.15);
	addLocation("sanFrancisco",    37.76, 122.44);
	addLocation("sanJose",         37.30, 121.87);
	addLocation("sanLuisObispo",   35.27, 120.66);
	addLocation("santaFe",         35.67, 105.96);
	addLocation("saultSteMarie",   46.49,  84.35);
	addLocation("savannah",        32.05,  81.10);
	addLocation("seattle",         47.63, 122.33);
	addLocation("stLouis",         38.63,  90.24);
	addLocation("stamford",        41.07,  73.54);
	addLocation("stockton",        37.98, 121.30);
	addLocation("tallahassee",     30.45,  84.27);
	addLocation("tampa",           27.97,  82.46);
	addLocation("thunderBay",      48.38,  89.25);
	addLocation("toledo",          41.67,  83.58);
	addLocation("toronto",         43.65,  79.38);
	addLocation("tucson",          32.21, 110.92);
	addLocation("tulsa",           36.13,  95.94);
	addLocation("uk1",             51.30,   0.00);
	addLocation("uk2",             51.30,   0.00);
	addLocation("vancouver",       49.25, 123.10);
	addLocation("washington",      38.91,  77.01);
	addLocation("westPalmBeach",   26.71,  80.05);
	addLocation("wichita",         37.69,  97.34);
	addLocation("winnipeg",        49.90,  97.13);
	addLocation("yuma",            32.69, 114.62);
}

void createGraph()
{
	//adding edges
	addEdge("albanyNY", "montreal", 226);
	addEdge("albanyNY", "boston", 166);
	addEdge("albanyNY", "rochester", 148);
	addEdge("albanyGA", "tallahassee", 120);
	addEdge("albanyGA", "macon", 106);
	addEdge("albuquerque", "elPaso", 267);
	addEdge("albuquerque", "santaFe", 61);
	addEdge("atlanta", "macon", 82);
	addEdge("atlanta", "chattanooga", 117);
	addEdge("augusta", "charlotte", 161);
	addEdge("augusta", "savannah", 131);
	addEdge("austin", "houston", 186);
	addEdge("austin", "sanAntonio", 79);
	addEdge("bakersfield", "losAngeles", 112);
	addEdge("bakersfield", "fresno", 107);
	addEdge("baltimore", "philadelphia", 102);
	addEdge("baltimore", "washington", 45);
	addEdge("batonRouge", "lafayette", 50);
	addEdge("batonRouge", "newOrleans", 80);
	addEdge("beaumont", "houston", 69);
	addEdge("beaumont", "lafayette", 122);
	addEdge("boise", "saltLakeCity", 349);
	addEdge("boise", "portland", 428);
	addEdge("boston", "providence", 51);
	addEdge("buffalo", "toronto", 105);
	addEdge("buffalo", "rochester", 64);
	addEdge("buffalo", "cleveland", 191);
	addEdge("calgary", "vancouver", 605);
	addEdge("calgary", "winnipeg", 829);
	addEdge("charlotte", "greensboro", 91);
	addEdge("chattanooga", "nashville", 129);
	addEdge("chicago", "milwaukee", 90);
	addEdge("chicago", "midland", 279);
	addEdge("cincinnati", "indianapolis", 110);
	addEdge("cincinnati", "dayton", 56);
	addEdge("cleveland", "pittsburgh", 157);
	addEdge("cleveland", "columbus", 142);
	addEdge("coloradoSprings", "denver", 70);
	addEdge("coloradoSprings", "santaFe", 316);
	addEdge("columbus", "dayton", 72);
	addEdge("dallas", "denver", 792);
	addEdge("dallas", "mexia", 83);
	addEdge("daytonaBeach", "jacksonville", 92);
	addEdge("daytonaBeach", "orlando", 54);
	addEdge("denver", "wichita", 523);
	addEdge("denver", "grandJunction", 246);
	addEdge("desMoines", "omaha", 135);
	addEdge("desMoines", "minneapolis", 246);
	addEdge("elPaso", "sanAntonio", 580);
	addEdge("elPaso", "tucson", 320);
	addEdge("eugene", "salem", 63);
	addEdge("eugene", "medford", 165);
	addEdge("europe", "philadelphia", 3939);
	addEdge("ftWorth", "oklahomaCity", 209);
	addEdge("fresno", "modesto", 109);
	addEdge("grandJunction", "provo", 220);
	addEdge("greenBay", "minneapolis", 304);
	addEdge("greenBay", "milwaukee", 117);
	addEdge("greensboro", "raleigh", 74);
	addEdge("houston", "mexia", 165);
	addEdge("indianapolis", "stLouis", 246);
	addEdge("jacksonville", "savannah", 140);
	addEdge("jacksonville", "lakeCity", 113);
	addEdge("japan", "pointReyes", 5131);
	addEdge("japan", "sanLuisObispo", 5451);
	addEdge("kansasCity", "tulsa", 249);
	addEdge("kansasCity", "stLouis", 256);
	addEdge("kansasCity", "wichita", 190);
	addEdge("keyWest", "tampa", 446);
	addEdge("lakeCity", "tampa", 169);
	addEdge("lakeCity", "tallahassee", 104);
	addEdge("laredo", "sanAntonio", 154);
	addEdge("laredo", "mexico", 741);
	addEdge("lasVegas", "losAngeles", 275);
	addEdge("lasVegas", "saltLakeCity", 486);
	addEdge("lincoln", "wichita", 277);
	addEdge("lincoln", "omaha", 58);
	addEdge("littleRock", "memphis", 137);
	addEdge("littleRock", "tulsa", 276);
	addEdge("losAngeles", "sanDiego", 124);
	addEdge("losAngeles", "sanLuisObispo", 182);
	addEdge("medford", "redding", 150);
	addEdge("memphis", "nashville", 210);
	addEdge("miami", "westPalmBeach", 67);
	addEdge("midland", "toledo", 82);
	addEdge("minneapolis", "winnipeg", 463);
	addEdge("modesto", "stockton", 29);
	addEdge("montreal", "ottawa", 132);
	addEdge("newHaven", "providence", 110);
	addEdge("newHaven", "stamford", 92);
	addEdge("newOrleans", "pensacola", 268);
	addEdge("newYork", "philadelphia", 101);
	addEdge("norfolk", "richmond", 92);
	addEdge("norfolk", "raleigh", 174);
	addEdge("oakland", "sanFrancisco", 8);
	addEdge("oakland", "sanJose", 42);
	addEdge("oklahomaCity", "tulsa", 105);
	addEdge("orlando", "westPalmBeach", 168);
	addEdge("orlando", "tampa", 84);
	addEdge("ottawa", "toronto", 269);
	addEdge("pensacola", "tallahassee", 120);
	addEdge("philadelphia", "pittsburgh", 319);
	addEdge("philadelphia", "newYork", 101);
	addEdge("philadelphia", "uk1", 3548);
	addEdge("philadelphia", "uk2", 3548);
	addEdge("phoenix", "tucson", 117);
	addEdge("phoenix", "yuma", 178);
	addEdge("pointReyes", "redding", 215);
	addEdge("pointReyes", "sacramento", 115);
	addEdge("portland", "seattle", 174);
	addEdge("portland", "salem", 47);
	addEdge("reno", "saltLakeCity", 520);
	addEdge("reno", "sacramento", 133);
	addEdge("richmond", "washington", 105);
	addEdge("sacramento", "sanFrancisco", 95);
	addEdge("sacramento", "stockton", 51);
	addEdge("salinas", "sanJose", 31);
	addEdge("salinas", "sanLuisObispo", 137);
	addEdge("sanDiego", "yuma", 172);
	addEdge("saultSteMarie", "thunderBay", 442);
	addEdge("saultSteMarie", "toronto", 436);
	addEdge("seattle", "vancouver", 115);
	addEdge("thunderBay", "winnipeg", 440);
}



