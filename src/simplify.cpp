#include "simplify.h"

void simplifyExpression (node_t ** node)
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
            newNode->elem = (*node)->right->elem + (*node)->left->elem;
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else if ((*node)->op_t == OP_SUB)
        {
            newNode->elem = (*node)->right->elem + (*node)->left->elem;
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else if ((*node)->op_t == OP_MUL)
        {
            newNode->elem = (*node)->right->elem * (*node)->left->elem;
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else if ((*node)->op_t == OP_DIV)
        {
            newNode->elem = (*node)->right->elem / (*node)->left->elem;
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
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
    if (((*node)->left != nullptr) && ((*node)->left->left != nullptr) && ((*node)->left->right != nullptr) &&
        ((*node)->left->type == OPER_T) && ((*node)->left->left->type == NUM_T) && ((*node)->left->right->type == NUM_T))
    {
        node_t * newNode = createNodeWithNum (1);
        if ((*node)->left->op_t == OP_ADD)
        {
            newNode->elem = (*node)->left->left->elem + (*node)->left->right->elem;
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else if ((*node)->left->op_t == OP_SUB)
        {
            newNode->elem = (*node)->left->left->elem - (*node)->left->right->elem;
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else if ((*node)->left->op_t == OP_MUL)
        {
            newNode->elem = (*node)->left->left->elem * (*node)->left->right->elem;
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else if ((*node)->left->op_t == OP_DIV)
        {
            newNode->elem = (*node)->left->left->elem / (*node)->left->right->elem;
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else if ((*node)->left->op_t == OP_DEG)
        {
            newNode->elem = pow((*node)->left->left->elem, (*node)->left->right->elem);
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else
        {
            MY_ASSERT (1, "There is no such operation");
        }
    }
    if (((*node)->right != nullptr) && ((*node)->right->left != nullptr) &&
        ((*node)->right->right != nullptr) && ((*node)->right->left->type == NUM_T) &&
        ((*node)->right->right->type == NUM_T) && ((*node)->right->type == OPER_T))
    {
        node_t * newNode = createNodeWithNum (1);

        // COPYPASTE!!!!!!
        if ((*node)->right->op_t == OP_ADD)
        {
            newNode->elem = (*node)->right->left->elem + (*node)->right->right->elem;
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else if ((*node)->right->op_t == OP_SUB)
        {
            newNode->elem = (*node)->right->left->elem - (*node)->right->right->elem;
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else if ((*node)->right->op_t == OP_MUL)
        {
            newNode->elem = (*node)->right->left->elem * (*node)->right->right->elem;
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else if ((*node)->right->op_t == OP_DIV)
        {
            newNode->elem = (*node)->right->left->elem / (*node)->right->right->elem;
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else if ((*node)->right->op_t == OP_DEG)
        {
            newNode->elem = pow ((*node)->right->left->elem, (*node)->right->right->elem);
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else
        {
            MY_ASSERT (1, "There is no such operation\n");
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

    if (((*node)->left != nullptr) && ((*node)->left->op_t == OP_DEG || (*node)->left->op_t == OP_DIV) &&
        ((*node)->left->right != nullptr) && (compareFractionalNum((*node)->left->right->elem, 1) == 1))
    {
        node_t * saveNodeLeftLeft = (*node)->left->left;

        deleteNode ((*node)->left->right);
        deleteNode ((*node)->left);

        (*node)->left = saveNodeLeftLeft;
    }
    else if (((*node)->right != nullptr) && ((*node)->right->op_t == OP_DEG || (*node)->right->op_t == OP_DIV) &&
            ((*node)->right->right != nullptr) && (compareFractionalNum((*node)->right->right->elem, 1) == 1))
    {
        node_t * saveNodeRightLeft = (*node)->right->left;
        deleteNode ((*node)->right->right);
        deleteNode ((*node)->right);

        (*node)->right = saveNodeRightLeft;
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

    if (((*node)->left != nullptr) && ((*node)->left->op_t == OP_MUL))
    {
        if (((*node)->left->right != nullptr) &&
            compareFractionalNum((*node)->left->right->elem, 1) == 1)
        {
            node_t * saveNodeLeftLeft = (*node)->left->left;

            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);

            (*node)->left = saveNodeLeftLeft;
        }
        if (((*node)->left->left != nullptr) &&
            (compareFractionalNum((*node)->left->left->elem, 1) == 1))
        {
            node_t * saveNodeLeftRight = (*node)->left->right;

            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left);

            (*node)->left = saveNodeLeftRight;
        }
    }
    if (((*node)->right != nullptr) && ((*node)->right->op_t == OP_MUL))
    {
        if (((*node)->right->right != nullptr) &&
            (compareFractionalNum((*node)->right->right->elem, 1) == 1))
        {
            node_t * saveNodeLeftLeft = (*node)->right->left;

            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);

            (*node)->right = saveNodeLeftLeft;
        }
        if (((*node)->right->left != nullptr) &&
            (compareFractionalNum((*node)->right->left->elem, 1) == 1))
        {
            node_t * saveNodeLeftRight = (*node)->right->right;

            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right);

            (*node)->right = saveNodeLeftRight;
        }
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
        if (((*node)->left != nullptr) && ((*node)->left->op_t == OP_ADD))
        {
            if (((*node)->left->right != nullptr) && ((*node)->left->right->type == NUM_T) &&
                (compareFractionalNum((*node)->left->right->elem, 0) == 1))
            {
                node_t * saveNodeLeftLeft = (*node)->left->left;

                deleteNode ((*node)->left->right);
                deleteNode ((*node)->left);

                (*node)->left = saveNodeLeftLeft;
            }
            if (((*node)->left->left != nullptr) && ((*node)->left->left->type == NUM_T) &&
                (compareFractionalNum((*node)->left->left->elem, 0) == 1))
            {
                node_t * saveNodeLeftRight = (*node)->left->right;

                deleteNode ((*node)->left->left);
                deleteNode ((*node)->left);

                (*node)->left = saveNodeLeftRight;
            }
        }
        else if (((*node)->right != nullptr) && ((*node)->right->op_t == OP_ADD))
        {
            if (((*node)->right->right != nullptr) && ((*node)->right->right->type == NUM_T) &&
                (compareFractionalNum((*node)->right->right->elem, 0) == 1))
            {
                node_t * saveNodeRightLeft = (*node)->right->left;

                deleteNode ((*node)->right->right);
                deleteNode ((*node)->right);

                (*node)->right = saveNodeRightLeft;
            }
            if (((*node)->right->left != nullptr) && ((*node)->right->left->type == NUM_T) &&
                (compareFractionalNum((*node)->right->left->elem, 0) == 1))
            {
                node_t * saveNodeRightRight = (*node)->right->right;

                deleteNode ((*node)->right->left);
                deleteNode ((*node)->right);

                (*node)->right = saveNodeRightRight;
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
    else
    {
        if ((node->left != nullptr) && (node->left->op_t == OP_MUL) &&
            (node->left->right->type == NUM_T) && (compareFractionalNum(node->left->right->elem, 0) == 1))
        {
            deleteNode (node->left->right);
            deleteNode (node->left);

            node_t * newNode = createNodeWithNum (0);
            node->left = newNode;
        }
        else if ((node->right != nullptr) && (node->right->op_t == OP_MUL) &&
                (node->right->right->type == NUM_T) && (compareFractionalNum(node->right->right->elem, 0) == 1))
        {
            deleteNode (node->right->right);
            deleteNode (node->right);

            node_t * newNode = createNodeWithNum (0);
            node->right = newNode;
        }
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
    if (abs(firstNum-secondNum) < 0.00001)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
