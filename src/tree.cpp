#include "tree.h"
#include "graphDumpTree.h"
#include "myStrcmp.h"

//-------------------------------------------------------------building a tree-------------------------------------------------------

node_t * createNodeWithNum (elem_t num)
{
	node_t * node = (node_t *) calloc (1, sizeof(node_t));
	MY_ASSERT (node == nullptr, "Unable to allocate new memory");

	node->type = NUM_T;
	node->elem = num;

	return node;
}

node_t * createNodeWithOperation (enum operationType operation, node_t * valLeftNode, node_t * valRightNode)
{
	MY_ASSERT (valLeftNode == nullptr, "There is no access to the left node");
	MY_ASSERT (valRightNode == nullptr, "There is no access to the right node");

	node_t * node = (node_t *) calloc (1, sizeof(node_t));
	MY_ASSERT (node == nullptr, "Unable to allocate new memory");

	if (operation != OP_SUB &&
		operation != OP_ADD &&
		operation != OP_DIV &&
		operation != OP_MUL &&
		operation != OP_DEG)
	{
		MY_ASSERT (1, "Incorrect operation type specified");
	}

	node->left = valLeftNode;
	node->right = valRightNode;
	node->type = OPER_T;
	node->op_t = operation;

	valLeftNode->parent = node;
	valRightNode->parent = node;

	return node;
}

node_t * createNodeWithVariable (char variableName)
{
	node_t * node = (node_t *) calloc (1, sizeof(node_t));
	MY_ASSERT (node == nullptr, "Unable to allocate new memory");

	node->type = VAR_T;
	node->varName = variableName;

	return node;
}

node_t * createNodeWithFunction (const char * nameFunction, node_t * leftDescendant)
{
	if ((myStrcmp (nameFunction, "sin") != 0) && (myStrcmp (nameFunction, "cos") != 0) &&
		(myStrcmp (nameFunction, "tg") != 0) && (myStrcmp (nameFunction, "ctg") != 0) &&
		(myStrcmp (nameFunction, "ln") != 0))
	{
		MY_ASSERT (1, "Incorrect function name");
		return nullptr;
	}

	node_t * node = (node_t *) calloc (1, sizeof(node_t));
	MY_ASSERT (node == nullptr, "Unable to allocate new memory");

	node->type = FUNC_T;
	node->nameFunc = nameFunction;
	node->left = leftDescendant;
	leftDescendant->parent = node;
	return node;
}

node_t * copyNode (node_t * nodeForCopy)
{
	MY_ASSERT (nodeForCopy == nullptr, "There is no access to node");
	node_t * newNode = (node_t *) calloc (1, sizeof (node_t));
	MY_ASSERT (newNode == nullptr, "Unable to allocate new memory");
	*newNode = *nodeForCopy;

	if (nodeForCopy->left != nullptr)
	{
		node_t * leftNode = copyNode (nodeForCopy->left);
		newNode->left = leftNode;
		leftNode->parent = newNode;
	}

	if (nodeForCopy->right != nullptr)
	{
		node_t * rightNode = copyNode (nodeForCopy->right);
		newNode->right = rightNode;
		rightNode->parent = newNode;
	}

	return newNode;
}

void deleteTree (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this tree");

    if (node->left != nullptr)
    {
        deleteTree (node->left);
    }
    if (node->right != nullptr)
    {
        deleteTree (node->right);
    }

    free(node);
}

void deleteNode (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    free (node);
}
//----------------------------------------------------------------------------------------------------------------------------------

