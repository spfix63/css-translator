/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CodeGen.h"
#include "CppClass.h"
#include "Node.h"
#include "Selector.h"
#include "vector.h"

static FILE *fhout = NULL;
static FILE *fcout = NULL;
static char *fnh = NULL;
static char *fnc = NULL;
static char *currentMediaBlock = NULL;


static char *headCode = NULL;
static int headSize = 1024;
static char *implCode = NULL;
static int implSize = 1024;
static int isAborted = 0;
static vector *classVector = NULL;

static vector *postProcessNodes = NULL;

static CppClass *getClassByName(const char *name);
static VariableDeclaration *getVariableByName(CppClass *h, const char *name);
static void handleSelectorGroup(vector *selg, vector *attributes);
static void postProcess();

int CG_init(const char *headerFn, const char *codeFn)
{
	fnh = strdup(headerFn);
	fnc = strdup(codeFn);
	postProcessNodes = (vector *)malloc(sizeof(vector));
	vector_init(postProcessNodes);
	classVector = (vector *)malloc(sizeof(vector));
	vector_init(classVector);
	headCode = (char *)calloc(1, headSize);
	implCode = (char *)calloc(1, implSize);
	
	CppClass *cl = (CppClass *)malloc(sizeof(CppClass));
	CppClass_init(cl, "Element", NULL);
	MethodDeclaration md;
	MethodDeclaration_init(&md, "applyCss", "void");
	MethodDeclaration_setVirtual(&md, 1);
	CppClass_addPublicMethod(cl, &md);	
	
	VariableDeclaration vd;
	VariableDeclaration_init(&vd, "elId", "std::string");
	VariableDeclaration vd1;
	VariableDeclaration_init(&vd1, "elClass", "std::string");
	VariableDeclaration vd2;
	VariableDeclaration_init(&vd2, "elPseudo", "std::string");
	VariableDeclaration vd3;
	VariableDeclaration_init(&vd3, "elMedia", "std::string");
	CppClass_addProtectedVariable(cl, &vd);	
	CppClass_addProtectedVariable(cl, &vd1);	
	CppClass_addProtectedVariable(cl, &vd2);	
	CppClass_addProtectedVariable(cl, &vd3);	
	
	
	vector_add(classVector, cl);
	
	return 0;
}

void CG_abort()
{
	isAborted = 1;
	remove(fnh);
	remove(fnc);
	CG_destroy();
}

void CG_destroy()
{
	if (currentMediaBlock)
	{
		free(currentMediaBlock);
		currentMediaBlock = NULL;
	}
	if (fhout)
	{
		fclose(fhout);
		fhout = NULL;
	}
	if (fcout)
	{
		fclose(fcout);
		fcout = NULL;
	}
	
	if (classVector)
	{
		//TODO free CppClass
		vector_free(classVector);
		free(classVector);
		classVector = NULL;
	}
	
	if (postProcessNodes)
	{
		//TODO free Node CppClass
		vector_free(postProcessNodes);
		free(postProcessNodes);
		postProcessNodes = NULL;
	}
	
	if (headCode)
	{
		free(headCode);
		headCode = NULL;
	}
	if (implCode)
	{
		free(implCode);
		implCode = NULL;
	}
}

void CG_setMediaBlock(const char *media)
{
	if (currentMediaBlock)
		free(currentMediaBlock);
	currentMediaBlock = media ? strdup(media) : NULL;
}

void CG_parseNodes(vector *nodes)
{
	int count = 0;
		
	for (count = 0; count < vector_count(nodes); count++)
	{	
		Node *node;
		int i;
		vector *v;
		
		node = (Node *)vector_get(nodes, count);
		Node_setMedia(node, currentMediaBlock);
		i = 0;
		v = &node->selectorGroup;
		for (i = 0; i < vector_count(v); i++)
		{
			int j = 0;
			vector *selg;
			selg = vector_get(v, i);
			//printf("handleSelectorGroup++ %p %p\n", selg, &node->attributes);
			handleSelectorGroup(selg, &node->attributes);
			//printf("handleSelectorGroup--\n");
		}
	}
}

void CG_addHeaderCode(const char *code)
{
	int size = strlen(headCode) + strlen(code) + 1;
	if (size > headSize)
	{
		headCode = realloc(headCode, size);
		headSize = size;
	}
	strcat(headCode, code);
}

