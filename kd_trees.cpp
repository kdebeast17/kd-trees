#include <iostream>     
#include <vector>
#include <tuple>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <queue>

using namespace std;
using namespace std::chrono;
int type_of_dist = 1;
int Ldist_count = 0;
int Udist_count = 0;
int Ldist_time = 0;
int Udist_time = 0;
int brute_time = 0;

bool yes = 0;
#define DIM 5

void random_vector_generator(int number, float array[][DIM]){
    for (int i = 0; i < number; i++) 
        {
            for(int j = 0 ; j < DIM ; j++){
                float r1 = rand() % 100;
                int	r2 = rand() % 2;
                array[i][j] = (r1/10.0)* pow(-1, r2);
            }    
        }
}

void swap_(int a, int b , float list[][DIM]){
    for(int i= 0 ; i < DIM ; i++){ 
        float t;
        t = list[b][i] ,
        list[b][i] = list[a][i];
        list[a][i] = t;
    }
}

void print(float list[][DIM],int k){
    for(int i = 0 ;i < k ; i++ ){
        for ( int j= 0; j < DIM ; j++){
            cout << list[i][j] << " ";
        }
        cout << endl;
    }
}

int partition(float list[][DIM], int left, int right, int dimension){
    int pivotValue = list[right][dimension];
    
    int storeIndex = left;
    for (int i = left; i < right; i++){
        if (list[i][dimension] < pivotValue){
            swap_(storeIndex, i, list);
            storeIndex++;
        } 
    }
    swap_(right, storeIndex, list); // Move pivot to its final place
    return storeIndex;
}

float* median(float list[][DIM], int left, int right, int dim,int k){

    int pivotIndex = partition(list, left, right, dim);
    if (k -1 == pivotIndex - left){
        return list[pivotIndex]; 
    }
    else if (pivotIndex - left > k -1){
        return median(list, left, (pivotIndex - 1), dim,k);
    }
    else{
        return median(list, (pivotIndex + 1), right, dim,k-pivotIndex+left-1);
    }
}

class Node{
public:
    float key[DIM];
    Node* left;
    Node* right;
    Node* parent;
    float cell_box[DIM][2];
    int dim;

    Node(float* keyarr){
        for(int i=0; i<DIM; i++){
            this->key[i] = keyarr[i];
        }
    }
};

float EucDist(float*a , float*b ){
    float sum = 0;
    for (int i = 0; i < DIM; i++){
        sum += pow((a[i] - (b[i])), 2);
    }

    return sqrt(sum);
}

float Manhattan(float *a, float *b){
    float sum = 0;
    for (int i = 0; i < DIM; i++){
        sum += abs(a[i] - (b[i]));
    }
    return sum;
}

float Max_Norm(float *a, float *b){
 
    float sum[DIM];
    for (int i = 0; i < DIM; i++){
        sum[i]= abs((a[i])-(b[i]));
    }
    float max = sum[0];
    for (int i = 1; i < DIM; i++){
        if (sum[i] > max){
            max = sum[i];
        } 
    }

 return max;
}

float distance(float *a, float*b){
    if (type_of_dist == 1){
        return EucDist(a, b);
    }
    else if (type_of_dist == 2){
        return Manhattan(a, b);
    }
    else if (type_of_dist == 3){
        return Max_Norm(a, b);
    }
}

float Udist(Node* R, float* qp){
    float cent[DIM];
    float v[DIM];
    for(int i = 0; i < DIM; i++){
        cent[i] = (R->cell_box[i][0] + R->cell_box[i][1])/2;
        float face_dist = (abs(R->cell_box[i][0] - R->cell_box[i][1])/2);
        v[i] = cent[i] + face_dist *( ((cent[i] - qp[i]) >= 0) ? 1 : -1);
    }
    return distance(qp, v);
}

float Ldist(Node* R, float* qp){
    float cent[DIM];
    float v[DIM];
    for(int i = 0; i < DIM; i++){
        cent[i] = (R->cell_box[i][0] + R->cell_box[i][1])/2;
        float face_dist = abs((R->cell_box[i][0] - R->cell_box[i][1])/2);
        v[i] = (abs((cent[i] - qp[i])) - face_dist>=0) ? abs((cent[i] - qp[i])) - face_dist : 0;
    }
    float zero[DIM];
    for (int i = 0; i<DIM; i++){
        zero[i] = 0;
    }
    return distance(v, zero);
}

