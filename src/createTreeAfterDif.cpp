#include "createTreeAfterDif.h"

node_t * getGrammarForDif (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");

    node_t * firstNode = getExpressionForDif(node);

    return firstNode;
}

node_t * getExpressionForDif (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    while (node->op_t == OP_ADD || node->op_t == OP_SUB)
    {
        node_t * leftNode = getExpressionForDif (node->left);
        node_t * rightNode = getExpressionForDif (node->right);
        node_t * headNode = nullptr;

        if (node->op_t == OP_ADD)
        {
            headNode = createNodeWithOperation (OP_ADD, leftNode, rightNode);
        }
        else
        {
            headNode = createNodeWithOperation (OP_SUB, leftNode, rightNode);
        }

        return headNode;
    }
    return (difMulDiv (node));
}

node_t * difMulDiv (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    while (node->op_t == OP_MUL)
    {
        node_t * difLeftNode = getExpressionForDif (node->left);
        node_t * difRightNode = getExpressionForDif (node->right);

        node_t * copyLeftNode  = copyNode (node->left);
        node_t * copyRightNode = copyNode (node->right);

        node_t * firstMul = createNodeWithOperation (OP_MUL, difLeftNode, copyRightNode);
        node_t * secondMul = createNodeWithOperation (OP_MUL, copyLeftNode, difRightNode);

        node_t * headNodeForMul = createNodeWithOperation (OP_ADD, firstMul, secondMul);

        return headNodeForMul;
    }
    while (node->op_t == OP_DIV)
    {
        node_t * difLeftNode = getExpressionForDif (node->left);
        node_t * difRightNode = getExpressionForDif (node->right);

        node_t * copyLeftNode  = copyNode (node->left);
        node_t * copyRightNode = copyNode (node->right);

        node_t * firstMul = createNodeWithOperation (OP_MUL, difLeftNode, copyRightNode);
        node_t * secondMul = createNodeWithOperation (OP_MUL, copyLeftNode, difRightNode);

        node_t * headNodeForNumerator = createNodeWithOperation (OP_SUB, firstMul, secondMul);

        node_t * degIndicator = createNodeWithNum (2);
        node_t * copyRightNodeForDenominator = copyNode (node->right);
        node_t * headNodeForDenominator = createNodeWithOperation (OP_DEG, copyRightNodeForDenominator, degIndicator);

        node_t * headNodeForDiv = createNodeWithOperation (OP_DIV, headNodeForNumerator, headNodeForDenominator);

        return headNodeForDiv;
    }
    return (difDegree(node));
}

node_t * difDegree (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");

    while (node->op_t == OP_DEG)
    {
        if (node->right->type == NUM_T)
        {
            node_t * copyDegreeIndicator = copyNode (node->right);
            node_t * unit = createNodeWithNum (1);
            node_t * newDegreeIndicator = createNodeWithOperation (OP_SUB, copyDegreeIndicator, unit);

            node_t * copyLeftNode = copyNode (node->left);
            node_t * firstMul = createNodeWithOperation (OP_DEG, copyLeftNode, newDegreeIndicator);

            node_t * secondMul = copyNode (node->right);
            node_t * leftDescendant = createNodeWithOperation (OP_MUL, secondMul, firstMul);

            node_t * difOfDegreeBase = getExpressionForDif (node->left);
            node_t * headNodeForDeg = createNodeWithOperation (OP_MUL, leftDescendant, difOfDegreeBase);

            return headNodeForDeg;
        }
        else
        {
            node_t * copyExpression = copyNode (node);
            node_t * func = createNodeWithFunction ((char*) "ln");
            node_t * argOfFunc = createNodeWithNum (2);
            func->left = argOfFunc;
            argOfFunc->parent = func;
            node_t * firstMul = createNodeWithOperation (OP_MUL, copyExpression, func);

            node_t * difRightNode = getExpressionForDif (node->right);

            node_t * headNodeForExpFunc = createNodeWithOperation (OP_MUL, firstMul, difRightNode);

            return headNodeForExpFunc;
        }
    }
    return (difNumberOrVar(node));
}

node_t * difNumberOrVar (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    if (node->type == NUM_T)
    {
        return (createNodeWithNum(0));
    }
    else if (node->type == VAR_T)
    {
        return (createNodeWithNum(1));
    }
    else if (node->type == FUNC_T)
    {
        return (difFunc (node));
    }
    else
    {
        MY_ASSERT (1, "Wrong type");
        return nullptr;
    }
}

node_t * difFunc (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    if (myStrcmp (node->nameFunc, "ln") == 0)
    {
        node_t * unit = createNodeWithNum (1);
        node_t * copyArgument = copyNode (node->left);
        node_t * leftDescendant = createNodeWithOperation (OP_DIV, unit, copyArgument);
        node_t * rightDescendant = getExpressionForDif (node->left);
        node_t * headNode = createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);

        return headNode;
    }

    else if (myStrcmp (node->nameFunc, "sin") == 0)
    {
        node_t * leftDescendant = createNodeWithFunction ((char *) "cos");
        node_t * copyArgument = copyNode (node->left);
        leftDescendant->left = copyArgument;
        copyArgument->parent = leftDescendant;
        node_t * rightDescendant = getExpressionForDif (node->left);
        node_t * headNode = createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);
        rightDescendant->parent = headNode;

        return headNode;
    }
    else if (myStrcmp (node->nameFunc, "cos") == 0)
    {
        node_t * unit = createNodeWithNum (-1);
        node_t * copyArgument = copyNode (node->left);
        node_t * difCos = createNodeWithFunction ((char *) "sin");
        difCos->left = copyArgument;
        copyArgument->parent = difCos;
        node_t * leftDescendant = createNodeWithOperation (OP_MUL, unit, difCos);
        node_t * rightDescendant = getExpressionForDif (node->left);
        node_t * headNode =  createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);

        return headNode;
    }
    else if (myStrcmp (node->nameFunc, "tg") == 0)
    {
        node_t * unit = createNodeWithNum (1);
        node_t * degIndicator = createNodeWithNum (2);
        node_t * copyArgument = copyNode (node->left);
        node_t * difSin = createNodeWithFunction ((char *) "cos");
        difSin->left = copyArgument;
        copyArgument->parent = difSin;
        node_t * degree = createNodeWithOperation (OP_DEG, difSin, degIndicator);
        node_t * leftDescendant = createNodeWithOperation (OP_DIV, unit, degree);
        node_t * rightDescendant = getExpressionForDif (node->left);
        node_t * headNode = createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);

        return headNode;
    }
    else
    {
        return node;
    }
}
