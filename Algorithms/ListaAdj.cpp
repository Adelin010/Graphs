#include <fstream>
#include <vector>
#include <iostream>
#include <assert.h>
#include <stack>
#include <queue>

using std::queue;
using std::cout;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::stack;

struct Edge{
    int to;
    long weight;
};

class Graph{
    public: 
        int V,  E;
        char type;
        vector<int>* adj;
        vector<Edge>* edges;

        Graph(){
            this->V = 0;
            this->E = 0;
        }
        Graph( char* filename, char option){
            ifstream fin(filename);
            fin >> this->V;
            // int n1, n2;
            // for(int i = 1; i <= this->E; ++i){
            //     fin >> n1 >> n2;
            //     this->adj[n1].push_back(n2);
            //     this->adj[n2].push_back(n1);
            // }
            if(option == 'd'){
                this->type = 'd';
                this->adj = new vector<int>[this->V + 1];
                int x, y;
                while(fin >> x >> y){
                    this->adj[x].push_back(y);
                } 
            }else if(option == 'u'){
                this->type = option;
                this->adj = new vector<int>[this->V + 1];
                int x, y;
                while(fin >> x >> y){
                    this->adj[x].push_back(y);
                    this->adj[y].push_back(x);
                } 
            }else if(option == 'w'){
                //init the weights of the non existing edges with -10^7
                this->type = option;
                this->edges = new vector<Edge>[this->V+1];
                int x, y, w;
                while(fin >> x >> y >> w){
                    Edge edge;
                    edge.to = y;
                    edge.weight = w;
                    this->edges[x].push_back(edge);
                    edge.to = x;
                    this->edges[y].push_back(edge);
                }

            }else if(option == 'r'){
                this->type = option;
                this->edges = new vector<Edge>[this->V+1];
                int x, y, w;
                while(fin >> x >> y >> w){
                    Edge edge;
                    edge.to = y;
                    edge.weight = w;
                    this->edges[x].push_back(edge);
                }
            }
        }

        /**
         * --------------------------
         * TOP SORT
         * --------------------------
        */
        void TopSortDfs(vector<int>& visited, stack<int>& res, int node){
            visited[node] = 1;

            for(auto it = this->adj[node].begin(); it != this->adj[node].end(); ++it){
                if(!visited[(*it)]){
                    TopSortDfs(visited, res, *it);
                }
            }
            res.push(node);
        }

        int findProperNodeForTopSort(vector<int>& visited){
            int* inDegree = new int[this->V+1];
            for(int i = 1; i <= V; ++i){
                inDegree[i] = 0;
            }

            for(int i = 1; i <= V; ++i){
               for(auto it = this->adj[i].begin(); it != this->adj[i].end(); it++){
                    inDegree[*it]++;
               }
            }

            //Select the best option
            int selection = -1;
            for(int i = 1; i <= V; ++i){
                if(inDegree[i] == 0 && !visited[i]){  
                    selection = i;
                    break;
                }
            }
            delete[] inDegree;
            return selection;
        }

        void TopSort(ofstream & fout){
            if(this->type != 'd')
                return;
            
            stack<int> results;
            vector<int> visited(V+1, 0);

            for(int i =1; i <= V; ++i){
                assert(visited[i] == 0);
            }

            int nod = this->findProperNodeForTopSort(visited);
            while(nod != -1){
                TopSortDfs(visited, results, nod);
                nod = this->findProperNodeForTopSort(visited);
            }

            fout << "\n\n\t*****TOP SORT******\n";
            while(!results.empty()){
                fout << results.top() << " ";
                results.pop();  
            }
        }
        /**
         * -----------------
         * END TOP SORT 
         * ----------------
        */


       /**
        * --------------------------
        * SHORTEST PATH PROBLEM 
        * DIJKSTRA ALGO
        * --------------------------
       */
        int finProperNode(vector<int>& visited, vector<long>& dist, long INF){
            int minNode = -1;
            long  minDist = INF;
            for(int i = 1; i <= this->V; ++i){
                if(dist[i] < minDist && !visited[i]){
                    minNode= i;
                    minDist = dist[i];
                }
            }
            return minNode;
        }

        void DijAfisDist(ofstream& fout, vector<long>& dist, long INF){
            for(int i = 1; i <= this->V; ++i){
                if(dist[i] == INF){
                    fout << "There is no way to this node: " << i << '\n';
                    continue;
                }
                fout << "Dist to node: " << i<< "is: ";
                fout << dist[i] << "\n";
            }
        }

        void DijPathDisplay(ofstream& fout, vector<int> parent, int start, int end){
            if(start != end){
                DijPathDisplay(fout, parent, start, parent[end]);
            }
            fout << end << " ";
        }


        void Dijkstra(ofstream& fout, int start){
            cout << this->type;
            // if(this->type != 'w' || this->type != 'r')
            //     return;

            long INF = 100000000;
            vector<int> visited(V+1, 0);
            vector<long> dist(this->V + 1, INF);
            vector<int> parent(this->V +1, -1);

            //the parent of each node is itself until further change
            for(int i = 1; i <= this->V; ++i){
                parent[i] = i;
            }

            dist[start] = 0;
            int bestNode = this->finProperNode(visited, dist, INF);
            while(bestNode != -1){
                //Mark as visited
                visited[bestNode] = 1;
                auto it = this->edges[bestNode].begin();
                for(; it != this->edges[bestNode].end(); ++it){
                    if(dist[bestNode] + (*it).weight < dist[(*it).to]){
                        dist[(*it).to] = dist[bestNode] + (*it).weight;
                        parent[(*it).to] = bestNode;
                    }
                }
                bestNode = this->finProperNode(visited, dist, INF);
            }

           this->DijAfisDist(fout, dist, INF);
           fout << "\n\n\t***Path from "<< start << "to " << 4 << "is*****\n";
           this->DijPathDisplay(fout, parent, start, 4);

        }
        /**
         * -----------------------------
         * END  OF DIJKSTRA'S ALGORITHM
         * -----------------------------
        */

        ~Graph(){
            delete this->adj;
        }

        void printAdj(ofstream& fout){
            for(int i = 1; i <= this->V; ++i){
                fout << "Nodul " << i << ": ";
                for(auto it = this->adj[i].begin(); it != this->adj[i].end(); ++it){
                    fout << (*it)<< " ";
                }
                fout << '\n';
            }
        }

        void printEdges(ofstream& fout){
            for(int i =1 ; i <= this->V; ++i){
                fout << " Node " << i<< "has: ";
                for(auto it = this->edges[i].begin(); it != this->edges[i].end(); it++){
                    fout << "To: " << (*it).to << " Weight: " << (*it).weight << "||| ";
                } 
                fout << '\n';
            }
        }


        /**
         * --------------------------
         * BELLMANFORD ALGORITHM
         * --------------------------
        */

       void BellmanFord(ofstream& fout){

       }

};

int main(){
    
    ofstream fout("bellman.out");
    char file[] = "bellman.in";

    Graph* g = new Graph(file, 'r');
    g->printEdges(fout);

    fout.close();
    return 0;
}