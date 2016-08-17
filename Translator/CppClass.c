/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <string.h>
#include <stdlib.h>

#include "CppClass.h"
#include "MethodImplementation.h"

static void catLineVector(char *out, vector *v, const char *head);
static void freeMethods(vector *v);
static void freeVariableDeclarations(vector *v);
static vector *getMethodHeaderVector(vector *v);
static vector *getMethodImplementationVector(CppClass *h, vector *v);
static vector *getVariableHeaderVector(vector *v);
static int getVectorStrLen(vector *v);

void CppClass_init(CppClass *h, const char *className, const char *parent)
{
	h->name = strdup(className);
	if (parent)
		h->parent = strdup(parent);
	else
		h->parent = NULL;
	vector_init(&h->publicMethods);
	vector_init(&h->protectedMethods);
	vector_init(&h->privateMethods);
	
	vector_init(&h->publicVars);
	vector_init(&h->protectedVars);
	vector_init(&h->privateVars);
}

void CppClass_destroy(CppClass *h)
{
	if (h->name)
	{
		free(h->name);
		h->name = NULL;
	}
	if (h->parent)
	{
		free(h->parent);
		h->parent = NULL;
	}
	freeMethods(&h->publicMethods);
	freeMethods(&h->protectedMethods);
	freeMethods(&h->privateMethods);
	vector_free(&h->publicMethods);
	vector_free(&h->protectedMethods);
	vector_free(&h->privateMethods);

	freeVariableDeclarations(&h->publicVars);
	freeVariableDeclarations(&h->protectedVars);
	freeVariableDeclarations(&h->privateVars);	
	vector_free(&h->publicVars);
	vector_free(&h->protectedVars);
	vector_free(&h->privateVars);
}

static void freeMethods(vector *v)
{
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		MethodDeclaration *m = vector_get(v, i);
		MethodDeclaration_destroy(m);
		free(m);
	}
}

static void freeVariableDeclarations(vector *v)
{
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		VariableDeclaration *var = vector_get(v, i);
		VariableDeclaration_destroy(var);
		free(var);
	}
}

void CppClass_addPublicMethod(CppClass *h, MethodDeclaration *m)
{
	MethodDeclaration *copy = (MethodDeclaration *)malloc(sizeof(MethodDeclaration));
	MethodDeclaration_initWithCopy(copy, m);
	vector_add(&h->publicMethods, copy);
}

void CppClass_addProtectedMethod(CppClass *h, MethodDeclaration *m)
{
	MethodDeclaration *copy = (MethodDeclaration *)malloc(sizeof(MethodDeclaration));
	MethodDeclaration_initWithCopy(copy, m);
	vector_add(&h->protectedMethods, copy);
}

void CppClass_addPrivateMethod(CppClass *h, MethodDeclaration *m)
{
	MethodDeclaration *copy = (MethodDeclaration *)malloc(sizeof(MethodDeclaration));
	MethodDeclaration_initWithCopy(copy, m);
	vector_add(&h->privateMethods, copy);
}

void CppClass_addPublicVariable(CppClass *h, VariableDeclaration *v)
{
	VariableDeclaration *copy = (VariableDeclaration *)malloc(sizeof(VariableDeclaration));
	VariableDeclaration_initWithCopy(copy, v);
	vector_add(&h->publicVars, copy);
}

void CppClass_addProtectedVariable(CppClass *h, VariableDeclaration *v)
{
	VariableDeclaration *copy = (VariableDeclaration *)malloc(sizeof(VariableDeclaration));
	VariableDeclaration_initWithCopy(copy, v);
	vector_add(&h->protectedVars, copy);
}

void CppClass_addPrivateVariable(CppClass *h, VariableDeclaration *v)
{
	VariableDeclaration *copy = (VariableDeclaration *)malloc(sizeof(VariableDeclaration));
	VariableDeclaration_initWithCopy(copy, v);
	vector_add(&h->privateVars, copy);
}

