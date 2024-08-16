#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <algorithm>
std::ifstream fin("freq.txt");
const std::string DUMMY_NODE = ".";

std::vector<std::pair<std::string, std::string>> encoding;

struct Node {
    std::string name;
    double prob;
    Node *leftChild, *rightChild;
    Node(double p, const std::string &str = DUMMY_NODE): name(str), prob(p), leftChild(nullptr), rightChild(nullptr) {}
};

struct NodepCmp {
    bool operator()(Node* a, Node* b) const {
        return a->prob > b->prob;
    }
};

void goThroughTree(Node *currNode, std::string currEncoding) {
    if (currNode->name != DUMMY_NODE) {
        encoding.push_back({currNode->name, currEncoding});
        return;
    }
    goThroughTree(currNode->leftChild, currEncoding + "0");
    goThroughTree(currNode->rightChild, currEncoding + "1");
}

int main() {
    double prob;
    std::string operation;
    std::priority_queue<Node*, std::vector<Node*>, NodepCmp> minProb;
    while (fin >> operation) {
        fin >> prob;
        Node *newNode = new Node(prob, operation);
        minProb.push(newNode);
    }
    while (minProb.size() > 1) {
        auto leftNode = minProb.top();
        minProb.pop();
        auto rightNode = minProb.top();
        minProb.pop();
        auto newNode = new Node(leftNode->prob + rightNode->prob);
        newNode->leftChild = leftNode;
        newNode->rightChild = rightNode;
        minProb.push(newNode);
    }
    goThroughTree(minProb.top(), "");
    std::sort(encoding.begin(), encoding.end(), [](auto& l, auto& r){ return l.second.length() < r.second.length(); });
    for (auto elem : encoding) {
        std::cout << elem.first << '\t' << elem.second << '\n';
    }
    return 0;
}
