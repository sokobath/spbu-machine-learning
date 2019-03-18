#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

struct edge {
	int id;
	double w;
	double a = 0;
	double r = 0;

	edge (int id, double w) {
		this->id = id;
		this->w = w;
	}
};

int n = 196591;
vector<vector<edge>> edges(n);
double dump_factor = 0.5;
int n_epochs = 200;

vector<int> get_exemplars() {
	vector<int> exemplars(n);

    for (int i = 0; i < n; ++i) {
    	int id = -1;
        double arg_max = -1000000000;

        for (size_t j = 0; j < edges[i].size(); ++j) {
        	if (edges[i][j].r + edges[i][j].a > arg_max) {
        		arg_max = edges[i][j].r + edges[i][j].a;
                id = edges[i][j].id;
            }
        }
        exemplars[i] = id;
    }
    return exemplars;
}

void update_parameters() {
    vector<double> sum_max(n);

    for (int i = 0; i < n; ++i) {
    	double max_1 = -1000000000;
    	double max_2 = -1000000000;
    	int id_max = -1;
    	for (size_t j = 0; j < edges[i].size(); ++j) {
    		if (edges[i][j].a + edges[i][j].w > max_1) {
    			max_2 = max_1;
    		    max_1 = edges[i][j].a + edges[i][j].w;
    		    id_max = edges[i][j].id;
    		}
    	}

    	for (size_t j = 0; j < edges[i].size(); ++j) {
    		double cur_max = (id_max == edges[i][j].id ? max_2 : max_1);
    	    edges[i][j].r = dump_factor * edges[i][j].r + (1 - dump_factor) * (edges[i][j].w - cur_max);
    	    sum_max[j] += (0 > edges[i][j].r ? 0 : edges[i][j].r);
    	}
    }

    for (int i = 0; i < n; ++i) {
    	for (size_t j = 0; j < edges[i].size(); ++j) {
    		int k = edges[i][j].id;
            vector<edge> k_edges = edges[k];
            double rkk = k_edges[k_edges.size() - 1].r;
            double sum = sum_max[k] - (0 > edges[i][j].r ? 0 : edges[i][j].r) - (0 > rkk ? 0 : rkk);
            if (i != k) {
            	edges[i][j].a = dump_factor * edges[i][j].a + (1 - dump_factor) * (0 < rkk + sum ? 0 : rkk + sum);
            } else {
            	edges[i][j].a = dump_factor * edges[i][j].a + (1 - dump_factor) * sum;
            }
        }
    }
}

int count_unique(vector<int>& arr) {
	set<int> unique(arr.begin(), arr.end());
	return unique.size();
}

int main() {
	ifstream input_file("Gowalla_edges.txt");
	string line;
	while (getline(input_file, line)) {
		istringstream iss(line);
		int e1, e2;
		if (!(iss >> e1 >> e2)) { break; }
		edge ed = edge(e2, 1);
		edges[e1].push_back(ed);
	}
	input_file.close();

	for (int i = 0; i < n; ++i) {
		edge ed = edge(i, -1);
		edges[i].push_back(ed);
	}

	int count_eq = 0;
	vector<int> new_exemplars(n);
	vector<int> cur_exemplars(n);

	for (int i = 1; i <= n_epochs; ++i) {
		cout << "Epoch " << i << endl;
		update_parameters();
		cur_exemplars = new_exemplars;
		new_exemplars = get_exemplars();

		bool eq = true;
		for (int j = 0; j < n; ++j) {
			if (cur_exemplars[j] != new_exemplars[j]) {
				eq = false;
				break;
			}
		}
		if (eq) {
			count_eq++;
		} else {
			count_eq = 0;
		}
		if (count_eq > 9) {
			break;
		}
	}
	cout << "Number of clusters: ";
	cout << count_unique(new_exemplars) << endl;

	ofstream output_file("exemplars.txt");
	for (int exemplar : new_exemplars) {
		output_file << exemplar << "\n";
	}
	output_file.close();
	return 0;
}
