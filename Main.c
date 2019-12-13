
/**
	Lab 5 CSCE 3530
	Demonstration of Dijkstra’s Shortest Path Algorithm
	Supports up to six routers.
	
	Algorithm used: https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/
	From GeeksForGeeks.org (100% credit goes to them for the algorithm)

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <limits.h> 

// Represents the number of routers in the network/graph.
// NOTE: professor told us that it would be ok to pre-define
// the number of routers in the program.
#define ROUTERS 6 


// Represents the temporary shortest path generated while performing
// Dijkstra’s algorithm.
char tempPath[128];

// Funtion that implements Dijkstra's 
// single source shortest path 
// algorithm for a graph represented 
// using adjacency matrix representation 
// ***** 100% credit goes to GeeksForGeeks.org for this algorithm) *****
// The link is at the top of the file.
void dijkstra(FILE *fp, int graph[ROUTERS][ROUTERS], int source);

// Writes the path constructed by dijkstra's algorithm
// to the console and to the file LS.txt as per the lab
// requirements, and in the same format as mentioned on Canvas.
void writePath(FILE *fp, int source, int parent[]);

// Gets the shortest path from the parent array to
// the next node.
void evaluatePath(int parent[], int j);

// Finds the minimum distance value from the vertices
// not in the shortest path tree.
int minimumDist(int distance[], bool shortestPath[]);

// Returns true if the specified file exists.
bool fileExists(const char* file);


// Main function which fully executes the program.
int main() {
	
	// Creates the output file if it doesn't exist.
	if(!fileExists("./LS.txt")) {
		FILE* fp = fopen("LS.txt", "ab+");
		fclose(fp);
	}

	// Opens up the output file for writing.
	FILE* fp;
	fp = fopen("./LS.txt", "w");
	
	// Opens the router file for reading and evaluating the
	// cost matrix between the routers.
	FILE* routerfp;
    routerfp = fopen("router.txt", "r");
    if (routerfp == NULL) {
        exit(EXIT_FAILURE);
	}

	// Initializes the file variables.
	char* line = NULL;
    ssize_t file;
	size_t lengthOf = 0;
	
	// Initializes the two-dimensional array as a cost matrix
	// with all costs set to 0.
	int graph[ROUTERS][ROUTERS] = { 0 };

	// Loops through the router file and reads each line individually.
    while ((file = getline(&line, &lengthOf, routerfp)) != -1) {
		// Removes the new line separator at the end of each line so
		// that each line can be properly evaluated.
		line[strcspn(line, "\r\n")] = 0;
		
		// Splits the line into tokens using a space as a delimiter.
		// This is used for the "s d c" format as mentioned in the lab
		// requirements: s being source, d being destination and c being cost.
		char* token = strtok(line, " ");
		int src = 0;
		int dest = 0;
		int cost = 0;
		
		// Represents the counter which is incremented after every token
		// is split by using a space as the delimiter.
		int count = 0;
		
		while(token != NULL) {
			switch(count) {
				
				// Src node
				case 0:
					src = (int) (token[0] - 48);
				break;
				
				// Dest node
				case 1:
					dest = (int) (token[0] - 48);
				break;
				
				// Cost
				case 2:
					cost = atoi(token);
				break;
				
				default:
				break;
			}
			count++;
			token = strtok(NULL, " ");
		}
		
		// Sets the cost in the cost matrix of the source and destination
		// nodes by subtracting 69, which is the character equivalent of
		// 'u'. This means that in the graph, 0 corresponds to 69 ('u'), 1 corresponds 
		// to 70 ('v'), etc.
		graph[src - 69][dest - 69] = cost;
		graph[dest - 69][src - 69] = cost;
    }

	// Closes the router file and frees up the memory.
    fclose(routerfp);
    if (line) {
        free(line);
	}

	// Performs Dijkstra’s algorithm for all of the routers from u through z.
	// This is done via a for loop that goes through every router.
	// Dijkstra’s starts at the node specified in the second parameter.
	printf("\n\n");
    dijkstra(fp, graph, 0);
	printf("\n");
	fprintf(fp, "\n");
	
	for(int i = 1; i < 6; i++) {
		dijkstra(fp, graph, i);
		printf("\n");
		fprintf(fp, "\n");
	}
	
	// Closes our file writer.
	fclose(fp);
	return 0;
}


// Funtion that implements Dijkstra's 
// single source shortest path 
// algorithm for a graph represented 
// using adjacency matrix representation 
// ***** 100% credit goes to GeeksForGeeks.org for this algorithm) *****
// The link is at the top of the file.
void dijkstra(FILE *fp, int graph[ROUTERS][ROUTERS], int source) { 

	bzero(tempPath, 128);
      
    // The output array. distance[i] 
    // will hold the shortest 
    // distance from source to i 
    int distance[ROUTERS];  
  
    // shortestPath[i] will true if vertex 
    // i is included / in shortest 
    // path tree or shortest distance  
    // from source to i is finalized 
    bool shortestPath[ROUTERS]; 
  
    // Parent array to store 
    // shortest path tree 
    int parent[ROUTERS]; 
	parent[source]  = -1;
  
    // Initialize all distances as  
    // INFINITE and shortestPath[] as false 
    for (int i = 0; i < ROUTERS; i++) { 
        distance[i] = INT_MAX; 
        shortestPath[i] = false; 
    } 
  
    // Distance of source vertex  
    // from itself is always 0 
    distance[source] = 0; 
  
    // Find shortest path for all vertices 
    for (int count = 0; count < ROUTERS - 1; count++) { 
	
        // Pick the minimum distance 
        // vertex from the set of 
        // vertices not yet processed.  
        // u is always equal to src 
        // in first iteration. 
        int u = minimumDist(distance, shortestPath); 
  
        // Mark the picked vertex  
        // as processed 
        shortestPath[u] = true; 
		
		// Stops if the next node is not adjacent to it.
		if (distance[u] == INT_MAX) {
			continue;
		}
  
        // Update distance value of the  
        // adjacent vertices of the 
        // picked vertex. 
        for (int v = 0; v < ROUTERS; v++) {
  
            // Update distance[v] only if is 
            // not in shortestPath, there is 
            // an edge from u to v, and  
            // total weight of path from 
            // src to v through u is smaller 
            // than current value of 
            // distance[v] 
			if (graph[u][v] && shortestPath[v] == false && distance[u] + graph[u][v] < distance[v]) {
                distance[v] = distance[u] + graph[u][v];
                parent[v] = u;
            }
		}
	} 
  
    // Writes out the constructed path and prints it.
    writePath(fp, source, parent); 
} 

// Writes the path constructed by dijkstra's algorithm
// to the console and to the file LS.txt as per the lab
// requirements, and in the same format as mentioned on Canvas.
void writePath(FILE *fp, int source, int parent[]) {
	
	// Converts the source index to its char representation
	char sourceToChar = (char) (source + 69 + 48);
	
	// Prints and writes the router that it is from u through z.
    printf("Router: %c", sourceToChar); 
	fprintf(fp, "Router: %c", sourceToChar);
	
	// Loops through the adjacent routers.
	for (int i = 0; i < ROUTERS; i++) { 
	
		// Skips if adjacent router is itself because that
		// cannot happen.
		if(i == source) {
			continue;
		}
		
		// Converts the adjacent router to its character representation
		// and prints it out. After this, it evalutes the path recursively.
		char iToChar = (char) (i + 69 + 48);
        printf("\n%c (%c, ", iToChar, sourceToChar); 
		fprintf(fp, "\n%c (%c, ", iToChar, sourceToChar); 
		evaluatePath(parent, i); 
		
		// Splits the shortest path into tokens and only prints out
		// the path from one router to its adjacent ones as per
		// the lab requirements; however, the entire path can also be printed.
		char* toksForPath = strtok(tempPath, " ");
		while(toksForPath != NULL) {
			printf("%s)", toksForPath);
			fprintf(fp, "%s)", toksForPath);
			break;
		}
		
		// Zeroes the temporary path
		bzero(tempPath, 128);
    }	
	
	// Separates the output of each router.
	printf("\n---------");
	fprintf(fp, "\n---------");
} 

// Gets the shortest path from the parent array to
// the next node.
void evaluatePath(int parent[], int nextNode) { 

	// Returns if the next node is the source node.
    if (parent[nextNode] == - 1)  {
		return; 
	}
	
	// Converts the index of next node to its
	// character representation which is easy to display
	// as a router from u through z.
	char jToChar = (char) (nextNode + 69 + 48);
	
	// Recursively prints the rest of the path.
	evaluatePath(parent, parent[nextNode]); 
	
	// Appends the path to the temporary path string.
	strcat(tempPath, (char[2]) { (char) jToChar, '\0' });
	strcat(tempPath, " ");
}

// Finds the minimum distance value from the vertices
// not in the shortest path tree.
int minimumDist(int distance[], bool shortestPath[]) { 
      
    // Set minimum value to INFINITY so that any
	// value that is less will be evaluated as less
	// than the previous value that was read.
    int minimum = INT_MAX;
	
	// The index of the minimum value is returned.
	int indexOfMinimum; 
  
    for (int v = 0; v < ROUTERS; v++) {
        if (!shortestPath[v] && distance[v] <= minimum) {
			minimum = distance[v];
			indexOfMinimum = v; 
		}
	}
  
    return indexOfMinimum; 
}

// Returns true if the specified file exists.
bool fileExists(const char* file) {
    struct stat buffer;
    int exists = stat(file, &buffer);
    if(exists == 0) {
        return true;
    }
	else {
        return false;
	}
}


