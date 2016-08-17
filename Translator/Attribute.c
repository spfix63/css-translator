/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "Attribute.h"


static void replaceWrongSymbols(Attribute *attr)
{
	char *t = attr->name;
	if (t)
		while (*t != '\0')
		{
			if (*t == '-')
				*t = '_';
			t++;
		}
	t = attr->value;
	if (t)
		while (*t != '\0')
		{
			if (*t == '"')
				*t = '\'';
			t++;
		}
}

void Attribute_init(Attribute *attr, const char *name, const char *value, const char *prepCode)
{
	attr->name = name ? strdup(name) : NULL;
	attr->value = value ? strdup(value) : NULL;
	attr->prep = prepCode ? strdup(prepCode) : NULL;
	replaceWrongSymbols(attr);
}


void Attribute_initWithCopy(Attribute *attr, Attribute *in)
{
	attr->name = in->name ? strdup(in->name) : NULL;
	attr->value = in->value ? strdup(in->value) : NULL;
	attr->prep = in->prep ? strdup(in->prep) : NULL;
}

void Attribute_destroy(Attribute *attr)
{
	if (attr->name)
	{
		free(attr->name);
		attr->name = NULL;
	}
	if (attr->value)
	{
		free(attr->value);
		attr->value = NULL; 
	}
	if (attr->prep)
	{
		free(attr->prep);
		attr->prep = NULL; 
	}
}

char *Attribute_toString(Attribute *attr)
{
	int len = Attribute_getLength(attr);
	if (len == 0)
		return NULL;
	char *ret = (char *)malloc(len + 1);
	ret[0] = 0;
	strcat(ret, attr->name);
	strcat(ret, " = \"");
	strcat(ret, attr->value);
	strcat(ret, "\";");
	
	return ret;
}

int Attribute_getLength(Attribute *attr)
{
	int len = 0;
	if (!attr->name || !attr->value)
		return 0;	
	len += strlen(attr->name);
	len += strlen(attr->value);
	len += strlen(" = \"\";");
	if (attr->prep)
		len += strlen(attr->prep);
	return len;
}


char *Attribute_toStringPrep(Attribute *attr)
{
	int len = Attribute_getLengthPrep(attr);
	if (len == 0)
		return 0;
	char *ret = (char *)malloc(len + 1);
	ret[0] = 0;
	if (attr->prep)
	{
		strcat(ret, attr->prep);
		strcat(ret, "\r\n");
	}
	return ret;
}

int Attribute_getLengthPrep(Attribute *attr)
{
	int len = 0;
	if (attr->prep)
	{
		len += strlen(attr->prep);
		len += strlen("\r\n");
	}
	return len;
}