void CG_addImplementationCode(const char *code)
{
	int size = strlen(implCode) + strlen(code) + 1;
	if (size > implSize)
	{
		implCode = realloc(implCode, size);
		implSize = size;
	}
	strcat(implCode, code);
}

int CG_flush()
{
	int i = 0;
	printf("Outputting data to %s and %s.\n", fnh, fnc);
	if (isAborted)
		return -1;
	postProcess();
	if (!fhout && !fcout)
	{
		fhout = fopen(fnh, "w");
		fcout = fopen(fnc, "w");
		if (!fhout || !fcout)
		{
			printf("fopen error.\n");
			fhout = NULL;
			fcout = NULL;
			return -1;
		}
	}
	
	fprintf(fhout, "#ifndef ELEMENT1233786128731263812_H\r\n");
	fprintf(fhout, "#define ELEMENT1233786128731263812_H\r\n");
	fprintf(fhout, "\r\n#include <string>\r\n");
	
	fprintf(fhout, "\r\n\r\n");
	fprintf(fcout, "#include \"%s\"", fnh);
	fprintf(fcout, "\r\n\r\n");
	
	for (i = 0; i < vector_count(classVector); i++)
	{
		//printf("decl str++\n");
		fprintf(fhout, CppClass_declarationString(vector_get(classVector, i)));
		fprintf(fhout, "\r\n\r\n");
		//printf("decl str--\n");
	}
	
	for (i = 0; i < vector_count(classVector); i++)
	{
		//printf("impl str++\n");
		fprintf(fcout, CppClass_implementationString(vector_get(classVector, i)));
		fprintf(fcout, "\r\n\r\n");
		//printf("impl str--\n");
	}
	fprintf(fhout, "#endif\r\n");
	return 0;
}