char *CppClass_declarationString(CppClass *h)
{
	char *ret = NULL;
	int retLen = 0;
	int lines = 0;
	//printf("str1\n");
	vector *pubMetDec = getMethodHeaderVector(&h->publicMethods);
	vector *protMetDec = getMethodHeaderVector(&h->protectedMethods);
	vector *privMetDec = getMethodHeaderVector(&h->privateMethods);
	//printf("str2\n");
	vector *pubVarDec = getVariableHeaderVector(&h->publicVars);
	vector *protVarDec = getVariableHeaderVector(&h->protectedVars);
	vector *privVarDec = getVariableHeaderVector(&h->privateVars);
	//printf("str3\n");
	retLen += getVectorStrLen(pubMetDec); //for some \t\r\n
	retLen += getVectorStrLen(protMetDec);
	retLen += getVectorStrLen(privMetDec);
	//printf("str4 %p %p %p\n", pubVarDec, protVarDec, privVarDec);
	retLen += getVectorStrLen(pubVarDec);
	//printf("str4 %p %p %p\n", pubVarDec, protVarDec, privVarDec);
	
	retLen += getVectorStrLen(protVarDec);
	//printf("str4 %p %p %p\n", pubVarDec, protVarDec, vector_get(privVarDec, 0));
	
	retLen += getVectorStrLen(privVarDec);
	//printf("str5\n");
	lines += 
		vector_count(pubMetDec) +
		vector_count(protMetDec) +
		vector_count(privMetDec) +
		vector_count(pubVarDec) +
		vector_count(protVarDec) +
		vector_count(privVarDec);
		
	//printf("str6\n");
	retLen += 
		strlen("class ") +
		strlen(h->name) +
		strlen("\r\n{\r\n") +
		strlen("};\r\n") +
		strlen("\r\npublic:\r\n") + 
		strlen("\r\nprotected:\r\n") + 
		strlen("\r\nprivate:\r\n") +
		strlen("\t\r\n") * lines;
	if (h->parent)
		retLen += strlen(" : public ") + strlen(h->parent);
	ret = (char *)malloc(retLen + 1);
	//printf("str7\n");
	strcpy(ret, "class ");
	strcat(ret, h->name);
	if (h->parent)
	{
		strcat(ret, " : public ");
		strcat(ret, h->parent);
	}
	strcat(ret, "\r\n{\r\n");
	strcat(ret, "\r\npublic:\r\n");
	//printf("str8\n");
	catLineVector(ret, pubMetDec, "\t");
	catLineVector(ret, pubVarDec, "\t");
	strcat(ret, "\r\nprotected:\r\n");
	catLineVector(ret, protMetDec, "\t");
	catLineVector(ret, protVarDec, "\t");
	strcat(ret, "\r\nprivate:\r\n");
	catLineVector(ret, privMetDec, "\t");
	catLineVector(ret, privVarDec, "\t");
	//printf("str9\n");
	vector_deepFree(pubMetDec);
	vector_deepFree(pubVarDec);
	vector_deepFree(protMetDec);
	vector_deepFree(protVarDec);
	vector_deepFree(privMetDec);
	vector_deepFree(privVarDec);
	
	strcat(ret, "};\r\n");
	return ret;
}



char *CppClass_implementationString(CppClass *h)
{
	char *ret = NULL;
	int retLen = 0;
	int lines = 0;
	//printf("imp1\n");
	vector *pubMetImpl = getMethodImplementationVector(h, &h->publicMethods);
	vector *protMetImpl = getMethodImplementationVector(h, &h->protectedMethods);
	vector *privMetImpl = getMethodImplementationVector(h, &h->privateMethods);
	//printf("imp2\n");
	retLen += getVectorStrLen(pubMetImpl); //for \r\n
	retLen += getVectorStrLen(protMetImpl);
	retLen += getVectorStrLen(privMetImpl);
	//printf("imp3\n");
	lines += 
		vector_count(pubMetImpl) +
		vector_count(protMetImpl) +
		vector_count(privMetImpl);
	retLen += 2 * lines;
	
	ret = (char *)malloc(retLen + 1);
	ret[0] = 0;
	catLineVector(ret, pubMetImpl, NULL);
	catLineVector(ret, protMetImpl, NULL);
	catLineVector(ret, privMetImpl, NULL);
	vector_deepFree(pubMetImpl);
	vector_deepFree(protMetImpl);
	vector_deepFree(privMetImpl);
	//printf("imp5\n");
	
	return ret;
}

static void catLineVector(char *out, vector *v, const char *head)
{
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		//printf("IC: %s\n", vector_get(v, i));
		if (head)
			strcat(out, head);
		strcat(out, vector_get(v, i));
		strcat(out, "\r\n");
	}
}

static void freeVectorData(vector *v)
{
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
		free(vector_get(v, i));
	vector_free(v);
}

static vector *getMethodHeaderVector(vector *v)
{
	vector *ret = (vector *)malloc(sizeof(vector));
	vector_init(ret);
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		MethodDeclaration *md = (MethodDeclaration *)vector_get(v, i);
		vector_add(ret, MethodDeclaration_toString(md));
	}
	return ret;
}

static vector *getMethodImplementationVector(CppClass *h, vector *v)
{
	vector *ret = (vector *)malloc(sizeof(vector));
	vector_init(ret);
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		MethodDeclaration *md = (MethodDeclaration *)vector_get(v, i);
		vector_add(ret, MethodDeclaration_implementationString(md, h->name));
	}
	return ret;
}

static vector *getVariableHeaderVector(vector *v)
{
	vector *ret = (vector *)malloc(sizeof(vector));
	vector_init(ret);
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		VariableDeclaration *vd = (VariableDeclaration *)vector_get(v, i);
		//printf("sss :n=%s t=%s\n", vd->name, vd->type);
		char *str = VariableDeclaration_toString(vd);
		//printf("sss :%s\n", str);
		vector_add(ret, str);
	}
	return ret;
}

static int getVectorStrLen(vector *v)
{
	int len = 0;
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
		len += strlen(vector_get(v, i));
	return len;
}
