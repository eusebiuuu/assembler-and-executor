#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <algorithm>

struct Instruction {
    std::string name;
    std::string encoding;
};

std::vector<Instruction> encodings;

struct Node {
    std::string name;
    double prob;

    Node* left;
    Node* right;
    
    Node(double p, const std::string& str = "")
        : name(str), prob(p), left(nullptr), right(nullptr)
    {}
};

void goThroughTree(Node* root, std::string currEncoding) {
    if (root->name != "") {
        encodings.push_back({root->name, currEncoding});
        return;
    }

    goThroughTree(root->left, currEncoding + "0");
    goThroughTree(root->right, currEncoding + "1");
}

int main() {
    auto node_ptr_comparator = [](Node* a, Node* b) { return a->prob > b->prob; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(node_ptr_comparator)> minProb(node_ptr_comparator);

    {
        std::ifstream fin("./freq.txt");
        
        std::string operation;
        double prob;
        while (fin >> operation >> prob) {
            Node *newNode = new Node(prob, operation);
            
            minProb.push(newNode);
        }
        
        fin.close();
    }

    while (minProb.size() > 1) {
        Node* leftNode = minProb.top(); minProb.pop();
        Node* rightNode = minProb.top(); minProb.pop();

        Node* newNode = new Node(leftNode->prob + rightNode->prob);
        newNode->left = leftNode;
        newNode->right = rightNode;

        minProb.push(newNode);
    }

    goThroughTree(minProb.top(), "");
    
    std::sort(encodings.begin(), encodings.end(), [](auto& l, auto& r){ return l.encoding.length() < r.encoding.length(); });
    
    {
        std::ofstream fout("./encodings.txt");

        for (const Instruction& instruction : encodings) {
            fout << instruction.name << ' ' << instruction.encoding << '\n';
        }
        
        fout.close();
    }
}