static void handleSelectorGroup(vector *selg, vector *attributes)
{
	int i;
	vector *vs = selg;
	
	Selector *mainSel = vector_get(vs, 0);
	//printf("mainSel: %p t=%d\n", mainSel, mainSel->type);
		
	if (mainSel->type == SEL_MAIN)
	{
		char *ifDec;
		int ifDecLen = 0;
		vector *va = attributes;
		CppClass *cl = getClassByName(mainSel->identifier);
		//printf("cl: p=%p\n", cl);
		if (!cl)
		{
			cl = (CppClass *)malloc(sizeof(CppClass));
			CppClass_init(cl, mainSel->identifier, "Element");
			MethodDeclaration md;
			MethodDeclaration_init(&md, "applyCss", "void");
			MethodDeclaration_setVirtual(&md, 1);
			CppClass_addPublicMethod(cl, &md);
			//printf("cl1: p=%p\n", cl);
			vector_add(classVector, cl);
			//printf("cl2: p=%p\n", cl);
		}
		//printf("cl: n=%s\n", cl->name);
		for (i = 0; i < vector_count(va); i++)
		{
			Attribute *attr = vector_get(va, i);
			if (!getVariableByName(cl, attr->name))
			{
				VariableDeclaration vd;
				VariableDeclaration_init(&vd, attr->name, "std::string");
				CppClass_addPrivateVariable(cl, &vd);
			}
			ifDecLen += Attribute_getLengthPrep(attr) + strlen("\t\t") + Attribute_getLength(attr) + strlen("\r\n");
			//ifDecLen += strlen("\t\t") + strlen(attr->name) + strlen(" = \"") + strlen(attr->value) + strlen("\";\r\n");
		}	
		
		ifDecLen += strlen("\tif()\r\n");
		ifDecLen += strlen("\t{\r\n\t}\r\n");
		
		if (currentMediaBlock)
		{
			ifDecLen += strlen("elMedia") + strlen(" == \"") + strlen(currentMediaBlock) + strlen("\"");
			if (vector_count(vs) != 0)
				ifDecLen += strlen(" && ");
		}
		for (i = 1; i < vector_count(vs); i++)
		{
			Selector *sel = vector_get(vs, i);
			if (sel->type == SEL_CLASS)
				ifDecLen += strlen("elClass") + strlen(" == \"") + strlen(sel->identifier) + strlen("\"");
			else if (sel->type == SEL_ID)
				ifDecLen += strlen("elId") + strlen(" == \"") + strlen(sel->identifier) + strlen("\"");
			else if (sel->type == SEL_PSEUDO)
				ifDecLen += strlen("elPseudo") + strlen(" == \"") + strlen(sel->identifier) + strlen("\"");
			else if (sel->type == SEL_ATTR)
			{
				if (sel->attr->value)
					ifDecLen += strlen(sel->attr->name) + strlen(" == \"") + strlen(sel->attr->value + strlen("\""));
				else
					ifDecLen += strlen(sel->attr->name);
			}
			if (i != vector_count(vs) - 1)
				ifDecLen += strlen(" && ");
		}
		if (i == 1)
		{
			ifDecLen += strlen("1");
		}
		//printf("ifdecLen: %d\n", ifDecLen);
		ifDec = (char *)malloc(ifDecLen + 1);
		strcpy(ifDec, "\tif(");
		if (currentMediaBlock)
		{
			strcat(ifDec, "elMedia == \"");
			strcat(ifDec, currentMediaBlock);
			strcat(ifDec, "\"");
			if (vector_count(vs) != 0)
				strcat(ifDec, " && ");
		}
		for (i = 1; i < vector_count(vs); i++)
		{
			Selector *sel = vector_get(vs, i);
			if (sel->type == SEL_MAIN)
			{
				continue;
			}
			if (i != 1)
				strcat(ifDec, " && ");
			if (sel->type == SEL_CLASS)
			{	
				strcat(ifDec, "elClass == \"");
				strcat(ifDec, sel->identifier);
				strcat(ifDec, "\"");
			}
			else if (sel->type == SEL_ID)
			{
				strcat(ifDec, "elId == \"");
				strcat(ifDec, sel->identifier);
				strcat(ifDec, "\"");
			}
			else if (sel->type == SEL_PSEUDO)
			{
				strcat(ifDec, "elPseudo == \"");
				strcat(ifDec, sel->identifier);
				strcat(ifDec, "\"");
			}
			else if (sel->type == SEL_ATTR)
			{
				if (sel->attr->value)
				{
					strcat(ifDec, sel->attr->name);
					strcat(ifDec, " == \"");
					strcat(ifDec, sel->attr->value);
					strcat(ifDec, "\"");
				}
				else
					strcat(ifDec, sel->attr->name);
			}
			
		}
		if (i == 1)
		{
			strcat(ifDec, "1");
		}
		strcat(ifDec, ")\r\n");
		strcat(ifDec, "\t{\r\n");
		
		for (i = 0; i < vector_count(va); i++)
		{
			Attribute *attr = vector_get(va, i);
			strcat(ifDec, Attribute_toStringPrep(attr));
			strcat(ifDec, "\t\t");
			strcat(ifDec, Attribute_toString(attr));
			strcat(ifDec, "\r\n");
		}
		
		strcat(ifDec, "\t}\r\n");
		
		MethodDeclaration_addCode(vector_get(&cl->publicMethods, 0), ifDec);
	}
	else
	{
		vector s;
		vector_init(&s);
		vector_add(&s, selg);
		Node *n = malloc(sizeof(Node));
		Node_initWithMedia(n, &s, attributes, currentMediaBlock);
		vector_add(postProcessNodes, n); 
	}
}

