#include "tree.h"

static unsigned int	NUMBER_GRAPHICAL_TREE_DUMPS = 0;

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

	if (operation != OP_SUB && operation != OP_ADD && operation != OP_DIV && operation != OP_MUL && operation != OP_DEG)
	{
		printf ("Incorrect operation type specified\n");
		return nullptr;
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

node_t * createNodeWithConst (char variableName)
{
	node_t * node = (node_t *) calloc (1, sizeof(node_t));
	MY_ASSERT (node == nullptr, "Unable to allocate new memory");

	node->type = CONST_T;
	node->varName = variableName;

	return node;
}

node_t * createNodeWithFunction (char * nameFunction)
{
	if (myStrcmp ((const char *) nameFunction, "sin") == 0)
	{
		node_t * node = (node_t *) calloc (1, sizeof(node_t));
		MY_ASSERT (node == nullptr, "Unable to allocate new memory");

		node->type = FUNC_T;
		node->nameFunc = (char *) "sin";
		return node;
	}
	else if (myStrcmp ((const char *) nameFunction, "cos") == 0)
	{
		node_t * node = (node_t *) calloc (1, sizeof(node_t));
		MY_ASSERT (node == nullptr, "Unable to allocate new memory");

		node->type = FUNC_T;
		node->nameFunc = (char *) "cos";
		return node;
	}
	else if (myStrcmp ((const char *) nameFunction, "tg") == 0)
	{
		node_t * node = (node_t *) calloc (1, sizeof(node_t));
		MY_ASSERT (node == nullptr, "Unable to allocate new memory");

		node->type = FUNC_T;
		node->nameFunc = (char *) "tg";
		return node;
	}
	else if (myStrcmp ((const char *) nameFunction, "ctg") == 0)
	{
		node_t * node = (node_t *) calloc (1, sizeof(node_t));
		MY_ASSERT (node == nullptr, "Unable to allocate new memory");

		node->type = FUNC_T;
		node->nameFunc = (char *) "ctg";
		return node;
	}
	else if (myStrcmp ((const char *) nameFunction, "log") == 0 || myStrcmp ((const char *) nameFunction, "ln") == 0)
	{
		node_t * node = (node_t *) calloc (1, sizeof(node_t));
		MY_ASSERT (node == nullptr, "Unable to allocate new memory");

		node->type = FUNC_T;
		node->nameFunc = (char *) "ln";
		return node;
	}
	else
	{
		printf ("Incorrect function name\n");
		return nullptr;
	}
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
//----------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------graphical tree dump-------------------------------------------------

void graphicDumpTree (const node_t * node)
{
	MY_ASSERT (node == nullptr, "There is no access to this node");

    const char * dotFileTree = "treeGraphviz.dot";
	const char * htmlFileTree = "treeGraphviz.html";

	dotFileHeaderTree	(node, dotFileTree);
	createDotFileTree	(dotFileTree, NUMBER_GRAPHICAL_TREE_DUMPS);
	createHtmlFileTree	(htmlFileTree, &NUMBER_GRAPHICAL_TREE_DUMPS);
}

void dotFileHeaderTree (const node_t * node, const char * nameDotFileTree)
{
	MY_ASSERT (node == nullptr, "There is no access to tree");

	FILE * graphicDump = fopen (nameDotFileTree, "w");
	MY_ASSERT (graphicDump == nullptr, "Unable to open the file for graphic dump");

	dumplineTree ("digraph G{\n");
	dumplineTree ("  graph [dpi = 100];\n\n");
	dumplineTree ("  ranksep = 1.5;\n\n");
	dumplineTree ("  splines = ortho;\n\n");

	// dumplineTree ("  edge[minlen = 3, penwidth = 3];\n");
    dumplineTree ("  node[shape = record];\n\n");


	writeNodeToDotFile (node, graphicDump);
	writeEdgeToDotFile (node, graphicDump);


	dumplineTree ("}\n");

	fflush (graphicDump);
	fclose (graphicDump);
}

void writeNodeToDotFile (const node_t * node, FILE * graphicDump)
{
	MY_ASSERT (node == nullptr, "There is no access to the node");
	MY_ASSERT (graphicDump == nullptr, "There is no access to the file for dump");

	if (node->type == OPER_T)
	{
		dumplineTree ("\t node%p [label=\"{ %c | %p }\"];\n", node, node->op_t, node);
	}
	else if (node->type == VAR_T || node->type == CONST_T)
	{
		dumplineTree ("\t node%p [label=\"{ %c | %p }\"];\n", node, node->varName, node);
	}
	else if (node->type == NUM_T)
	{
		dumplineTree ("\t node%p [label=\"{ %lf | %p }\"];\n", node, node->elem, node); //| [style = filled, color = black, fillcolor = orange]
	}
	else
	{
		dumplineTree ("\t node%p [label=\"{ %s | %p }\"];\n", node, node->nameFunc, node);
	}

	if (node->left != nullptr)
	{
		writeNodeToDotFile (node->left, graphicDump);
	}
	if (node->right != nullptr)
	{
		writeNodeToDotFile (node->right, graphicDump);
	}
}

void writeEdgeToDotFile (const node_t * node, FILE * graphicDump)
{
	MY_ASSERT (node == nullptr, "There is no access to the node");
	MY_ASSERT (graphicDump == nullptr, "There is no access to the file for dump");

	if (node->left != nullptr)
	{
		dumplineTree ("\t node%p -> node%p;\n", node, node->left);
		writeEdgeToDotFile (node->left, graphicDump);
	}
	if (node->right != nullptr)
	{
		dumplineTree ("\t node%p -> node%p;\n", node, node->right);
		writeEdgeToDotFile (node->right, graphicDump);
	}
}

void createDotFileTree (const char * nameDotFile, unsigned int timesCreatePicture)
{
	MY_ASSERT (nameDotFile == nullptr, "There is no access to name file for graphic dump");

	char buf[100] = {};
    sprintf(buf, "dot -T png -o graph%u.png %s", timesCreatePicture, nameDotFile);
    system(buf);
}

static void createHtmlFileTree(const char * nameFileDump, unsigned int * timesCreatePicture)
{
	FILE * treeHTML = fopen (nameFileDump, "a");
	MY_ASSERT (treeHTML == nullptr, "Unable to open the file for html-dump");

	char namePicture[100] = {};
    sprintf (namePicture, "graph%u.png", *timesCreatePicture);

	printf ("namePicture = %s\n", namePicture);

	fprintf (treeHTML, "<img src=\"%s\" alt=\"dump №%u\">\n", namePicture, *timesCreatePicture);
	fprintf (treeHTML, "<hr>\n\n");

    (*timesCreatePicture)++;

    fflush (treeHTML);
    fclose (treeHTML);
}

//---------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------support functions---------------------------------------------------------
int myStrcmp (const char * string1, const char * string2)
{
	int i = 0, j = 0;
	for (; string1[i] != '\0' && string2[j] != '\0'; i++, j++)
	{
		while (!isalpha(string1[i]) && string1[i] != '\0')
			i++;
		while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;
		if (tolower(string1[i]) == tolower(string2[j]))
			continue;
		return (tolower(string1[i]) - tolower(string2[j]));
	}

	while (!isalpha(string1[i]) && string1[i] != '\0')
			i++;

	while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;

	return (tolower(string1[i]) - tolower(string2[j]));
}
//---------------------------------------------------------------------------------------------------------------------------------
