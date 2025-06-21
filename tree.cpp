#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

//make sure to edit both the parent and child vector when adding/removing a child node. make this change.

//map everybody to their nodes, so split trees can be retrieved
unordered_map<string, Node*> everyone = {};

//helper function
void removeFromVector(vector<Node*>& vec, Node* target) {
    auto it = find(vec.begin(), vec.end(), target);
    if (it != vec.end()) {
        vec.erase(it);
    }
}

//class for a member
class Node {
public:
    string val;
    vector<Node*> parents;
    Node* partner;
    vector<Node*> children;

    Node() : val(""), parents({}), partner(nullptr), children({}) {}

    Node(string _val) : val(_val), parents({}), partner(nullptr), children({}) {}

    Node(string _val, vector<Node*> _parents) : val(_val), parents(_parents), partner(nullptr), children({}) {}

    Node(string _val, vector<Node*> _parents, Node* _partner) : val(_val), parents(_parents), partner(_partner), children({}) {}

    Node(string _val, vector<Node*> _parents, Node* _partner, vector<Node*> _children) : val(_val), parents(_parents), partner(_partner), children(_children) {}

    //displays stats for a node
    void displayNode() {
        if (val.size() == 0) {
            cout << "Name: N/A\n";
        } else {
            cout << "Name: " << val << "\n";
        }

        if (parents.size() == 0) {
            cout << "Parents: N/A\n";
        } else {
            cout << "Parents: ";
            for (int i = 0; i < parents.size(); i++) {
                cout << parents[i]->val << " ";
            }
            cout << "\n";
        }

        if (!partner) {
            cout << "Partner: N/A\n";
        } else {
            cout << "Partner: " << partner->val << "\n";
        }

        if (children.size() == 0) {
            cout << "Children: N/A\n";
        } else {
            cout << "Children: ";
            for (int i = 0; i < children.size(); i++) {
                cout << children[i]->val << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    //when this is called, the children remain shared between the partners
    void removePartner() {
        if (!partner) return;
        partner->partner = nullptr;
        this->partner = nullptr;
    }

    //seperates into two trees (child tree and current tree). also removes child from partner
    void removeChild(Node* child) {
        removeFromVector(children, child);
        removeFromVector(partner->children, child);
    }

    //cuts connection between thisNode and parent
    void removeParent(Node* oldParent) {
        auto it = find(oldParent->children.begin(), oldParent->children.end(), this);
        if (it != oldParent->children.end()) {
            oldParent->children.erase(it);
        }

        it = find(parents.begin(), parents.end(), oldParent);
        if (it != parents.end()) {
            parents.erase(it);
        }
    }

    //adds partner if doesn't already have
    void addPartner(Node* partnerNode) {
        if (partner) {
            return;
        }
        partner = partnerNode;
        unordered_set<Node*> s;
        for (int i = 0; i < children.size(); i++) {
            s.insert(children[i]);
        }
        for (int i = 0; i < partner->children.size(); i++) {
            s.insert(partner->children[i]);
        }
        children = vector<Node*>(s.begin(), s.end());
        partner->children = children;
    }

    //adds children to both it and its partner (if applicable)
    void addChildren(vector<Node*> newChildren) {
        for (int i = 0; i < newChildren.size(); i++) {
            if (partner) {
                partner->children.push_back(newChildren[i]);
                newChildren[i]->parents.push_back(partner);
            }
            children.push_back(newChildren[i]);
            newChildren[i]->parents.push_back(this);
        }
    }

    //adds a parent
    void addParent(Node* newParent) {
        if (parents.size() == 2) {
            return;
        }
        parents.push_back(newParent);
        newParent->children.push_back(this);
    }
};

//searches for a name in everyone and returns its node
Node* search() {
    string name;
    cout << "Enter the name you would like to search for: ";
    cin >> name;
    if (everyone.find(name) == everyone.end()) {
        return nullptr;
    }
    return everyone[name];
}

//lists everyone
void listEveryone() {
    for (auto &i : everyone) {
        cout << i.first << "\n";
    }
    cout << "\n";
}

//main function
void run() {
    string startName;
    cout << "Enter starting node name: ";
    cin >> startName;

    Node* startNode = new Node(startName);
    everyone[startName] = startNode;
    Node* curr = startNode;

    //to be completed
}
