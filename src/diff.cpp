#include "diff.h"

static node_t * diffAddSub (node_t * node);
static node_t * diffMulDiv (node_t * node);
static node_t * diffDegree (node_t * node);
static node_t * diffNumVar (node_t * node);
static node_t * diffFunc (node_t * node);

#define NUM(number)         \
    createNodeWithNum (number)

#define COPY(node)          \
    copyNode (node)

#define DIV(left, right)    \
    createNodeWithOperation (OP_DIV, left, right)

#define DIF_NODE(node)      \
    diffAddSub (node)

#define MUL(left, right)    \
    createNodeWithOperation (OP_MUL, left, right)

#define FUNC(nameFunc, leftDescendant)              \
    createNodeWithFunction (nameFunc, leftDescendant)

#define DEG(left, right)                            \
    createNodeWithOperation (OP_DEG, left, right)

#define ADD(left, right)                            \
    createNodeWithOperation (OP_ADD, left, right)

#define SUB(left,right)                             \
    createNodeWithOperation (OP_SUB, left, right)

//---------------------------------------------------------------------------------------------------

node_t * diff (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");

    node_t * firstNode = DIF_NODE(node);

    return firstNode;
}

node_t * diffAddSub (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    while (node->op_t == OP_ADD || node->op_t == OP_SUB)
    {
        node_t * leftNode = DIF_NODE (node->left);
        node_t * rightNode = DIF_NODE (node->right);
        node_t * headNode = nullptr;

        if (node->op_t == OP_ADD)
        {
            headNode = ADD(leftNode, rightNode);
        }
        else
        {
            headNode = SUB(leftNode, rightNode); 
        }

        return headNode;
    }
    return (diffMulDiv (node));
}

node_t * diffMulDiv (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    while (node->op_t == OP_MUL)
    {
        return ADD(MUL(DIF_NODE(node->left),COPY(node->right)), MUL(COPY(node->left), DIF_NODE(node->right)));
    }
    while (node->op_t == OP_DIV)
    {
        return DIV(SUB(MUL(DIF_NODE(node->left), COPY(node->right)), MUL(COPY(node->left), DIF_NODE(node->right))), DEG(COPY(node->right), NUM(2)));
    }
    return (diffDegree(node));
}

node_t * diffDegree (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");

    while (node->op_t == OP_DEG)
    {
        if (node->right->type == NUM_T)
        {
            return MUL(MUL(COPY(node->right), DEG(COPY(node->left), SUB(COPY(node->right), NUM(1)))), DIF_NODE(node->left));
        }
        else
        {
            return MUL(MUL(COPY(node), FUNC("ln", NUM(2))), DIF_NODE(node->right));
        }
    }
    return (diffNumVar(node));
}

node_t * diffNumVar (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    if (node->type == NUM_T)
    {
        return (NUM(0));
    }
    else if (node->type == VAR_T)
    {
        return (NUM(1));
    }
    else if (node->type == FUNC_T)
    {
        return (diffFunc (node));
    }
    else
    {
        MY_ASSERT (1, "Wrong type");
        return nullptr;
    }
}

node_t * diffFunc (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    if (myStrcmp (node->nameFunc, "ln") == 0)
    {
        return MUL(DIV(NUM(1), COPY(node->left)), DIF_NODE(node->left));
    }
    else if (myStrcmp (node->nameFunc, "sin") == 0)
    {
        return MUL(FUNC("cos", COPY(node->left)), DIF_NODE(node->left));

    }
    else if (myStrcmp (node->nameFunc, "cos") == 0)
    {
        return MUL(MUL(NUM(-1),FUNC("sin", COPY(node->left))), DIF_NODE(node->left));
    }
    else if (myStrcmp (node->nameFunc, "tg") == 0)
    {
        return MUL(DIV(NUM(1),DEG(FUNC("cos", COPY(node->left)), NUM(2))), DIF_NODE(node->left));
    }
    else
    {
        return node;
    }
}