class kdtree{

private:
     void add_subtree(vector<Node*> &neighbours, Node* curr){
        if(curr == NULL){
            return;
        }
        add_subtree(neighbours, curr->left);
        neighbours.push_back(curr);
        add_subtree(neighbours, curr->right);
    }

    bool in_range(Node* c, float* qp, float epsilon){
        bool flag = 1;
        for(int i = 0; i<DIM; i++){
            if(c->key[i] < (qp[i]-epsilon) || c->key[i] > (qp[i] + epsilon )){
                flag = 0;
                break;
            }
        }
        return flag;
    }

    bool in_box(Node*c, float*qp){
        bool flag = 1;
        for(int i = 0; i<DIM; i++){
            if(c->cell_box[i][0] > qp[i] || c->cell_box[i][1] < qp[i] ){
                flag = 0;
                break;
            }
        }
        return flag;
    }

    void radius_query(vector<Node*> &neighbours, float* query_point, float radius, Node* current, int &count){
        if(current == NULL){
            return;
        }
        visited++;
        if (current->right != NULL && current->left != NULL){
            // Udist_count++;
            // auto start1 = high_resolution_clock::now();
            // float farther_dist = Udist(current, query_point);
            // auto end1 = high_resolution_clock::now();
            // auto duration1 = duration_cast<nanoseconds>(end1 - start1);
            // Udist_time += duration1.count();
            if (Udist(current, query_point) <= radius){
                count++;
                if (current == root){
                    cout << "All points lie within the hypersphere." << endl;
                    return;
                }
                else{
                    add_subtree(neighbours, current);
                    return;
                }
            }
            // Ldist_count++;
            // auto start2 = high_resolution_clock::now();
            // float closer_dist = Ldist(current, query_point);
            // auto end2 = high_resolution_clock::now();
            // auto duration2 = duration_cast<nanoseconds>(end2 - start2);
            // Ldist_time += duration2.count();
            if(Ldist(current, query_point) >= radius){
                count++;
                if(current == root) cout << "No points in hypersphere. \n";
                return;
            }   
        }

        if (distance(current->key, query_point) <= radius){ 
            neighbours.push_back(current);
        }
                
        // Searching left child and right child recursively
        radius_query(neighbours, query_point, radius, current->left, count);
        radius_query(neighbours, query_point, radius, current->right, count);
        
    } 

    void brute_force(vector<Node*> & bf_neighbours, float* query_point, float radius, Node* curr){
        if(curr == NULL){
            return;
        }
        brute_visited++;
        float dist = distance(curr->key, query_point);
        if(dist < radius){
            bf_neighbours.push_back(curr);
        }    
        brute_force(bf_neighbours, query_point, radius, curr->left);
        brute_force(bf_neighbours, query_point, radius, curr->right);
    }

    Node* guess_node(Node* root, float* qp, int dim){
        if(root->left == NULL && root->right == NULL){
            for(int i = 0; i < DIM ; i++){
                cout << root->key[i] << " ";
            }
            cout << endl;
            return root;
        }

        if(root->key[dim] > qp[dim] && root->left != NULL){
            guess_node(root->left,qp,dim+1);
        }
        if(root->key[dim] < qp[dim] && root->right != NULL){
            guess_node(root->right,qp,dim+1);
        }

        return root;
    }

public:

