#include "simplify.h"

static void simplifyDegOrDivToOne (node_t ** node);
static void simplifyMulByZero (node_t * node);
static void simplifyMulByOne (node_t ** node);
static void simplifyAddWithZero (node_t ** node);
static int compareFractionalNum (double firstNum, double secondNum);
static void removeConstants (node_t ** node);

#define NODE_OP(OP)                                                             \
    newNode->elem = (*node)->right->elem OP (*node)->left->elem;                \
    deleteNode ((*node)->right);                                                \
    deleteNode ((*node)->left);                                                 \
    deleteNode ((*node));                                                       \
    (*node) = newNode

#define LEFT_NODE_OP(OP)                                                        \
    newNode->elem = (*node)->left->left->elem OP (*node)->left->right->elem;    \
    deleteNode ((*node)->left->left);                                           \
    deleteNode ((*node)->left->right);                                          \
    deleteNode ((*node)->left);                                                 \
    (*node)->left = newNode

#define RIGHT_NODE_OP(OP)                                                       \
    newNode->elem = (*node)->right->left->elem OP (*node)->right->right->elem;  \
    deleteNode ((*node)->right->left);                                          \
    deleteNode ((*node)->right->right);                                         \
    deleteNode ((*node)->right);                                                \
    (*node)->right = newNode

void simplify (node_t ** node)
{
    for (int i = 0; i < 3; i++)
    {
        removeConstants (node);
        simplifyDegOrDivToOne (node);
        simplifyMulByZero (*node);
        simplifyAddWithZero (node);
        simplifyMulByOne (node);
    }
}

void removeConstants (node_t ** node)
{
    MY_ASSERT ((*node) == nullptr, "There is no access to this node");
    if ((*node)->type == OPER_T && ((*node)->right != nullptr) && ((*node)->left != nullptr) &&
        (*node)->right->type == NUM_T && (*node)->left->type == NUM_T)
    {
        node_t * newNode = createNodeWithNum (1);
        if ((*node)->op_t == OP_ADD)
        {
            NODE_OP (+);
        }
        else if ((*node)->op_t == OP_SUB)
        {
            NODE_OP (-);
        }
        else if ((*node)->op_t == OP_MUL)
        {
            NODE_OP (*);
        }
        else if ((*node)->op_t == OP_DIV)
        {
            NODE_OP (/);
        }
        else if ((*node)->op_t == OP_DEG)
        {
            newNode->elem = pow ((*node)->right->elem, (*node)->left->elem);
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else
        {
            MY_ASSERT (1, "There is no such operation");
        }
    }

    if ((*node)->left != nullptr)
    {
        removeConstants (&((*node)->left));
    }
    if ((*node)->right != nullptr)
    {
        removeConstants (&((*node)->right));
    }
}

void simplifyDegOrDivToOne (node_t ** node)
{
    MY_ASSERT (node == nullptr, "There is no access to node for simplify");
    MY_ASSERT ((*node) == nullptr, "There is no access to node for simplify");

    if (((*node)->op_t == OP_DIV || (*node)->op_t == OP_DEG) &&
        ((*node)->left != nullptr) && (compareFractionalNum((*node)->right->elem, 1) == 1))
    {
        node_t * saveLeftNode = (*node)->left;

        deleteNode ((*node)->right);
        deleteNode ((*node));

        (*node) = saveLeftNode;
    }

    if ((*node)->left != nullptr)
    {
        simplifyDegOrDivToOne (&((*node)->left));
    }
    if ((*node)->right != nullptr)
    {
        simplifyDegOrDivToOne (&((*node)->right));
    }
}

void simplifyMulByOne (node_t ** node)
{
    MY_ASSERT (node == nullptr, "There is no access to node for simplify");
    MY_ASSERT ((*node) == nullptr, "There is no access to node for simplify");

    if (((*node)->op_t == OP_MUL) && (((*node)->right != nullptr) ) &&
        (compareFractionalNum((*node)->right->elem, 1) == 1))
    {
        node_t * saveLeftNode = (*node)->left;

        deleteNode ((*node)->right);
        deleteNode ((*node));

        (*node) = saveLeftNode;
    }
    if (((*node)->op_t == OP_MUL) && ((*node)->left != nullptr) &&
        (compareFractionalNum((*node)->left->elem, 1) == 1))
    {
        node_t * saveRightNode = (*node)->right;

        deleteNode ((*node)->left);
        deleteNode ((*node));

        (*node) = saveRightNode;
    }

    if ((*node)->left != nullptr)
    {
        simplifyMulByOne (&((*node)->left));
    }
    if ((*node)->right != nullptr)
    {
        simplifyMulByOne (&((*node)->right));
    }
}

void simplifyAddWithZero (node_t ** node)
{
    MY_ASSERT ((*node) == nullptr, "There is no access to node for simplify");
    MY_ASSERT (node == nullptr, "There is no access to node for simplify");

    if ((*node)->op_t == OP_ADD)
    {
        if (((*node)->right != nullptr) && ((*node)->right->type == NUM_T) &&
            (compareFractionalNum((*node)->right->elem, 0) == 1))
        {
            deleteNode ((*node)->right);
            (*node)->right = nullptr;
            node_t * newNode = copyNode ((*node)->left);
            MY_ASSERT (newNode == nullptr, "The left node cannot be copied");
            deleteTree ((*node)->left);
            (*node)->left = nullptr;
            deleteNode (*node);
            (*node) = newNode;
        }
        if (((*node)->left != nullptr) && ((*node)->left->type == NUM_T) &&
            (compareFractionalNum((*node)->left->elem, 0) == 1))
        {
            deleteNode ((*node)->left);
            (*node)->left = nullptr;
            node_t * newNode = copyNode ((*node)->right);
            MY_ASSERT (newNode == nullptr, "The left node cannot be copied");
            deleteTree ((*node)->right);
            (*node)->right = nullptr;
            deleteNode (*node);
            (*node) = newNode;
        }
    }

    if ((*node)->left != nullptr)
    {
        simplifyAddWithZero (&((*node)->left));
    }
    if ((*node)->right != nullptr)
    {
        simplifyAddWithZero (&((*node)->right));
    }
}

void simplifyMulByZero (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    if ((node->op_t == OP_MUL) &&
        (((node->right != nullptr) && (node->right->type == NUM_T) &&
            (compareFractionalNum(node->right->elem, 0) == 1)) ||
        ((node->left != nullptr) && (node->left->type == NUM_T) &&
            (compareFractionalNum(node->left->elem, 0) == 1))))
    {
        deleteNode (node->right);
        deleteNode (node->left);

        node->elem = 0;
        node->left = nullptr;
        node->right = nullptr;
        node->op_t = OP_ERROR;
        node->type = NUM_T;
    }

    if (node->left != nullptr)
    {
        simplifyMulByZero (node->left);
    }
    if (node->right != nullptr)
    {
        simplifyMulByZero (node->right);
    }
}

int compareFractionalNum (double firstNum, double secondNum)
{
    return (abs(firstNum-secondNum) < 0.00001);
}
