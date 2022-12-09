#include "dumpToTex.h"

static const char * TEX_OPTIONS =
"\\documentclass[12pt,a4paper]{article}\n"
"\\usepackage[utf8x]{inputenc}\n"
"\\usepackage{ucs}\n"
"\\usepackage{diagbox}\n"
"\\usepackage[english,russian]{babel}\n"
"\\usepackage[OT1]{fontenc}\n"
"\\usepackage{amsmath}\n"
"\\usepackage{amsfonts}\n"
"\\usepackage{amssymb}\n"
"\\usepackage{wasysym}\n"
"\\usepackage{wrapfig}\n"
"\\usepackage{physics}\n"
"\\usepackage[normalem]{ulem}\n"
"\\usepackage[left=2cm,right=2cm,top=2cm,bottom=2cm,includefoot,footskip=1.5cm]{geometry}\n"
"\\usepackage{mathrsfs}\n"
"\\usepackage{graphicx}\n"
"\\graphicspath{{pictures/}}\n"
"\\DeclareGraphicsExtensions{.png,.jpg,.jpeg}\n"
"\\usepackage{indentfirst}\n"
"\\usepackage[T2A]{fontenc}\n"
"\\usepackage{subfigure}\n"
"\\usepackage{fancyhdr}\n"
"\\pagestyle{fancy}\n"
"\\usepackage{caption}\n"
"\\captionsetup[table]{justification=}\n"
"\\usepackage{tabularx}\n"
"\\usepackage{floatrow}\n"
"\\usepackage{multicol}\n"
"\\usepackage[dvipsnames]{color}\n"
"\\usepackage{colortbl}\n\n";

static const char * TEX_TITLE_PAGE =
"\\begin{titlepage}\n"
    "\\begin{center}\n"
    "\\large Московский физико-технический институт\\\\ \n"
    "(национальный исследовательский университет)\\\\ \n"
    "\\includegraphics[scale=0.1]{mipt_image1.jpg}\\\\ \n"
    "\\vspace{5cm}\n"
    "\\Large Лабораторная работа № 1.3.1\\\\ \n"
    "\\vspace{2cm}\n"
    "\\textbf{\\Huge <<Дифференцируем в уме (почти)>>}\\\\ \n"
    "\\end{center}\n"

    "\\vspace{8cm}\n"
    "{\\raggedleft \\large Выполнили:\\\\ больные наголову студенты\\par\n"
    "}"
"\\end{titlepage}\n\n";

void texStart (FILE * texfile)
{
    dumpTexTree (TEX_OPTIONS);
    dumpTexTree ("\\begin{document}\n\n");
    dumpTexTree (TEX_TITLE_PAGE);
}

void startEquation (FILE * texfile, char var)
{
    dumpTexTree ("\nДавайте возьмём производную от этого чуда!\\\\\n");
    dumpTexTree ("\\begin{equation}\n");
    dumpTexTree ("f(%c) = ", var);
}

void startDifEquation (FILE * texfile, char var)
{
    dumpTexTree ("\nЭто не должно быть страшно! Посмотрим...\\\\\n");
    dumpTexTree ("\\begin{equation}\n");
    dumpTexTree ("f(%c)^\\prime_%c = ", var, var);
}

void endEquation (FILE * texfile)
{
    dumpTexTree ("\n\\end{equation}\n");
    dumpTexTree ("Как говорится в старой пословице: \"Меньше знаешь -- крепче спишь\"."
                    "Не задумывайтесь о том, что здесь произошло\\\\\n");
}

void texPrintNode (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");


    if (node->left != nullptr && node->right != nullptr)
    {
        if (node->op_t == OP_DIV)
        {
            dumpTexTree ("\\frac{");
        }
        if (node->op_t == OP_DEG && node->left->type == FUNC_T)
        {
            dumpTexTree ("(");
        }
        texPrintNode (texfile, node->left);
        if (node->op_t == OP_DEG && node->left->type == FUNC_T)
        {
            dumpTexTree (")");
        }
        if (node->op_t == OP_DIV)
        {
            dumpTexTree ("}");
        }
    }

    switch (node->type)
    {
        case OPER_T:
            texPrintOperation (texfile, node);
            break;
        case VAR_T:
            texPrintVar (texfile, node);
            break;
        case NUM_T:
            texPrintNum (texfile, node);
            break;
        case FUNC_T:
            texPrintFunc (texfile, node);
            dumpTexTree ("(");
            texPrintNode (texfile, node->left);
            dumpTexTree (")");
            break;
        case CONST_T:
            texPrintConst (texfile, node);
            break;
        default:
            fprintf (stderr, "Non-existent node type\n");
            break;
    }

    if (node->right != nullptr)
    {
        if (node->op_t == OP_DIV)
        {
            dumpTexTree ("{");
        }
        if ((node->op_t == OP_MUL || node->op_t == OP_DIV) &&
            (node->right->op_t == OP_ADD || node->right->op_t == OP_SUB))
        {
            dumpTexTree ("(");
        }
        texPrintNode (texfile, node->right);
        if (node->op_t == OP_DEG)
        {
            dumpTexTree ("}");
        }
        if ((node->op_t == OP_MUL || node->op_t == OP_DIV) &&
            (node->right->op_t == OP_ADD || node->right->op_t == OP_SUB))
        {
            dumpTexTree (")");
        }
        if (node->op_t == OP_DIV)
        {
            dumpTexTree ("}");
        }
    }
}

void texPrintOperation (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    switch (node->op_t)
    {
        case OP_ADD:
            dumpTexTree ("+");
            break;
        case OP_SUB:
            dumpTexTree (" %c ", OP_ADD);
            break;
        case OP_MUL:
            dumpTexTree (" \\cdot ");
            break;
        case OP_DIV:
            break;
        case OP_DEG:
            dumpTexTree (" %c{ ", OP_DEG);
            break;
        default:
            printf ("There is no such node\n");
            break;
    }
}

void texPrintVar (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpTexTree ("%c", node->varName);
}

void texPrintNum (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpTexTree ("%.2lf", node->elem);
}

void texPrintFunc (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpTexTree ("%s", node->nameFunc);
}

void texPrintConst (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpTexTree ("%c", node->varName);
}

int texFinish(FILE * texfile)
{
    dumpTexTree("\n\\end{document}\n");

    fclose(texfile);

    return 0;
}
