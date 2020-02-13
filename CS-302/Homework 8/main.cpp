//Homework 8, Extra Credit Project

#include <iostream>
#include <vector>
#include <climits>

#define V 6

typedef std::vector<int> iVec;
typedef std::vector<iVec> Graph;
typedef std::vector<bool> bVec;

// A utility function to find the vertex with 
// minimum key value, from the set of vertices 
// not yet included in MST 
int minKey(iVec key, bVec visited) 
{ 
// Initialize min value 
int min = INT_MAX, min_index; 

for (int i = 0; i < V; i++) 
	if (visited[i] == false && key[i] < min) 
		min = key[i], min_index = i; 

return min_index; 
} 

// A utility function to print the 
// constructed MST stored in parent[] 
void printMST(iVec parent, Graph graph) 
{ 
std::cout << "Edge\tWeight" <<std::endl; 
for (int i = 1; i < V; i++) 
	std::cout << parent[i] + 1 << "-" << i + 1 << "\t" << graph[i][parent[i]] << std::endl; 
} 


void primMST(Graph graph) 
{ 
	iVec MST(V); 
	iVec weights(V); 
	bVec visited(V); 

	for (int i = 0; i < V; i++) {
		weights[i] = INT_MAX;
		visited[i] = false; 
	}

	weights[0] = 0;	 
	MST[0] = -1; 


	for (int count = 0; count < V-1; count++) 
	{ 
 
		int u = minKey(weights, visited); 
		visited[u] = true; 

		for (int v = 0; v < V; v++) { 
			if (graph[u][v] && visited[v] == false && graph[u][v] < weights[v]) {
				MST[v] = u, weights[v] = graph[u][v]; 
			}
		}
	} 

	// print the constructed MST 
	printMST(MST, graph); 
} 


// driver program to test above function 
int main() 
{ 
Graph graph = 		{{0, 2, 0, 6, 0, 0}, 
					{2, 0, 3, 8, 5, 0}, 
					{0, 3, 0, 0, 7, 2}, 
					{6, 8, 0, 0, 9, 1}, 
					{0, 5, 7, 9, 0, 4},
					{0, 0, 1, 2, 0, 0}}; 

	// Print the solution 
	primMST(graph); 

	return 0; 
} 