    Node* root = NULL;
    int count = 0;
    int brute_visited = 0;
    int visited = 0;
    Node* construct(float array[][DIM], int left, int right, int root_dim){

        if(left > right){
            return NULL;
        }
        int k = (right - left)/2 + 1; 

        float* median_point = median(array, left, right, root_dim,k); 
        if (!root){
            root = new Node(median_point); 
        }
        Node* curr = new Node(median_point); 
        curr->dim = root_dim;

        if(right>left){ 
            if (right - left == 1){
            curr->left = NULL;
        }
        else{
            curr->left = construct(array, left, (left+right)/2 -1, (root_dim+1)%DIM);
            if(curr->left != NULL){
            curr->left->parent = curr;
            }
        }

        curr->right = construct(array, (left+right)/2 + 1, right, (root_dim+1)%DIM);
        if(curr->right != NULL){
            curr->right->parent = curr;
        }
        }
        if(left == right){
            for(int i = 0; i<DIM; i++){
                curr->cell_box[i][0] = curr->key[i];
                curr->cell_box[i][1] = curr->key[i];
            }
        }

        else{
            for(int i = 0; i<DIM; i++){
                if(curr->left == NULL && curr->right != NULL){
                    curr->cell_box[i][0] = min(curr->right->cell_box[i][0], curr->key[i]);
                    curr->cell_box[i][1] = max(curr->right->cell_box[i][1], curr->key[i]);
                }
                else if(curr->right == NULL && curr->left != NULL){
                    curr->cell_box[i][0] = min(curr->left->cell_box[i][0], curr->key[i]);
                    curr->cell_box[i][1] = max(curr->left->cell_box[i][1], curr->key[i]);
                }
                else if(curr->right != NULL && curr->left != NULL){
                    curr->cell_box[i][0] = min(min(curr->left->cell_box[i][0], curr->right->cell_box[i][0]), curr->key[i]);
                    curr->cell_box[i][1] = max(max(curr->left->cell_box[i][1], curr->right->cell_box[i][1]), curr->key[i]);
                }
            }
        }
        return curr; 
    }

    void radius_range_query(vector<Node*> &neighbours, float* query_point, float radius){
        auto start1 = high_resolution_clock::now(); 
        radius_query(neighbours, query_point, radius, root, count);
        auto stop1 = high_resolution_clock::now();
        auto duration1 = duration_cast<nanoseconds>(stop1 - start1);
        cout << endl << "number of prunes: " << count ;
        cout << endl << "number of visted nodes: " << visited ;
        cout << endl << "Time in nanoseconds for modified range search in func: " << duration1.count() << endl;
    }

    void brute_force_query(vector<Node*> & bf_neighbours, float* query_point, float radius){
        auto start1 = high_resolution_clock::now(); 
        brute_force(bf_neighbours, query_point, radius, root);
        auto stop1 = high_resolution_clock::now();
        auto duration1 = duration_cast<nanoseconds>(stop1 - start1);
        cout << endl << "number of brute visted nodes: " << brute_visited ; 
        cout << endl << "Time in nanoseconds for brute force search in func: " << duration1.count() << endl;
    }

    void print_tree(Node *root){
        if (root == NULL) return;
        queue<Node*> q;
        Node *curr;
        q.push(root);
        q.push(NULL);
    
        while (q.size() > 1)
        {
            curr = q.front();
            q.pop();
            if (curr == NULL)
            {
            q.push(NULL);
            cout << "\n";
            }
            
            else {
                if(curr->left)
                q.push(curr->left);
                if(curr->right)
                q.push(curr->right);
                cout << curr->key[0] << " "  << curr->key[1] << "\n";
            }
        }
    }

};


int main(){
    //srand(time(0));
    int number;
    // cin >> number;
    number = 300;
    float points[number][DIM];
    random_vector_generator(number, points);
    vector<Node*> neighbours;
    vector<Node*> bf_neighbours;
    float radius;
    //cin >> radius;
    radius = 10;
    for(type_of_dist = 1; type_of_dist <= 1; type_of_dist++){
        float query_point[1][DIM];
        random_vector_generator(1, query_point);
        float qp[DIM];
        for(int i = 0; i<DIM; i++){
            qp[i] = query_point[0][i];
        }
        kdtree kd;
        kd.root = kd.construct(points, 0, number - 1, 0);
    
        //kd.print_tree(kd.root);
        cout << "\n\n\n";
        kd.radius_range_query(neighbours,qp,radius);
        cout << neighbours.size() << "\n";
        for(int i = 0 ;i < neighbours.size() ; i++){
            for(int j = 0 ; j < DIM; j++){
                cout << neighbours[i]->key[j] << " ";
            }
            cout << "\n";
        }
        cout << "\n\n\n";
        cout << "brute force method"; 
        kd.brute_force_query(bf_neighbours,qp,radius);    
        cout << bf_neighbours.size() << "\n";
        for(int i = 0 ;i < bf_neighbours.size() ; i++){
            for(int j = 0 ; j < DIM; j++){
                cout << bf_neighbours[i]->key[j] << " ";
            }
            cout << "\n";
        }
    }
        
    return 0;
}

