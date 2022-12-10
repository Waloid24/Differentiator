#include "support.h"

#define dumplineTree(text, ...)\
		fprintf (graphicDump, text, ##__VA_ARGS__)

//--------------------------------------------graphical tree dump-------------------------------------------------
static void dotFileHeaderForTree (const node_t * node, const char * nameDotFileTree);
static void writeNodeToDotFile (const node_t * node, FILE * graphicDump);
static void writeEdgeToDotFile (const node_t * node, FILE * graphicDump);
static void createDotFileTree (const char * nameDotFile, unsigned int timesCreatePicture);
static void createHtmlFileTree(const char * nameFileDump, unsigned int * timesCreatePicture);

static unsigned int	NUMBER_GRAPHICAL_TREE_DUMPS = 0;
//----------------------------------------------------------------------------------------------------------------

int checkInput (int * degreeOfNum)
{
    MY_ASSERT (degreeOfNum == nullptr, "There is no access to this number");

    int enterSymbols = scanf (" %d", degreeOfNum);
    while (getchar() != '\n');

    if (enterSymbols == 0 || *degreeOfNum < 0)
    {
        printf ("Please, enter a positive number\n");
        return checkInput (degreeOfNum);
    }

    return *degreeOfNum;
}

FILE * openFile (char * nameFile)
{
    FILE * texfile = fopen (nameFile, "w");
    MY_ASSERT (texfile == nullptr, "There is no access to logfile");
    setbuf (texfile, nullptr);

    return texfile;
}

//--------------------------------------------graphical tree dump-------------------------------------------------

void graphicDumpTree (const node_t * node)
{
	MY_ASSERT (node == nullptr, "There is no access to this node");

    const char * dotFileTree = "treeGraphviz.dot";
	const char * htmlFileTree = "treeGraphviz.html";

	dotFileHeaderForTree	(node, dotFileTree);
	createDotFileTree	(dotFileTree, NUMBER_GRAPHICAL_TREE_DUMPS);
	createHtmlFileTree	(htmlFileTree, &NUMBER_GRAPHICAL_TREE_DUMPS);
}

void dotFileHeaderForTree (const node_t * node, const char * nameDotFileTree)
{
	MY_ASSERT (node == nullptr, "There is no access to tree");

	FILE * graphicDump = fopen (nameDotFileTree, "w");
	MY_ASSERT (graphicDump == nullptr, "Unable to open the file for graphic dump");

	dumplineTree ("digraph G{\n");
	dumplineTree ("  graph [dpi = 100];\n\n");
	dumplineTree ("  ranksep = 1.5;\n\n");
	dumplineTree ("  splines = ortho;\n\n");

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
		dumplineTree ("\t node%p [label=\"{ %lf | %p }\"];\n", node, node->elem, node);
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
