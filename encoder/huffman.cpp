#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <map>
std::ifstream fin("freq.txt");
const std::string DUMMY_NODE = ".";

struct cmpByStringLength {
    bool operator()(const std::string& a, const std::string& b) const {
        return a.length() <= b.length();
    }
};

std::map<std::string, std::string, cmpByStringLength> encoding;

struct Node {
    std::string name;
    double prob;
    Node *leftChild, *rightChild;
    Node(double p, const std::string &str = DUMMY_NODE): name(str), prob(p), leftChild(nullptr), rightChild(nullptr) {}
    bool operator<(Node *n) {
        return this->prob < n->prob;
    }
};

void goThroughTree(Node *currNode, const std::string &currEncoding) {
    if (currNode->name != DUMMY_NODE) {
        encoding[currEncoding] = currNode->name;
        delete currNode;
        return;
    }
    goThroughTree(currNode->leftChild, currEncoding + "0");
    goThroughTree(currNode->rightChild, currEncoding + "1");
    delete currNode;
}

int main() {
    double prob;
    std::string operation;
    std::priority_queue<Node *> minProb;
    while (fin >> operation) {
        fin >> prob;
        Node *newNode = new Node(prob, operation);
        minProb.push(newNode);
    }
    while ((int) minProb.size() > 1) {
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
    for (auto elem : encoding) {
        std::cout << elem.second << '\t' << elem.first << '\n';
    }
    return 0;
}
