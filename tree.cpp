//to-do: add/remove sibling method, use bfs to print generations
//urgent fix: for add commands, make sure newNode links are severed. eg if newNode is already a child of this then it cannot be a partner of this until connection removed.
//make addParent() and removeParent() make partnering parents optional

#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

//forward declaration
class Node;

//map everybody to their nodes, so split trees can be retrieved
unordered_map<string, Node*> everyone = {};

//tree name
string treeName = "";

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
        cout << '\n';
        if (val.size() == 0) {
            cout << "Name: N/A\n";
        } else {
            cout << "Name: " << val << "\n";
        }

        if (parents.size() == 0) {
            cout << "Parents: N/A\n";
            cout << "Siblings: N/A\n";
        } else {
            cout << "Parents: ";
            for (int i = 0; i < parents.size(); i++) {
                cout << parents[i]->val << " ";
            }
            cout << "\n";
            cout << "Siblings: ";
            //gets siblings from accessing its parents' other children
            for (int i = 0; i < parents[0]->children.size(); i++) {
                if (parents[0]->children[i] != this) {
                    cout << parents[0]->children[i]->val << " ";
                }
            }
            if (parents[0]->children.size() == 1) {
                cout << "N/A";
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
        if (partner) {
            removeFromVector(partner->children, child);
        }
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

        if (parents.size() == 1) {
            parents[0]->partner = nullptr;
            oldParent->partner = nullptr;
        }
    }

    //adds partner if doesn't already have
    void addPartner(Node* partnerNode) {
        if (partner) {
            cout << "Already has a partner\n";
            return;
        }
        if (partnerNode->partner) {
            cout << "Already has a partner\n";
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
        partner->partner = this;
    }

    //adds child to both it and its partner (if applicable)
    void addChildren(Node* newChild) {
        if (find(children.begin(), children.end(), newChild) != children.end()) {
            cout << "Already a child\n";
            return;
        }
        if (newChild == this || newChild == partner) {
            cout << "Cannot add self or partner as child\n";
            return;
        }
        if (newChild->parents.size() == 2) {
            cout << "Already has max parent capacity (2)\n";
            return;
        }
        if (newChild->parents.size() == 1) {
            if (partner) {
                cout << "Cannot overload parents.\n";
                return;
            } else {
                partner = newChild->parents[0];
                newChild->parents[0]->partner = this;
            }
        }
        children.push_back(newChild);
        newChild->parents.push_back(this);
        if (partner) {
            partner->children.push_back(newChild);
            if (find(newChild->parents.begin(), newChild->parents.end(), partner) == newChild->parents.end()) {
                newChild->parents.push_back(partner);
            }
        }
    }

    //adds a parent
    void addParent(Node* newParent) {
        if (newParent == this) {
            cout << "Cannot assign self to parent\n";
            return;
        }
        if (find(parents.begin(), parents.end(), newParent) != parents.end()) {
            cout << "Already a parent of node\n";
            return;
        }
        if (parents.size() == 2) {
            cout << "Max parent capacity (2) already\n";
            return;
        }
        if (parents.size() == 1) {
            if (newParent->partner) {
                cout << "Cannot overload parents\n";
                return;
            }
            parents[0]->partner = newParent;
            newParent->partner = parents[0];
        }
        parents.push_back(newParent);
        newParent->children.push_back(this);
    }

    void deleteNode() {
        if (children.size() != 0) {
            cout << "Remove children first\n";
            return;
        }
        if (partner) {
            partner->partner = nullptr;
        }
        for (int i = 0; i < parents.size(); i++) {
            removeFromVector(parents[i]->children, this);
        }
        everyone.erase(val);
        delete this;
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
    cout << "\n";
    for (auto &i : everyone) {
        cout << i.first << "\n";
    }
    cout << "\n";
}

/*
Use one and two letter commands, seperated by whitespace
Steps for main function
1. Be able to either traverse upstream/downstream to parent/child node with input from name.
2. Be able to use all the random methods from the class, plus extra ones (search() and listEveryone())

display
remove <partner, parent, child>
add <partner, parent, child>
delete
search (returns whether exists or not)
switch <val>
listall
save
load
stop
*/

void cmdRemove(Node* curr) {
    string response;
    cout << "Partner, child, or parent (all lowercase)?\n";
    cin >> response;
    if (response == "partner") {
        curr->removePartner();
    } else if (response == "child") {
        string childName;
        cout << "Enter child name: ";
        cin >> childName;
        if (everyone.find(childName) == everyone.end()) {
            cout << "Doesn't exist\n";
        } else if (find(curr->children.begin(), curr->children.end(), everyone[childName]) == curr->children.end()) {
            cout << "Not a child of current\n";
        } else {
            curr->removeChild(everyone[childName]);
        }
    } else if (response == "parent") {
        string parentName;
        cout << "Enter parent name: ";
        cin >> parentName;
        if (everyone.find(parentName) == everyone.end()) {
            cout << "Doesn't exist\n";
        } else if (find(curr->parents.begin(), curr->parents.end(), everyone[parentName]) == curr->parents.end()) {
            cout << "Not a parent of current\n";
        } else {
            curr->removeParent(everyone[parentName]);
        }
    } else {
        cout << "Response not recognised\n";
    }
}

//fix: so that existing nodes can be added
void cmdAdd(Node* curr) {
    string response;
    cout << "Partner, child, or parent (all lowercase)?\n";
    cin >> response;
    if (response == "partner") {
        string partnerName;
        cout << "Enter new partner name: ";
        cin >> partnerName;

        Node* partnerNode;
        if (everyone.find(partnerName) != everyone.end()) {
            partnerNode = everyone[partnerName];
        } else {
            partnerNode = new Node(partnerName);
            everyone[partnerName] = partnerNode;
        }
        curr->addPartner(partnerNode);
    } else if (response == "child") {
        string childName;
        cout << "Enter new child name: ";
        cin >> childName; 

        Node* childNode;
        if (everyone.find(childName) != everyone.end()) {
            childNode = everyone[childName];
        } else {
            childNode = new Node(childName);
            everyone[childName] = childNode;
        }
        curr->addChildren(childNode);
    } else if (response == "parent") {
        string parentName;
        cout << "Enter new parent name: ";
        cin >> parentName;

        Node* parentNode;
        if (everyone.find(parentName) != everyone.end()) {
            parentNode = everyone[parentName];
        } else {
            parentNode = new Node(parentName);
            everyone[parentName] = parentNode;
        }
        curr->addParent(parentNode);
    } else {
        cout << "Response not recognised\n";
    }
}

void cmdSearch() {
    string name;
    cout << "Enter name: ";
    cin >> name;
    if (everyone.find(name) == everyone.end()) {
        cout << "Doesn't exist\n";
    } else {
        cout << "Exists\n";
    }
}

//file io
void fileOut() {
    ofstream out(treeName+".tree");
    out << everyone.size() << "\n";
    for (auto &i : everyone) {
        //val
        out << i.first << "\n-\n" << i.second->parents.size() << "\n";
        //parents
        for (int j = 0; j < i.second->parents.size(); j++) {
            out << i.second->parents[j]->val << "\n";
        }
        out << "-\n";
        //partner
        if (i.second->partner) {
            out << i.second->partner->val << "\n";
        }
        out << "-\n";
        //children
        out << i.second->children.size() << "\n";
        for (int j = 0; j < i.second->children.size(); j++) {
            out << i.second->children[j]->val << "\n";
        }
        out << "--\n";
    }
    cout << "File saved\n";
    out.close();
}

/*
unordered_map<string, pair<Node*, vector<vector<string>>>>
um[val] = [this, [[parents], [partner], [children]]]
*/

bool fileIn() {
    string fileName;
    cout << "Enter file to read from: ";
    cin >> fileName;
    
    ifstream in(fileName);
    if (!in.is_open()) {
        cout << "File could not be opened\n";
        return false;
    } else {
        cout << "File opened...\n";
    }

    unordered_map<string, pair<Node*, vector<vector<string>>>> temp;
    int total, len;
    in >> total;

    for (int i = 0; i < total; i++) {
        string value, partner, child, parent, useless;
        vector<string> _children, _parents, _partners;
        in >> value;
        in >> useless;
        if (useless != "-") {
            cout << "Error while reading file\n";
            return false;
        }
        //parents
        in >> len;
        for (int j = 0; j < len; j++) {
            in >> parent;
            _parents.push_back(parent);
        }
        in >> useless;
        if (useless != "-") {
            cout << "Error while reading file\n";
            return false;
        }
        //partner if applicable
        in >> partner;
        if (partner != "-") {
            _partners = {partner};
            in >> useless;
            if (useless != "-") {
                cout << "Error while reading file\n";
                return false;
            }
        }
        //children
        in >> len;
        for (int i = 0; i < len; i++) {
            in >> child;
            _children.push_back(child);
        }
        in >> useless;
        if (useless != "--") {
            cout << "Error while reading file\n";
            return false;
        }
        //create key-value in temp
        temp[value] = {new Node(value), {_parents, _partners, _children}};
    }
    everyone.clear();
    for (auto &i : temp) {
        //adds parents
        for (int j = 0; j < i.second.second[0].size(); j++) {
            i.second.first->addParent(temp[i.second.second[0][j]].first);
        }
        //adds partner
        if (i.second.second[1].size() == 1) {
            i.second.first->addPartner(temp[i.second.second[1][0]].first);
        }
        //adds children
        for (int j = 0; j < i.second.second[2].size(); j++) {
            i.second.first->addChildren(temp[i.second.second[2][j]].first);
        }
        everyone[i.first] = i.second.first;
    }
    if (fileName.size() > 4) {
        if (fileName.substr(fileName.size()-5) == ".tree") {
            for (int i = 0; i < 5; i++) {
                fileName.pop_back();
            }
        }
    }
    treeName = fileName;
    in.close();
    return true;
}

//main function (needs to be tidied up (make a function for every command))
void run() {
    string name, command = "";
    cout << "Enter the tree name (no spaces): ";
    cin >> treeName;
    cout << "Enter starting node name: ";
    cin >> name;

    Node* startNode = new Node(name);
    everyone[name] = startNode;
    Node* curr = startNode;

    while (true) {
        cout << "CLI-" << treeName << "-Tree > ";
        cin >> command;
        if (command == "stop") {
            break;
        } else if (command == "display") {
            curr->displayNode();
        } else if (command == "remove") {
            cmdRemove(curr);
        } else if (command == "add") {
            cmdAdd(curr);
        } else if (command == "delete") {
            curr->deleteNode();
        } else if (command == "search") {
            cmdSearch();
        } else if (command == "switch") {
            string name;
            cout << "Enter name to switch to: ";
            cin >> name;
            if (everyone.find(name) == everyone.end()) {
                cout << "Doesn't exist\n";
            } else {
                cout << "Switch successful\n";
                curr = everyone[name];
            }
        } else if (command == "listall") {
            listEveryone();
        } else if (command == "save") {
            fileOut();
        } else if (command == "load") {
            if (fileIn()) {
                cout << "Successfully loaded saved tree\n";
            }
        } else {
            cout << "Command not recognised, try something else.\n";
        }
    }
}

int main() {
    run();
    //cleanup of memory
    for (auto& i : everyone) {
        delete i.second;
    }
    return 0;
}
