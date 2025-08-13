#include <iostream>
#include <set>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Operator precedence
int precedence(char op) {
    if (op == '*') return 3;
    if (op == '.') return 2; // Explicit concatenation
    if (op == '|') return 1;
    return 0;
}

bool isOperator(char c) {
    return c == '*' || c == '|' || c == '.';
}

// Insert explicit concatenation
string insertConcat(string regex) {
    string res;
    for (int i = 0; i < regex.size(); ++i) {
        res += regex[i];
        if (i + 1 < regex.size()) {
            if ((isalnum(regex[i]) || regex[i] == ')' || regex[i] == '*') &&
                (isalnum(regex[i + 1]) || regex[i + 1] == '(')) {
                res += '.';
            }
        }
    }
    return res;
}

// Convert infix regex to postfix using Shunting Yard algorithm
string toPostfix(string regex) {
    string postfix;
    stack<char> st;
    for (char c : regex) {
        if (isalnum(c) || c == '#') {
            postfix += c;
        } else if (c == '(') {
            st.push(c);
        } else if (c == ')') {
            while (!st.empty() && st.top() != '(') {
                postfix += st.top();
                st.pop();
            }
            st.pop();
        } else {
            while (!st.empty() && precedence(st.top()) >= precedence(c)) {
                postfix += st.top();
                st.pop();
            }
            st.push(c);
        }
    }
    while (!st.empty()) {
        postfix += st.top();
        st.pop();
    }
    return postfix;
}

// Node for syntax tree
struct Node {
    char symbol;
    Node *left, *right;
    bool nullable;
    set<int> firstpos, lastpos;
    int pos;

    Node(char s) : symbol(s), left(nullptr), right(nullptr), nullable(false), pos(-1) {}
};

int position = 1; // Global position counter
map<int, char> posSymbol;
map<int, set<int>> followpos;

Node* buildSyntaxTree(string postfix) {
    stack<Node*> st;
    for (char c : postfix) {
        if (isalnum(c) || c == '#') {
            Node* node = new Node(c);
            node->pos = position;
            posSymbol[position++] = c;
            node->firstpos.insert(node->pos);
            node->lastpos.insert(node->pos);
            node->nullable = false;
            st.push(node);
        } else if (c == '*') {
            Node* child = st.top(); st.pop();
            Node* node = new Node(c);
            node->left = child;
            node->nullable = true;
            node->firstpos = child->firstpos;
            node->lastpos = child->lastpos;
            for (int i : child->lastpos) {
                followpos[i].insert(child->firstpos.begin(), child->firstpos.end());
            }
            st.push(node) ;
        } else if (c == '.') {
            Node* right = st.top(); st.pop();
            Node* left = st.top(); st.pop();
            Node* node = new Node(c);
            node->left = left;
            node->right = right;
            node->nullable = left->nullable && right->nullable;

            if (left->nullable) {
                node->firstpos.insert(left->firstpos.begin(), left->firstpos.end());
                node->firstpos.insert(right->firstpos.begin(), right->firstpos.end());
            } else {
                node->firstpos = left->firstpos;
            }

            if (right->nullable) {
                node->lastpos.insert(left->lastpos.begin(), left->lastpos.end());
                node->lastpos.insert(right->lastpos.begin(), right->lastpos.end());
            } else {
                node->lastpos = right->lastpos;
            }

            for (int i : left->lastpos) {
                followpos[i].insert(right->firstpos.begin(), right->firstpos.end());
            }

            st.push(node);
        } else if (c == '|') {
            Node* right = st.top(); st.pop();
            Node* left = st.top(); st.pop();
            Node* node = new Node(c);
            node->left = left;
            node->right = right;
            node->nullable = left->nullable || right->nullable;
            node->firstpos.insert(left->firstpos.begin(), left->firstpos.end());
            node->firstpos.insert(right->firstpos.begin(), right->firstpos.end());
            node->lastpos.insert(left->lastpos.begin(), left->lastpos.end());
            node->lastpos.insert(right->lastpos.begin(), right->lastpos.end());
            st.push(node);
        }
    }
    return st.top();
}

// DFA State
struct DFAState {
    set<int> positions;
    bool isFinal;
    map<char, int> transitions;
};

int getStateIndex(const set<int>& positions, vector<DFAState>& states) {
    for (int i = 0; i < states.size(); ++i) {
        if (states[i].positions == positions) return i;
    }
    return -1;
}

void buildDFA(Node* root) {
    vector<DFAState> states;
    vector<set<int>> unmarked;

    states.push_back({root->firstpos, root->firstpos.count(position - 1) > 0, {}});
    unmarked.push_back(root->firstpos);

    while (!unmarked.empty()) {
        set<int> current = unmarked.back();
        unmarked.pop_back();

        int currentIndex = getStateIndex(current, states);

        map<char, set<int>> symbolMoves;
        for (int p : current) {
            char symbol = posSymbol[p];
            if (symbol == '#') continue;
            symbolMoves[symbol].insert(followpos[p].begin(), followpos[p].end());
        }

        for (auto [symbol, nextSet] : symbolMoves) {
            int idx = getStateIndex(nextSet, states);
            if (idx == -1) {
                bool isFinal = nextSet.count(position - 1) > 0;
                states.push_back({nextSet, isFinal, {}});
                unmarked.push_back(nextSet);
                idx = states.size() - 1;
            }
            states[currentIndex].transitions[symbol] = idx;
        }
    }

    // Print DFA
    cout << "\nDFA:\n";
    for (int i = 0; i < states.size(); ++i) {
        cout << "State " << i << (states[i].isFinal ? " (final)" : "") << ": ";
        for (auto [ch, to] : states[i].transitions) {
            cout << ch << "->" << to << " ";
        }
        cout << endl;
    }
}

int main() {
    string regex;
    cout << "Enter regex (example: (a|b)*abb): ";
    cin >> regex;

    regex = insertConcat(regex);
    regex += ".#"; // Add end marker and concatenate
    regex = insertConcat(regex);

    string postfix = toPostfix(regex);
    Node* root = buildSyntaxTree(postfix);

    buildDFA(root);
    return 0;
}