static void postProcess()
{
	int i;
	int j;
	int z;
	
	for (z = 0; z < vector_count(postProcessNodes); z++)
	{
		Node *currNode = (Node *)vector_get(postProcessNodes, z);
		vector *va = &currNode->attributes;
		vector *vs = vector_get(&currNode->selectorGroup, 0);
			
		for (j = 1; j < vector_count(classVector); j++)
		{
			char *ifDec;
			int ifDecLen = 0;
			
			CppClass *cl = vector_get(classVector, j);
			//printf("cl: n=%s\n", cl->name);
			for (i = 0; i < vector_count(va); i++)
			{
				Attribute *attr = vector_get(va, i);
				if (!getVariableByName(cl, attr->name))
				{
					VariableDeclaration vd;
					VariableDeclaration_init(&vd, attr->name, "std::string");
					CppClass_addPrivateVariable(cl, &vd);
				}
				ifDecLen += strlen("\t\t") + strlen(attr->name) + strlen(" = \"") + strlen(attr->value) + strlen("\";\r\n");
			}	
			
			ifDecLen += strlen("\tif()\r\n");
			ifDecLen += strlen("\t{\r\n\t}\r\n");
			if (currNode->media)
			{
				ifDecLen += strlen("elMedia") + strlen(" == \"") + strlen(currNode->media) + strlen("\"");
				if (vector_count(vs) != 0)
					ifDecLen += strlen(" && ");
			}
			for (i = 0; i < vector_count(vs); i++)
			{
				Selector *sel = vector_get(vs, i);
				if (sel->type == SEL_CLASS)
					ifDecLen += strlen("elClass") + strlen(" == \"") + strlen(sel->identifier) + strlen("\"");
				else if (sel->type == SEL_ID)
					ifDecLen += strlen("elId") + strlen(" == \"") + strlen(sel->identifier) + strlen("\"");
				else if (sel->type == SEL_PSEUDO)
					ifDecLen += strlen("elPseudo") + strlen(" == \"") + strlen(sel->identifier) + strlen("\"");
				else if (sel->type == SEL_ATTR)
				{
					if (sel->attr->value)
						ifDecLen += strlen(sel->attr->name) + strlen(" == \"") + strlen(sel->attr->value + strlen("\""));
					else
						ifDecLen += strlen(sel->attr->name);
				}
				if (i != vector_count(vs) - 1)
					ifDecLen += strlen(" && ");
			}
			if (i == 0)
			{
				ifDecLen += strlen("1");
			}
			//printf("ifdecLen: %d\n", ifDecLen);
			ifDec = (char *)malloc(ifDecLen + 1);
			strcpy(ifDec, "\tif(");
			if (currNode->media)
			{
				strcat(ifDec, "elMedia == \"");
				strcat(ifDec, currNode->media);
				strcat(ifDec, "\"");
				if (vector_count(vs) != 0)
					strcat(ifDec, " && ");
			}
			for (i = 0; i < vector_count(vs); i++)
			{
				Selector *sel = vector_get(vs, i);
				if (sel->type == SEL_MAIN)
					continue;
				if (i != 0)
					strcat(ifDec, " && ");
			
				if (sel->type == SEL_CLASS)
				{	
					strcat(ifDec, "elClass == \"");
					strcat(ifDec, sel->identifier);
					strcat(ifDec, "\"");
				}
				else if (sel->type == SEL_ID)
				{
					strcat(ifDec, "elId == \"");
					strcat(ifDec, sel->identifier);
					strcat(ifDec, "\"");
				}
				else if (sel->type == SEL_PSEUDO)
				{
					strcat(ifDec, "elPseudo == \"");
					strcat(ifDec, sel->identifier);
					strcat(ifDec, "\"");
				}
				else if (sel->type == SEL_ATTR)
				{
					if (sel->attr->value)
					{
						strcat(ifDec, sel->attr->name);
						strcat(ifDec, " == \"");
						strcat(ifDec, sel->attr->value);
						strcat(ifDec, "\"");
					}
					else
						strcat(ifDec, sel->attr->name);
				}
				if ((i != vector_count(vs) - 1) && (sel->type != SEL_MAIN))
					strcat(ifDec, " && ");
			}
			if (i == 0)
			{
				strcat(ifDec, "1");
			}
			strcat(ifDec, ")\r\n");
			strcat(ifDec, "\t{\r\n");
			
			for (i = 0; i < vector_count(va); i++)
			{
				Attribute *attr = vector_get(va, i);
				strcat(ifDec, "\t\t");
				strcat(ifDec, attr->name);
				strcat(ifDec, " = \"");
				strcat(ifDec, attr->value);
				strcat(ifDec, "\";\r\n");
			}
			
			strcat(ifDec, "\t}\r\n");
			
			MethodDeclaration_addCode(vector_get(&cl->publicMethods, 0), ifDec);
		}
	}
}

static CppClass *getClassByName(const char *name)
{
	int i;
	for (i = 0; i < vector_count(classVector); i++)
	{	
		if (!strcmp(name, ((CppClass *)vector_get(classVector, i))->name))
			return vector_get(classVector, i);
	}
	return NULL;
}

static VariableDeclaration *getVariableByName(CppClass *h, const char *name)
{
	int i;
	for (i = 0; i < vector_count(&h->publicVars); i++)
	{	
		if (!strcmp(name, ((VariableDeclaration *)vector_get(&h->publicVars, i))->name))
			return vector_get(&h->publicVars, i);
	}
	for (i = 0; i < vector_count(&h->protectedVars); i++)
	{	
		if (!strcmp(name, ((VariableDeclaration *)vector_get(&h->protectedVars, i))->name))
			return vector_get(&h->protectedVars, i);
	}
	for (i = 0; i < vector_count(&h->privateVars); i++)
	{	
		if (!strcmp(name, ((VariableDeclaration *)vector_get(&h->privateVars, i))->name))
			return vector_get(&h->privateVars, i);
	}
	return NULL;
}

