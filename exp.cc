#include <string>
#include <stack>
#include <iostream>
#include <cctype>
#include <cassert>

class Node {
public:
    virtual int caculate() const = 0;
    virtual ~Node() {}
};

class NumberNode:public Node {
public:
    NumberNode(int num): number_(num) {}
    int caculate() const {
        //std::cout << "call NumberNode" << std::endl;
        return number_;
    }

private:
    int number_;
    class Node* left_;
    class Node* right_;
};

class BinaryNode:public Node {
public:
    //int caculate() const {
        //return number_;
    //}
//protected:
    //class NumberNode* left_;
    //class NumberNode* right_;
    class Node* left_;
    class Node* right_;
};

class AddNode:public BinaryNode {
public:
    int caculate() const {
        return left_->caculate() + right_->caculate();
    }
};

class MultiNode:public BinaryNode {
public:
    int caculate() const {
        return left_->caculate() * right_->caculate();
    }
};

static const std::string op = "+*()";

static int CalcPriority(const char ch)
{
    return op.find(ch);
}

static bool IsOperator(char ch)
{
    return (std::string::npos != op.find(ch));
}

static std::string DexpToSexp(const std::string &exp)
{
    std::stack<char> stk;
    std::string sexp;
    std::string::const_iterator it = exp.begin();

    while (it != exp.end()) {
        if (isdigit(*it)) {
            sexp.push_back(*it);
        }
        else if (IsOperator(*it)) {
            if (*it == ')') {
                while (stk.top() != '(') {
                    sexp.push_back(stk.top());
                    stk.pop();
                }
                stk.pop();
            } else if (stk.empty() || 
                    (CalcPriority(*it) > CalcPriority(stk.top()))) {
                stk.push(*it);
            } else {
                if (stk.top() != '(') {
                    do {
                        sexp.push_back(stk.top());
                        stk.pop();
                    } while ((stk.empty() == false) && 
                            (CalcPriority(*it) <= CalcPriority(stk.top())));
                }

                stk.push(*it);
            }
        }
        it++;
    }

    while (stk.empty() == false) {
        sexp.push_back(stk.top());
        stk.pop();
    }

    //std::cout << sexp << std::endl;
    return sexp;
}

Node* SexpToEtree(const std::string exp) 
{
    std::stack<Node *> stk;
    std::string::const_iterator it = exp.begin();

    while (it != exp.end()) {
        if (isdigit(*it)) {
            Node *node = new NumberNode(*it - '0');
            stk.push(node);
            //std::cout << "SexpToEtree push\t" << *it - '0' << std::endl;
        } else if (*it == '+') {
            AddNode *node = new AddNode();
            node->right_ = stk.top();
            stk.pop();
            node->left_ = stk.top();
            stk.pop();
            //std::cout << "SexpToEtree pop\t" << std::endl;
            stk.push(node);
        } else if (*it == '*') {
            MultiNode *node = new MultiNode();
            node->right_ = stk.top();
            stk.pop();
            node->left_ = stk.top();
            stk.pop();
            //std::cout << "SexpToEtree pop\t" << std::endl;
            stk.push(node);
        }
        it++;
    }

    //std::cout << "SexpToEtree pop end" << std::endl;
    return stk.top();
}

int main(int argc, const char *argv[])
{
    //std::string expression = "2+4*5+(2*3+1)*3";
    //std::string expression = "2+4*5";
    std::string suffix_exp;

    assert (DexpToSexp("2") == "2");
    assert (DexpToSexp("2+4") == "24+");
    assert (DexpToSexp("2+4*5") == "245*+");
    assert (DexpToSexp("2*4+5") == "24*5+");
    assert (DexpToSexp("(2)") == "2");
    //assert (DexpToSexp("((2))") == "2");
    assert (DexpToSexp("2+4*(5+1)") == "2451+*+");

    //std::cout << suffix_exp << std::endl;

    Node *root = SexpToEtree("2");
    assert (root->caculate() == 2);
    //std::cout << root->caculate() << std::endl;

    root = SexpToEtree("24+");
    assert (root->caculate() == 6);
    //std::cout << root->caculate() << std::endl;

    root = SexpToEtree("245++");
    assert (root->caculate() == 11);
    //std::cout << root->caculate() << std::endl;

    root = SexpToEtree("245+*");
    assert (root->caculate() == 18);
    //std::cout << root->caculate() << std::endl;

    root = SexpToEtree(DexpToSexp("2+4*5"));
    assert (root->caculate() == 22);
    //std::cout << root->caculate() << std::endl;

    root = SexpToEtree(DexpToSexp("2*4+5"));
    assert (root->caculate() == 13);
    //std::cout << root->caculate() << std::endl;
    return 0;
}
