#include <string>
#include <stack>
#include <iostream>
#include <cctype>
#include <cassert>

class NodeBase {
public:
    virtual int caculate() const = 0;
    virtual ~NodeBase() {}
};

class Node {
public:
    Node(NodeBase *p): node_(p), use_(new int(1)) {
        std::cout << "call default constructor of Node" << std::endl;
    }

    Node(const Node& n) {
        std::cout << "call copy constructor of Node" << std::endl;
        node_ = n.node_;
        use_ = n.use_;
        ++*use_;
    } 

    Node& operator=(const Node& rhs) {
        std::cout << "call operator= of Node" << std::endl;
        DecrUse();
        node_ = rhs.node_;
        use_ = rhs.use_;
        ++*use_;
    }

    int caculate() const {
        return node_->caculate();
    }

    ~Node() {
        std::cout << "call destructor of Node" << std::endl;
        std::cout << "use" << *use_ << std::endl;
        DecrUse();
    }
private:
    NodeBase *node_;
    int *use_;

    void DecrUse() {
        if (--*use_ == 0) {
            std::cout << "call delete in DecrUse" << std::endl;
            delete use_;
            delete node_;
        }
    }
};

class NumberNode:public NodeBase {
public:
    NumberNode(int num): number_(num) {
        std::cout << "call constructor of NumberNode " << number_ << std::endl;
    }
    int caculate() const {
        std::cout << "call NumberNode caculate" << std::endl;
        return number_;
    }

    ~NumberNode() {
        std::cout << "call destructor of NumberNode " << number_ << std::endl;
    }

private:
    int number_;
};

class BinaryNode:public NodeBase {
public:
//protected:
    class NodeBase* left_;
    class NodeBase* right_;
};

class AddNode:public BinaryNode {
public:
    AddNode() {
        std::cout << "call constructor of AddNode" << std::endl;
    }
    ~AddNode() {
        std::cout << "call destructor of AddNode" << std::endl;
    }

    int caculate() const {
        std::cout << "call AddNode caculate" << std::endl;
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

//this function should return Node, copy constructor will not be called,
//cause g++ optimization  
Node SexpToEtree(const std::string exp) 
{
    std::stack<NodeBase *> stk;
    std::string::const_iterator it = exp.begin();

    while (it != exp.end()) {
        if (isdigit(*it)) {
            NodeBase *node = new NumberNode(*it - '0');
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

    Node node(stk.top());
    return node;
    //return *(new Node(stk.top()));
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

    //NodeBase *root = SexpToEtree("2");
    //assert (root->caculate() == 2);

    //Node root = SexpToEtree("2");
    //assert (root.caculate() == 2);
    //std::cout << "next" << std::endl;

    //root = SexpToEtree("4");
    //assert (root.caculate() == 4);

    Node root = SexpToEtree("34+");
    assert (root.caculate() == 7);
    //std::cout << "next" << std::endl;

    //assert (root->caculate() == 6);
    //std::cout << root->caculate() << std::endl;

    //root = SexpToEtree("245++");
    //assert (root->caculate() == 11);
    //std::cout << root->caculate() << std::endl;

    //root = SexpToEtree("245+*");
    //assert (root->caculate() == 18);
    //std::cout << root->caculate() << std::endl;

    //root = SexpToEtree(DexpToSexp("2+4*5"));
    //assert (root->caculate() == 22);
    //std::cout << root->caculate() << std::endl;

    //root = SexpToEtree(DexpToSexp("2*4+5"));
    //assert (root->caculate() == 13);
    //std::cout << root->caculate() << std::endl;
    return 0;
}
