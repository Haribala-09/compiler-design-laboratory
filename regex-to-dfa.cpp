#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using u128 = unsigned __int128;

#ifndef ONLINE_JUDGE
#include <debug_template.hpp>
#else
#define debug(...)
#define debugArr(...)
#endif

inline int precedence(char op)
{
    if (op == '*')
        return 3;
    if (op == '.')
        return 2;
    if (op == '|')
        return 1;
    return 0;
}

inline bool isOperator(char c)
{
    return c == '*' || c == '.' || c == '|';
}

string topostfix(string &s)
{
    // ab -> a.b
    auto add_concat = [&]() -> string
    {
        string res = "";
        for (int i = 0; i < s.size(); i++)
        {
            char c = s[i];
            res += c;

            if (c == '(' || c == '|')
                continue;

            if (i + 1 < s.size())
            {
                char nc = s[i + 1];
                if (nc == ')' || nc == '*' || nc == '|')
                    continue;
                res += '.';
            }
        }
        return res;
    };

    s = add_concat();
    cout << "concat:" << s << '\n';
    stack<char> st;
    string postfix = "";
    postfix.reserve(s.size());

    for (auto c : s)
    {
        if (isalpha(c) || c == '#')
        {
            postfix += c;
        }
        else if (c == '(')
        {
            st.push(c);
        }
        else if (c == ')')
        {

            while (!st.empty() && st.top() != '(')
            {
                postfix += st.top();
                st.pop();
            }
            st.pop();
        }
        else
        {

            while (!st.empty() && isOperator(st.top()) && precedence(st.top()) >= precedence(c))
            {
                postfix += st.top();
                st.pop();
            }
            st.push(c);
        }
    }

    while (!st.empty())
    {
        postfix += st.top();
        st.pop();
    }
    return postfix;
}

struct node
{
    char symbol;
    set<int> first, last;
    node *left, *right;
    bool nullable = 0;
    int position;
    node(char c) : symbol(c), left(nullptr), right(nullptr), nullable(false), position(-1) {}

    void print()
    {
        cerr << "Node debug:\n";
        cerr << "Nullable:" << nullable << '\n';
        cerr << "position:" << position << '\n';
        cerr << "first: ";
        for (auto c : first)
            cerr << c << ' ';
        cerr << "\nlast: ";
        for (auto c : last)
            cerr << c << ' ';
        cerr << '\n';
    }
};

int pos = 1;
map<int, char> pos_to_symbol; // map for pos to symbol
map<int, set<int>> follow;

// build tree returns root node
node *build(const string &postfix)
{

    stack<node *> st;
    for (auto c : postfix)
    {

        if (isalpha(c) || c == '#')
        {
            node *newnode = new node(c);
            newnode->position = pos;
            pos_to_symbol[pos++] = c;

            newnode->first.insert(newnode->position);
            newnode->last.insert(newnode->position);

            st.push(newnode);
        }
        else if (c == '*')
        {
            assert(!st.empty());
            auto c1 = st.top();
            st.pop();

            node *newnode = new node(c);
            newnode->first = c1->first;
            newnode->last = c1->last;
            newnode->nullable = true;

            newnode->left = c1;

            // finding follow
            for (int val : c1->last)
            {
                follow[val].insert(c1->first.begin(), c1->first.end());
            }
            st.push(newnode);
        }
        else if (c == '.')
        {
            assert(!st.empty());
            auto c1 = st.top();
            st.pop();
            assert(!st.empty());
            auto c2 = st.top();
            st.pop();

            // stack operation buddy
            swap(c1, c2);

            node *newnode = new node(c);
            newnode->left = c1;
            newnode->right = c2;
            newnode->nullable = c1->nullable && c2->nullable;

            if (c1->nullable)
            {
                newnode->first.insert(c1->first.begin(), c1->first.end());
                newnode->first.insert(c2->first.begin(), c2->first.end());
            }
            else
            {
                newnode->first.insert(c1->first.begin(), c1->first.end());
            }

            if (c2->nullable)
            {
                newnode->last.insert(c1->last.begin(), c1->last.end());
                newnode->last.insert(c2->last.begin(), c2->last.end());
            }
            else
            {
                newnode->last.insert(c2->last.begin(), c2->last.end());
            }

            // follow
            for (int val : c1->last)
            {
                follow[val].insert(c2->first.begin(), c2->first.end());
            }
            st.push(newnode);
        }
        else if (c == '|')
        {
            assert(!st.empty());
            auto c1 = st.top();
            st.pop();
            assert(!st.empty());
            auto c2 = st.top();
            st.pop();

            // stack operation buddy
            swap(c1, c2);

            node *newnode = new node(c);
            newnode->left = c1;
            newnode->right = c2;
            newnode->nullable = c1->nullable || c2->nullable;

            newnode->first.insert(c1->first.begin(), c1->first.end());
            newnode->first.insert(c2->first.begin(), c2->first.end());

            newnode->last.insert(c1->last.begin(), c1->last.end());
            newnode->last.insert(c2->last.begin(), c2->last.end());

            st.push(newnode);
            // no follow for |
        }
    }
    return st.top();
}

struct dfa
{
    set<int> pos;
    bool isfinal=false;
    map<char,int> transitions;
};

void build_dfa(node *root)
{
    
}
int main()
{
    int t;
    cin >> t;
    while (t--)
    {
        string s;
        cin >> s;
        debug("-------------------------------------\n");

        s = "(" + s + ")#"; // add null char at the end
        auto str = topostfix(s);

        debug(str);
        node *root = build(str);
        debug(pos_to_symbol);

        // debug node
        root->print();
        debug(follow);
        build_dfa(root);
        debug("--------------------------------------\n");
    }
}