/**
  $Id: tinycfg.cpp 4679 2006-04-05 13:16:35Z majo $
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <tinyxml.h>

#include "tinycfg.h"

/**
	Open a configuration file with name (without the extension ".rc").
	If any not exists, system creates a new empty into the RAM.
	If you use switch APP_CFG, system automatically opens in
	the App constructor. Default config file is appname.rc.
	The pointer at this config saves to global variable
	cApp->cfg and its destructed (and saved) in App destructor.
	The maximal number of items into the config file is up to 512.
	The number of at once opens config files is unlimited. The maximal
	size of name of variable is up to 32 chars. The maximal size
	of saved text string or byte array is up to 127 chars + '\\0'.
*/
class OldConfig : public ConfigInterface
{
  class Node
  {
	enum itemtype
	{
	_DOUBLE='d',
	_INT='i',
	_STRING='s',
	_COMMENT='c',
	_HEXA='h',
	_BOOLEAN='b',
	_NONE=255
	};

	friend class OldConfig;
	int type;
	char item_name[36];

	union {
		int	i;
		char  s[132];
		double d;
		bool b;
	};
	int len;

	void  ModifyNode(char * comment)	// Modify comment
	{
		type = _COMMENT;
		*item_name =	0;
		strncpy(s, comment, 128);
	}
	void  ModifyNode(char * meno, char *	val) // Modify string
	{
		type = _STRING;
		strncpy(item_name, meno, 32);
		strncpy(s, val, 128);
	}
	void  ModifyNode(char * meno, int val) // Modify integer
	{
		type = _INT;
		strncpy(item_name, meno, 32);
		i	 = val;
	}
	void  ModifyNode(char * meno, bool val) // Modify boolean
	{
		type = _BOOLEAN;
		strncpy(item_name, meno, 32);
		b	 = val;
	}
	void  ModifyNode(char * meno, double	val) // Modify double
	{
		type = _DOUBLE;
		strncpy(item_name, meno, 32);
		d	 = val;
	}
	void  ModifyNode(char * meno, char *	val, int kolko) // Modify string
	{
		type = _HEXA;
		strncpy(item_name, meno, 32);
		len = kolko>128? 128 : kolko;
		memcpy(s, val, len);
	}
  };

		enum { MAX_CONF_ITEMS=512 };
		Node 	pole[MAX_CONF_ITEMS];
		char 	file_name[128];
		FILE 	*f;
		int		entries;
		int		change;
		int		writable;

		int	  find(char *	s);
		void WriteComment(char * s);
	protected:
		virtual bool Create(char* fname);
	public:
		OldConfig(const char*, bool write=true);
		OldConfig(bool write=true) { entries = 0 ; writable = write; change = 0; }
		virtual ~OldConfig();

		virtual bool Load(const char* fname);
		virtual bool Save(void);

		virtual bool ReadBool(char* name, bool &data, const char* domain = 0);
		virtual bool ReadInt(char* name, int &data, const char* domain = 0);
		virtual bool ReadString(char* name,	char *data, int max, const char* domain = 0);
		virtual bool ReadDouble(char* name, double &data, const char* domain = 0);
		virtual bool ReadHex(char* name, char *data, int length_of_item, const char* domain = 0);

		virtual bool ReadBoolArray(char* name, bool data[], int range, const char* domain = 0) {}
		virtual bool ReadIntArray(char* name, int data[], int range, const char* domain = 0) {}
		virtual bool ReadStringArray(char* name, char* data[], int max, int range, const char* domain = 0) {}
		virtual bool ReadDoubleArray(char* name, double data[], int range, const char* domain = 0) {}
		virtual bool ReadHexArray(char* name, char* data[], int length_of_item, int range, const char* domain = 0) {}

		virtual void WriteBool(char* name, bool data, const char* domain = 0);
		virtual void WriteInt(char* name, int data, const char* domain = 0);
		virtual void WriteString(char* name, char* data, const char* domain = 0);
		virtual void WriteDouble(char* name, double data, const char* domain = 0);
		virtual void WriteHex(char* name, char* data, int length_of_item, const char* domain = 0);

		virtual void WriteBoolArray(char* name, bool data[], int range, const char* domain = 0) {}
		virtual void WriteIntArray(char* name, int data[], int range, const char* domain = 0) {}
		virtual void WriteStringArray(char* name, char *data[], int range, const char* domain = 0) {}
		virtual void WriteDoubleArray(char* name, double data[], int range, const char* domain = 0) {}
		virtual void WriteHexArray(char* name, char *data[], int length_of_item, int range, const char* domain = 0) {}

		//! convert to XML file
		bool SaveAsXML(char* filename);
};

#if (ENABLE_XML_CONFIG == 1)

/**
	An implementation of config file format using XML.
*/
class XMLConfig : public ConfigInterface
{
		friend class OldConfig;
		static const int MAX_HEX_LEN = 512;
		static const int MAX_CHILD_DEPTH = 32;
		TiXmlDocument* document;
		TiXmlNode* rootnode;
		char filename[256];
		bool change;
		bool writable;

		void init(void);
		TiXmlNode* find_group(const char* domain);
		TiXmlElement* find(char* name, char* type, const char* domain);
		TiXmlElement* find_array(char* name, char* type, int& range, const char* domain);

		const char* extract_node_name(const char* ptr, char* name, int& len);
		void remove_if_exists(char* name, char* type, const char* domain);

		TiXmlNode* WriteGroup(TiXmlNode* node, char* name);
	protected:
		virtual bool Create(const char* fname);
	public:
		XMLConfig(bool write = true);
		virtual ~XMLConfig();

		virtual bool Load(const char* fname);
		virtual bool Save(void);

		virtual bool ReadBool(char* name, bool &data, const char* domain = 0);
		virtual bool ReadInt(char* name, int &data, const char* domain = 0);
		virtual bool ReadString(char* name,	char *data, int max, const char* domain = 0);
		virtual bool ReadDouble(char* name, double &data, const char* domain = 0);
		virtual bool ReadHex(char* name, char *data, int length_of_item, const char* domain = 0);

		virtual bool ReadBoolArray(char* name, bool data[], int range, const char* domain = 0);
		virtual bool ReadIntArray(char* name, int data[], int range, const char* domain = 0);
		virtual bool ReadStringArray(char* name, char* data[], int max, int range, const char* domain = 0);
		virtual bool ReadDoubleArray(char* name, double data[], int range, const char* domain = 0);
		virtual bool ReadHexArray(char* name, char* data[], int length_of_item, int range, const char* domain = 0);

		virtual void WriteBool(char* name, bool data, const char* domain = 0);
		virtual void WriteInt(char* name, int data, const char* domain = 0);
		virtual void WriteString(char* name, char* data, const char* domain = 0);
		virtual void WriteDouble(char* name, double data, const char* domain = 0);
		virtual void WriteHex(char* name, char* data, int length_of_item, const char* domain = 0);

		virtual void WriteBoolArray(char* name, bool data[], int range, const char* domain = 0);
		virtual void WriteIntArray(char* name, int data[], int range, const char* domain = 0);
		virtual void WriteStringArray(char* name, char *data[], int range, const char* domain = 0);
		virtual void WriteDoubleArray(char* name, double data[], int range, const char* domain = 0);
		virtual void WriteHexArray(char* name, char *data[], int length_of_item, int range, const char* domain = 0);

		bool ExistValid(char* fname);
};
#endif

//
// Configuration file handling	   (for fastgl)
//
int  OldConfig::find(char * s)
{
	for (int i=0;i<entries;i++)
	{
		if (!strcmp(pole[i].item_name,s)) return i;
	}
	return -1;
}

bool OldConfig::Create(char* fname)
{
	char s[100] = "; ";
	strcat(s, file_name);

	WriteComment(s);
	WriteComment(";<.......name of item..........> t <...body of item... (LF)");
	WriteComment("; where 'name' is string up to 32 chars length");
	WriteComment("; where 't' is one of 's'(tring), 'i(nt)', 'b(oolean)' or 'd'(ouble)");
	WriteComment("; Warning! don't use TABs! & STRING MUST be enclosed in double \"\"");
	WriteComment("|..............................| T |.............................");
	return true;
}

/**
	Open a '*.rc' config file with our formats.
	If the file doesn't exists, create an empty one.
	@param s filename
	@param write set to true if you want write to the file
*/
OldConfig::OldConfig(const char * s, bool write)
{
	strncpy(file_name, s, 120);
	change = 0;
	entries = 0;
	writable = write;
#ifdef __BASE_H
	if (writable && fgstate.verbose) ::printf("Opening config file '%s'\n", file_name);
#endif
	f = fopen(file_name,"r");
	if (f)
	{
		bool isnt_xml = Load(file_name);
		fclose(f);

		if (isnt_xml)
		{	// do conversion to a new form, and save an old to the '*.orig' file
			strcat(file_name, ".orig");
			change = true;
			Save();
			file_name[strlen(file_name)-5] = 0;
			SaveAsXML(file_name);
		}
	}
#if (ENABLE_XML_CONFIG == 0)
	else
	{
		Create(file_name);
	}
#endif
}

/**
	Write the file on the disc if it is writable.
*/
OldConfig::~OldConfig()
{
	if (writable) Sync();
}

/**
	Write the comment line to the config.
	If it already contains this comments, do nothing,
	else append the comment at the end of file.
*/
void   OldConfig::WriteComment(char* name)
{
	int i;
	if (entries>=MAX_CONF_ITEMS) return;
	if ((i=find(name))==-1) pole[entries++].ModifyNode(name);
	else  pole[i].ModifyNode(name);
	change = 1;
}

/**
	Write item of type 'int' with name.
	@param name the name of variable
	@param i value that will be writen
*/
void   OldConfig::WriteInt(char* name, int data, const char* )
{
	int ii;

	if (entries>=MAX_CONF_ITEMS) return;
	if ((ii=find(name))==-1)
		pole[entries++].ModifyNode(name, data);
	else
		pole[ii].ModifyNode(name, data);
	change = 1;
}

/**
	Write item of type 'int' with name.
	@param name the name of variable
	@param i value that will be writen
*/
void   OldConfig::WriteBool(char* name, bool i, const char* )
{
	int ii;
	if (entries>=MAX_CONF_ITEMS) return;
	if ((ii=find(name))==-1) pole[entries++].ModifyNode(name,i);
	else  pole[ii].ModifyNode(name,i);
	change = 1;
}

/**
	Write item of type 'char *' with name. This procedure
	write strings that have max. 127 chars length.
	@param name the name of variable
	@param i string that will be writen
*/
void   OldConfig::WriteString(char* name, char * i, const char* )
{
	int ii;
	if (strlen(i)>127) return;
	if (entries>=MAX_CONF_ITEMS) return;
	if ((ii=find(name))==-1) pole[entries++].ModifyNode(name,i);
	else  pole[ii].ModifyNode(name,i);
	change = 1;
}

/**
	Write item of type 'byte array' with the name. This procedure
	write an binary data in hex form that have max. 128 bytes length.
	@param name the name of variable
	@param i bytes that will be writen
	@param c number of bytes
*/
void   OldConfig::WriteHex(char* name, char * i, int c, const char* )
{
	int ii;
	if (c>128) return;
	if (entries>=MAX_CONF_ITEMS) return;
	if ((ii=find(name))==-1)
		pole[entries++].ModifyNode(name,i,c);
	else
		pole[ii].ModifyNode(name,i,c);
	change = 1;
}

/**
	Write item of type 'double' with name 'name'.
	@param name the name of variable
	@param i value that will be writen
*/
void   OldConfig::WriteDouble(char* name, double i, const char* )
{
	int ii;
	if (entries>=MAX_CONF_ITEMS) return;
	if ((ii=find(name))==-1) pole[entries++].ModifyNode(name,i);
	else  pole[ii].ModifyNode(name,i);
	change = 1;
}

static int ChToI(char xCH)
{
	if ((xCH >= '0') & (xCH <= '9'))
		return ((int) xCH) - '0';
	if ((xCH >= 'a') & (xCH <= 'f'))
		return ((int) xCH) - 'a' + 10;
	if ((xCH >= 'A') & (xCH <= 'F'))
		return ((int) xCH) - 'A' + 10;
	return -1;
}

static int hexdata(const char *name)
{
	int a = ChToI(name[0]);
	int b = ChToI(name[1]);
	if (a!=-1 && b!=-1) return a*16 + b;
	return 256;
}

bool OldConfig::Load(const char*)
{
	char name[360], *p, n[33], *t, hex[128];
	int typ,i;
	double d;
	bool b;

	for(;entries<MAX_CONF_ITEMS;)
	{
		*name = 0;
		fgets(name,359,f);
		if (!*name) break;

		// is it a new XML form?
		if (strncmp(name, "<?xml", 5) == 0)
			return false;

		*(name+strlen(name)-1) = 0;
		p = name;
		t = n;
		if (!isalpha(name[0]) && *name != '_')
		{
			pole[entries].ModifyNode(name);
		}
		else
		{
			while(isalnum(*p) || *p=='_') (*t++) = (*p++);
			*t = 0;
			switch(typ = name[33])
			{
				case Node::_DOUBLE:
					d = atof(name+35);
					pole[entries].ModifyNode(n,d);
					break;
				case Node::_INT:
					i = atoi(name+35);
					pole[entries].ModifyNode(n,i);
					break;
				case Node::_BOOLEAN:
					b = !!strcmp(name+35, "no");
					pole[entries].ModifyNode(n,b);
					break;
				case Node::_STRING:
					t = name+36;
					while(*t!='"') t++;
					*t = 0;
					pole[entries].ModifyNode(n,name+36);
					break;
				case Node::_HEXA:
					t = name+36;
					i = 0;
					while((typ=hexdata(t))<256 && i<128)
					{
						hex[i++] = typ;
						t+=2;
					}
					pole[entries].ModifyNode(n,hex,i);
					break;
			}
		}
		entries++;
	}
//	SaveAsXML(name);
// 	unlink(name);
	return true;
}

/**
	Saves the config object to its filename.
*/
bool OldConfig::Save(void)
{
	char s[360], hex[260],pom[8];
	int j;

	if (change==0) return false;
#ifdef __BASE_H
	if (fgstate.verbose) ::
	printf("Writing config file '%s'\n", file_name);
#endif
	errno = 0;
	f = fopen(file_name,"w");
	if (f)
	{
		for(int i=0;i<entries;i++)
		{
			switch(pole[i].type)
			{
				case Node::_COMMENT:
					fputs(pole[i].s,f);
					break;
				case Node::_STRING:
					sprintf(s, "%-32s %c \"%s\"", pole[i].item_name, pole[i].type, pole[i].s);
					fputs(s,f);
					break;
				case Node::_INT:
					sprintf(s, "%-32s %c %d", pole[i].item_name, pole[i].type, pole[i].i);
					fputs(s,f);
					break;
				case Node::_BOOLEAN:
					sprintf(s, "%-32s %c %s", pole[i].item_name, pole[i].type, pole[i].b?"yes":"no");
					fputs(s,f);
					break;
				case Node::_DOUBLE:
					sprintf(s, "%-32s %c %f", pole[i].item_name, pole[i].type, pole[i].d);
					fputs(s,f);
					break;
				case Node::_HEXA:
					*hex = 0;
					for(j=0;j<pole[i].len;j++)
					{
						sprintf(pom, "%02x", pole[i].s[j]&255);
						strcat(hex,pom);
					}
					sprintf(s, "%-32s %c \"%s\"", pole[i].item_name, pole[i].type, hex);
					fputs(s,f);
					break;
			}
			fputc('\n',f);
		}
		fclose(f);
		change = false;
//		SaveAsXML(name);
		return true;
	}
	if (errno) perror("OldConfig::Sync() ");
	return false;
}

/**
	Read the variable value of type 'int' from the config.
	@param name name of the variable (max. size is 32 chars)
	@param p the reference to the variable that will be initialized wit reads value.
	@return true if value is assigned to variable, false when variable is not found in the config.
*/
bool OldConfig::ReadInt(char* name, int &p, const char* )
{
	int	n =	find(name);
	if (n>=0) return (p = pole[n].i), 1;
	return 0;
}

/**
	Read the variable value of type 'bool' from the config.
	@param name name of the variable (max. size is 32 chars)
	@param p the reference to the variable that will be initialized wit reads value.
	@return true if value is assigned to variable, false when variable is not found in the config.
*/
bool OldConfig::ReadBool(char	* name, bool &p, const char* )
{
	int	n =	find(name);
	if (n>=0) return (p = pole[n].b), 1;
	return 0;
}

/**
	Read the null terminated string from the config.
	@param name name of the variable (max. size is 32 chars)
	@param p the pointer to the variable that will be initialized with reads value. You
	must allocate this memory before call.
	@param max the max. number of bytes that will be reads (max 127 chars).
	@return true if value is assigned to variable, false when variable is not found in the config.
*/
bool OldConfig::ReadString(char* name,	char *p, int max, const char* )
{
	int	n =	find(name);
	if (max>128) max = 128;
	if (n>=0)
	{
		strncpy(p,pole[n].s,max);
		return 1;
	}
	return 0;
}

/**
	Read the variable value of type 'double' from the config.
	@param name name of the variable (max. size is 32 chars)
	@param p the reference to the variable that will be initialized with reads value.
	@return true if value is assigned to variable, false when variable is not found in the config.
*/
bool OldConfig::ReadDouble(char* name, double &p, const char* )
{
	int	n =	find(name);
	if (n>=0) return (p = pole[n].d), 1;
	return 0;
}

/**
	Read the array of bytes from the config.
	@param name name of the variable (max. size is 32 chars)
	@param p the pointer to the variable that will be initialized with reads value.
	@param max the max. number of bytes that will be reads (max 128 bytes).
	@return true if value is assigned to variable, false when variable is not found in the config.
*/
bool OldConfig::ReadHex(char* name, char *p, int max, const char* )
{
	int	n =	find(name);
	if (max>128) max = 128;
	if (n>=0)
	{
		memcpy(p, pole[n].s, max);
		return 1;
	}
	return 0;
}

/**
	Saves 'xxx.rc' as 'xxx.xml' using XML file format.
*/
bool OldConfig::SaveAsXML(char* filename)
{
#if (ENABLE_XML_CONFIG == 1)
	char tmpname[256], *ptr;

	strcpy(tmpname, filename);
/*
	ptr = strstr(tmpname, ".xml");

	if (ptr == 0)
		sprintf(tmpname, "%s.xml", filename);
*/
	XMLConfig tmpcfg;
	tmpcfg.Create(tmpname);
	tmpcfg.Load(tmpname);
	for(int i=0;i<entries;i++)
	{
		switch(pole[i].type)
		{
//			case Node::_COMMENT:
//				tmpcfg.WriteComment(pole[i].name);
//				break;
			case Node::_STRING:
				tmpcfg.WriteString(pole[i].item_name, pole[i].s);
				break;
			case Node::_INT:
				tmpcfg.WriteInt(pole[i].item_name, pole[i].i);
				break;
			case Node::_DOUBLE:
				tmpcfg.WriteDouble(pole[i].item_name, pole[i].d);
				break;
			case Node::_BOOLEAN:
				tmpcfg.WriteBool(pole[i].item_name, pole[i].b);
				break;
			case Node::_HEXA:
				tmpcfg.WriteHex(pole[i].item_name, pole[i].s, pole[i].len);
				break;
		}
	}
	return tmpcfg.Save();
#else
	return true;
#endif
}

// -----------------------------------------------------------------------------

#if (ENABLE_XML_CONFIG == 1)

#define STRING_STRING			"string"
#define INT_STRING				"int"
#define DOUBLE_STRING  			"double"
#define BOOL_STRING				"bool"
#define HEX_STRING				"hex"

#define ARRAY_STRING			"array"
#define ITEM_STRING				"item"
#define GROUP_STRING	 		"group"
#define VALUE_STRING	 		"val"
#define NAME_STRING	 			"name"
#define TYPE_STRING	 			"type"
#define LEN_STRING	 			"len"
#define RANGE_STRING   			"range"

XMLConfig::XMLConfig(bool write)
{
	init();
	writable = write;
}

void XMLConfig::init(void)
{
	document = 0;
	rootnode = 0;
	writable = false;
	filename[0] = 0;
	change = true;
}

XMLConfig::~XMLConfig()
{
	if (writable) Save();
	delete document;
	document = 0;
	init();
}

/**
	Creates & saves an empty XML file in OpenGUI INI format.
*/
bool XMLConfig::Create(const char* fname)
{
	strncpy(filename, fname, sizeof(filename)-1);

	if (ExistValid(filename)) return true;

	// Vytvor nejaky prazdny XML subor
	const char* demoStart =
		"<?xml version=\"1.0\" standalone='yes' >\n"
		"<tinycfg>\n"
		"</tinycfg>";

	TiXmlDocument tmpdoc( filename );
	tmpdoc.Parse( demoStart );
	change = true;
	if ( tmpdoc.Error() )
	{
//		printf( "Error in %s: %s\n", tmpdoc.Value(), tmpdoc.ErrorDesc() );
		return false;
	}
	return tmpdoc.SaveFile();
}

bool XMLConfig::ExistValid(char* fname)
{
	FILE* fp = fopen(fname, "r");
	bool ret = false;

	if (fp)
	{   char gps[80];
		fgets(gps,79,fp);
		ret = (strcmp(gps, "<?xml version=\"1.0\"  standalone='yes' >\n") == 0);
		fclose(fp);
	}
	return ret;
}

bool XMLConfig::Load(const char* fname)
{
	// save previous if any
	TiXmlDocument* old = document;
	TiXmlDocument* doc = new TiXmlDocument( fname );
	bool loadOkay = doc->LoadFile();

	if ( !loadOkay || doc->RootElement() == 0)
	{
//		printf( "Could not load test file '%s'. Error='%s'. Exiting.\n", fname, doc->ErrorDesc() );
		bool isnew = Create(fname);
		if (isnew == false)
		{
			document = old;	// comeback
			delete doc;
			return false;
		}
		loadOkay = doc->LoadFile();
		if ( !loadOkay )
		{
			delete doc;
			return false;
		}
	}

	if (old) delete old;

	document = doc;
	rootnode = document->RootElement();
	strncpy(filename, fname, sizeof(filename)-1);
	change = true;
	return !!rootnode;
}

/**
	Saves the document if any.
	@return true if OK
*/
bool XMLConfig::Save(void)
{
	if (writable == true && change == true && document)
	{
		change = false;
		return document->SaveFile();
	}
	return false;
}

TiXmlElement* XMLConfig::find(char* name, char* type, const char* domain)
{
	TiXmlElement* element = 0;
	const char* tmp_name;
	const char* tmp_value;

	TiXmlNode* node = find_group(domain);
	if (node == 0) return 0;

	for(element = node->FirstChildElement(type); element; element = element->NextSiblingElement(type))
	{
		tmp_name = element->Attribute(NAME_STRING);
		tmp_value = element->Attribute(VALUE_STRING);

		if (tmp_name && tmp_value)
		{
			if ( strcmp(tmp_name, name) == 0)
			{
				return element;
			}
		}
	}
	return 0;
}

TiXmlElement* XMLConfig::find_array(char* name, char* type, int& range, const char* domain)
{
	TiXmlElement* element = 0;
	const char* tmp_name;
	const char* tmp_type;

	TiXmlNode* node = find_group(domain);
	if (node == 0) return 0;

	for(element = node->FirstChildElement(type); element; element = element->NextSiblingElement(type))
	{
		tmp_name = element->Attribute(NAME_STRING);
		range = 0;
		element->QueryIntAttribute(RANGE_STRING, &range);

		if (tmp_name)
		{
			if ( strcmp(tmp_name, name) == 0 )
			{
				// an empty array
				if (range == 0)
					return 0;

				return element->FirstChildElement();
			}
		}
	}
	return 0;
}

/**
	@return the node with group of this name (eg. 'myapp.somedata.bank0')
*/
TiXmlNode* XMLConfig::find_group(const char* domain)
{
	if (domain == 0) return rootnode;

	assert(rootnode);
	
	TiXmlHandle docHandle( rootnode );
	TiXmlHandle childHandle(docHandle);
	char node_name[64];
	const char* tmp = domain;
	int len;

	for(int depth = 0; depth < MAX_CHILD_DEPTH; depth++)
	{
		node_name[0] = 0;
		tmp = extract_node_name(tmp, node_name, len);

		if (tmp == 0 || len < 1)
		{
			printf("Bad domain name '%s' for config variable!\a\n", domain);
			return rootnode;		// bad name character
		}

		childHandle = docHandle.FirstChild(node_name);

		if (childHandle.Node() == 0)
		{
			TiXmlNode* newnode = WriteGroup(docHandle.Node(), node_name);
			childHandle = docHandle.FirstChild(node_name);
		}

		docHandle = childHandle;

		if (*tmp == 0)
			return docHandle.Node();    // the end of domain

	}
	return rootnode;
}

TiXmlNode* XMLConfig::WriteGroup(TiXmlNode* node, char* name)
{
	TiXmlElement item( name );
	TiXmlNode* where = node->InsertEndChild( item );
	return where;
}

const char* XMLConfig::extract_node_name(const char* ptr, char* name, int& len)
{
	int maxi = 63;
	int c;
	len = 0;

	while( (c = *ptr) != 0)
	{
		if ( isalnum(c) || c == '_')
		{
			*name++ = c;
			*name = 0;
			maxi--;
			if (maxi == 0) return 0;
			ptr++;
			len++;
			continue;
		}
		else if (c == '.')
			return ++ptr;
		break;
	}
	return ptr;
}

void XMLConfig::remove_if_exists(char* name, char* type, const char* domain)
{
	int range = 0;
	TiXmlElement*ele = find_array(name, type, range, domain);
	if (ele == 0) return;

	TiXmlNode* orig = ele->Parent();
	while(ele)
	{
		ele->Parent()->RemoveChild(ele);
		ele = find_array(name, type, range, domain);
	}
	orig->Parent()->RemoveChild(orig);
	return;
}

// -------------------------------------------------------------------------

bool XMLConfig::ReadBool(char* name, bool &value, const char* domain)
{
	TiXmlElement* text = find(name, BOOL_STRING, domain);

	// not found!
	if (text == 0) return false;
	value = !!strcmp(text->Attribute(VALUE_STRING), "false");
	return true;
}

bool XMLConfig::ReadInt(char* name, int &value, const char* domain)
{
	TiXmlElement* text = find(name, INT_STRING, domain);

	// not found!
	if (text == 0)
		return false;
	return text->QueryIntAttribute(VALUE_STRING, &value) == TIXML_SUCCESS;
}

bool XMLConfig::ReadString(char* name,	char *value, int max, const char* domain)
{
	TiXmlElement* text = find(name, STRING_STRING, domain);

	// not found!
	if (text == 0)
		return false;
	strncpy(value, text->Attribute(VALUE_STRING), max);
	return true;
}

bool XMLConfig::ReadDouble(char* name, double &value, const char* domain)
{
	TiXmlElement* text = find(name, DOUBLE_STRING, domain);

	// not found!
	if (text == 0)
		return false;
	return text->QueryDoubleAttribute(VALUE_STRING, &value) == TIXML_SUCCESS;
}

bool XMLConfig::ReadHex(char* name, char *data, int len, const char* domain)
{
	assert(len <= MAX_HEX_LEN);

	TiXmlElement* text = find(name, HEX_STRING, domain);

	// not found!
	if (text == 0) return false;
	const char* hexstr = text->Attribute(VALUE_STRING);

	int i = 0, val;
	while(hexstr && hexstr[i*2] && ( val = hexdata(hexstr+i*2)) < 256 && i<len )
	{
		data[i++] = val;
	}
	return true;
}

bool XMLConfig::ReadBoolArray(char* name, bool data[], int range, const char* domain)
{
	int in_file_range = 0;
	TiXmlElement* items = find_array(name, BOOL_STRING, in_file_range, domain);

	if (items == 0) return false;

	for (int i = 0; i < range && items; i++)
	{
		data[i] = false;    // initialize array if no value
		const char* value = items->Attribute(VALUE_STRING);
		if (value)
			data[i] = !!strcmp(value, "false");
		items = items->NextSiblingElement(ITEM_STRING);
	}
	return true;
}

bool XMLConfig::ReadIntArray(char* name, int data[], int range, const char* domain)
{
	int in_file_range = 0;
	TiXmlElement* items = find_array(name, INT_STRING, in_file_range, domain);

	if (items == 0) return false;

	for (int i = 0; i < range && items; i++)
	{
		data[i] = 0;    // initialize array if no value
		items->QueryIntAttribute(VALUE_STRING, &data[i]);
		items = items->NextSiblingElement(ITEM_STRING);
	}
	return true;
}

bool XMLConfig::ReadStringArray(char* name, char *data[], int max, int range, const char* domain)
{
	int in_file_range = 0;
	TiXmlElement* items = find_array(name, STRING_STRING, in_file_range, domain);

	if (items == 0) return false;

	for (int i = 0; i < range && items; i++)
	{
		data[i][0] = 0;    // initialize array if no value
		const char* value = items->Attribute(VALUE_STRING);
		if (value)
			strncpy(&data[i][0], value, max);
		items = items->NextSiblingElement(ITEM_STRING);
	}
	return true;
}

bool XMLConfig::ReadDoubleArray(char* name, double data[], int range, const char* domain)
{
	int in_file_range = 0;
	TiXmlElement* items = find_array(name, DOUBLE_STRING, in_file_range, domain);

	if (items == 0) return false;

	for (int i = 0; i < range && items; i++)
	{
		data[i] = 0.0;    // initialize array if no value
		items->QueryDoubleAttribute(VALUE_STRING, &data[i]);
		items = items->NextSiblingElement(ITEM_STRING);
	}
	return true;
}

bool XMLConfig::ReadHexArray(char* name, char *data[], int length_of_item, int range, const char* domain)
{
	int in_file_range = 0;
	char hex[MAX_HEX_LEN * 2 + 1];
	TiXmlElement* items = find_array(name, HEX_STRING, in_file_range, domain);

	if (items == 0) return false;

	assert(length_of_item <= MAX_HEX_LEN);

	for (int i = 0; i < range && items; i++)
	{
		memset(&data[i][0], 0, length_of_item);    // initialize array if no value

		int j = 0, val;
		const char* value = items->Attribute(VALUE_STRING);
		if (value)
			while( value[j*2] && ( val = hexdata(value+j*2)) < 256 && j<length_of_item )
		{
			data[i][j++] = val;
		}
		items = items->NextSiblingElement(ITEM_STRING);
	}
	return true;
}

void XMLConfig::WriteString(char * name, char * value, const char* domain)
{
	TiXmlElement* found_item = find(name, STRING_STRING, domain);

	if (found_item)
	{
		found_item->SetAttribute( VALUE_STRING, value );
	}
	else
	{
		TiXmlElement* item = new TiXmlElement( STRING_STRING );
		item->SetAttribute( NAME_STRING, name );
		item->SetAttribute( VALUE_STRING, value );
		TiXmlNode* node = find_group(domain);
		node->InsertEndChild( *item );
		delete item;
		item = 0;
	}
	change = true;
}

void XMLConfig::WriteInt(char * name, int value, const char* domain)
{
	char str[64];
	TiXmlElement* text = find(name, INT_STRING, domain);

	if (text)
	{
		text->SetAttribute( VALUE_STRING, value );
	}
	else
	{
		TiXmlElement* item = new TiXmlElement( INT_STRING );
		item->SetAttribute( NAME_STRING, name );
		item->SetAttribute( VALUE_STRING, value );
		TiXmlNode* node = find_group(domain);
		node->InsertEndChild( *item );
		delete item;
		item = 0;
	}
	change = true;
}

void XMLConfig::WriteDouble(char* name, double value, const char* domain)
{
	char str[64];
	TiXmlElement* text = find(name, DOUBLE_STRING, domain);

	if (text)
	{
		text->SetDoubleAttribute( VALUE_STRING, value );
	}
	else
	{
		TiXmlElement* item = new TiXmlElement( DOUBLE_STRING );

		item->SetAttribute( NAME_STRING, name );
		item->SetDoubleAttribute( VALUE_STRING, value );
		TiXmlNode* node = find_group(domain);
		node->InsertEndChild( *item );
		delete item;
		item = 0;
	}
	change = true;
}

void XMLConfig::WriteBool(char * name, bool value, const char* domain)
{
	TiXmlElement* text = find(name, BOOL_STRING, domain);

	if (text) // if exist
	{
		text->SetAttribute(VALUE_STRING, value ? "true" : "false" );
	}
	else      // if not exist
	{
		TiXmlElement* item = new TiXmlElement( BOOL_STRING );

		item->SetAttribute( NAME_STRING, name );
		item->SetAttribute(VALUE_STRING, value ? "true" : "false" );

		TiXmlNode* node = find_group(domain);
		node->InsertEndChild( *item );
		delete item;
		item = 0;
	}
	change = true;
}

void XMLConfig::WriteHex(char * name, char *value, int len, const char* domain)
{
	assert(len <= MAX_HEX_LEN);

	char hex[MAX_HEX_LEN * 2 + 1];
	*hex = 0;
	for(int j=0;j<len;j++)
	{
		sprintf(hex+j*2, "%02x", value[j]&255);
		hex[j*2+2] = 0;
	}

	TiXmlElement* text = find(name, HEX_STRING, domain);

	if (text)
	{
		text->SetAttribute(VALUE_STRING, hex);
	}
	else
	{
		TiXmlElement* item = new TiXmlElement( HEX_STRING );
		item->SetAttribute( NAME_STRING, name );
		item->SetAttribute( VALUE_STRING, hex);
		item->SetAttribute( LEN_STRING, len);
		TiXmlNode* node = find_group(domain);
		node->InsertEndChild( *item );
		delete item;
		item = 0;
	}
	change = true;
}

void XMLConfig::WriteBoolArray(char * name, bool data[], int range, const char* domain)
{
	TiXmlElement* item = new TiXmlElement( BOOL_STRING );

	remove_if_exists(name, BOOL_STRING, domain);

	item->SetAttribute( NAME_STRING, name );
	item->SetAttribute( RANGE_STRING, range );

	for( int i = 0; i < range; i++)
	{
		TiXmlElement array_item( ITEM_STRING );
		array_item.SetAttribute( VALUE_STRING, data[i] ? "true" : "false" );
		item->InsertEndChild( array_item );
	}

	TiXmlNode* node = find_group(domain);
	node->InsertEndChild( *item );
	delete item;
	item = 0;
	change = true;
}

void XMLConfig::WriteIntArray(char* name, int data[], int range, const char* domain)
{
	TiXmlElement* item = new TiXmlElement( INT_STRING );

	remove_if_exists(name, INT_STRING, domain);

	item->SetAttribute( NAME_STRING, name );
	item->SetAttribute( RANGE_STRING, range );

	for( int i = 0; i < range; i++)
	{
		TiXmlElement array_item( ITEM_STRING );
		array_item.SetAttribute( VALUE_STRING, data[i]);
		item->InsertEndChild( array_item );
	}
	TiXmlNode* node = find_group(domain);
	node->InsertEndChild( *item );
	delete item;
	item = 0;
	change = true;
}

void XMLConfig::WriteStringArray(char* name, char* data[], int range, const char* domain)
{
	remove_if_exists(name, STRING_STRING, domain);

	TiXmlElement* item = new TiXmlElement( STRING_STRING );
	item->SetAttribute( NAME_STRING, name );
	item->SetAttribute( RANGE_STRING, range );

	for( int i = 0; i < range; i++)
	{
		TiXmlElement array_item( ITEM_STRING );
		array_item.SetAttribute( VALUE_STRING, data[i]);
		item->InsertEndChild( array_item );
	}

	TiXmlNode* node = find_group(domain);
	node->InsertEndChild( *item );
	delete item;
	item = 0;
	change = true;
}

void XMLConfig::WriteDoubleArray(char* name, double data[], int range, const char* domain)
{
	remove_if_exists(name, DOUBLE_STRING, domain);

	TiXmlElement* item = new TiXmlElement( DOUBLE_STRING );
	item->SetAttribute( NAME_STRING, name );
	item->SetAttribute( RANGE_STRING, range );

	for( int i = 0; i < range; i++)
	{
		TiXmlElement array_item( ITEM_STRING );
		array_item.SetDoubleAttribute( VALUE_STRING, data[i]);
		item->InsertEndChild( array_item );
	}

	TiXmlNode* node = find_group(domain);
	node->InsertEndChild( *item );
	delete item;
	item = 0;
	change = true;
}

void XMLConfig::WriteHexArray(char* name, char * data[], int length_of_item, int range, const char* domain)
{
	assert(length_of_item <= MAX_HEX_LEN);
	char str[64];
	char hex[MAX_HEX_LEN * 2 + 1];

	remove_if_exists(name, HEX_STRING, domain);

	TiXmlElement* item = new TiXmlElement( HEX_STRING );

	item->SetAttribute( NAME_STRING, name );
	item->SetAttribute( LEN_STRING, length_of_item);
	item->SetAttribute( RANGE_STRING, range );

	for( int i = 0; i < range; i++)
	{
		*hex = 0;
		for(int j=0;j<length_of_item;j++)
		{
			sprintf(hex+j*2, "%02x", data[i][j]&255);
			hex[j*2+2] = 0;
		}

		TiXmlElement array_item( ITEM_STRING );
		array_item.SetAttribute( VALUE_STRING, hex);
		item->InsertEndChild( array_item );
	}

	TiXmlNode* node = find_group(domain);
	node->InsertEndChild( *item );
	delete item;
	item = 0;
	change = true;
}
#endif

// -----------------------------------------------------------------------------

/**
	Creates and loads the config object from the file.
	@param s the file name
	@param write the flag for object writing, false = read-only object.
*/
Config::Config(const char	*s, bool write)
{
#if (ENABLE_XML_CONFIG == 0)
	realcfg = new OldConfig(s,write);
#else
	// try load & convert an old form
	OldConfig* tmpcfg = new OldConfig(s,write);
	delete tmpcfg;

	realcfg = new XMLConfig(write);
	realcfg->Load(s);
#endif
}

/**
	Creates the new and empty config object.
	@param write the flag for object writing, false = read-only object.
*/
Config::Config(bool write)
{
#if (ENABLE_XML_CONFIG == 0)
	realcfg = new OldConfig(write);
#else
	realcfg = new XMLConfig(write);
#endif
}

/**
	Destruct object.
	The object will be saved if is 'writable' and state is 'changed'.
*/
Config::~Config()
{
	delete realcfg;
	realcfg = 0;
}

/**
	@return true if object state is valid.
*/
bool Config::IsValid(void)
{
	return !!realcfg;
}

/**
	Saves the object if is 'writable' and state is 'changed'.
	@return true if object was correctly load from file.
*/
bool Config::Save(void)
{
	return realcfg->Save();
}

/**
	Loads object from the file.
	@param fname the file name
	@return true if object was correctly load from file.
*/
bool Config::Load(const char* fname)
{
	return realcfg->Load(fname);
}

/**
	Reads an boolean variable from the config file.
	@param name the symbolic name of variable
	@param data where the value of the variable will be stored
	@param domain the path where variable is stored
	@return true if variable was found and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadBool(char* name, bool &data, const char* domain)
{
	return realcfg->ReadBool(name, data, domain);
}

/**
	Reads an integer variable from the config file.
	@param name the symbolic name of variable
	@param data where the value of the variable will be stored
	@param domain the path where variable is stored
	@return true if variable was found and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadInt(char* name, int &data, const char* domain)
{
	return realcfg->ReadInt(name, data, domain);
}

/**
	Reads a double variable from the config file.
	@param name the symbolic name of variable
	@param data where the value of the variable will be stored
	@param domain the path where variable is stored
	@return true if variable was found and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadDouble(char* name, double &data, const char* domain)
{
	return realcfg->ReadDouble(name, data, domain);
}

/**
	Reads a string variable from the config file.
	@param name the symbolic name of variable
	@param data the pointer where the string will be stored
	@param max the size of allocated memory for string
	@param domain the path where variable is stored
	@return true if variable was found, memory size fits its size and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadString(char* name,	char *data, int max, const char* domain)
{
	return realcfg->ReadString(name, data, max, domain);
}

/**
	Reads a block of binary data from the config file.
	@param name the symbolic name of variable
	@param data the pointer where the data will be stored
	@param size the size of allocated memory for data (max 512)
	@param domain the path where variable is stored
	@return true if variable was found, memory size fits its size and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadHex(char* name, char *data, int size, const char* domain)
{
	return realcfg->ReadHex(name, data, size, domain);
}

/**
	Writes an item of type boolean with the 'name' to the config object (to save to the file use Config::Save() )
	@param name the name of variable
	@param val the value to write (true or false)
	@param domain the path where variable will be stored
	@see Config::Config()
*/
void Config::WriteBool(char* name, bool val, const char* domain)
{
	realcfg->WriteBool(name, val, domain);
}

/**
	Writes an item of type 'int' with the 'name' to the config object (to save to the file use Config::Save() )
	@param name the name of variable
	@param val the value to write ( C integer )
	@param domain the path where variable will be stored
	@see Config::Config()
*/
void Config::WriteInt(char* name, int val, const char* domain)
{
	realcfg->WriteInt(name, val, domain);
}

/**
	Writes an item of type 'char *' with the 'name' to the config object (to save to the file use Config::Save() )
	@param name the name of variable
	@param val the string to write (a traditional ASCIIZ form)
	@param domain the path where variable will be stored
	@see Config::Config()
*/
void Config::WriteString(char* name, char *val, const char* domain)
{
	Config::realcfg->WriteString(name, val, domain);
}

/**
	Writes an item of type 'double' with the 'name' to the config object (to save to the file use Config::Save() )
	@param name the name of variable
	@param val the value to write
	@param domain the path where variable will be stored
	@see Config::Config()
*/
void Config::WriteDouble(char* name, double val, const char* domain)
{
	realcfg->WriteDouble(name, val, domain);
}

/**
	Writes an item of type 'char *' with the 'name' to the config object (to save to the file use Config::Save() ).
	This saves memory contents as HEXADECIMAL string. Bytes 00ff00 will be writed as  "00FF00" ascii string.
	@param name the name of variable
	@param val the value to write ( C integer )
	@param len the size of object in bytes to write (max. 512 bytes)
	@param domain the path where variable will be stored
	@see Config::Config()
*/
void Config::WriteHex(char* name, char* val, int len, const char* domain)
{
	realcfg->WriteHex(name, val, len, domain);
}

/**
	Writes an array of boolean type with name and range.
	@param name the name of variable
	@param data data array to write
	@param range the number of items in the array
	@param domain the path where variable will be stored
*/
void Config::WriteBoolArray(char* name, bool data[], int range, const char* domain)
{
	realcfg->WriteBoolArray(name, data, range, domain);
}

/**
	Writes an array of integer type with name and range.
	@param name the name of variable
	@param data data array to write
	@param range the number of items in the array
	@param domain the path where variable will be stored
*/
void Config::WriteIntArray(char* name, int data[], int range, const char* domain)
{
	realcfg->WriteIntArray(name, data, range, domain);
}

/**
	Writes an array of double type with name and range.
	@param name the name of variable
	@param data data array to write
	@param range the number of items in the array
	@param domain the path where variable will be stored
*/
void Config::WriteDoubleArray(char* name, double data[], int range, const char* domain)
{
	realcfg->WriteDoubleArray(name, data, range, domain);
}

/**
	Writes an array of char* type with name and range.
	@param name the name of variable
	@param data data array to write
	@param range the number of items in the array
	@param domain the path where variable will be stored
*/
void Config::WriteStringArray(char* name, char *data[], int range, const char* domain)
{
	realcfg->WriteStringArray(name, data, range, domain);
}

/**
	Writes an array of char* as binary data type with name and range.
	@param name the name of variable
	@param data data array to write
	@param length_of_item the sizeof of one data item
	@param range the number of items in the array
	@param domain the path where variable will be stored
*/
void Config::WriteHexArray(char* name, char *data[], int length_of_item, int range, const char* domain)
{
	return realcfg->WriteHexArray(name, data, length_of_item, range, domain);
}

/**
	Reads an array of boolean from the config file.
	@param name the symbolic name of variable
	@param data where the value of the variable will be stored
	@param range the number of items to read
	@param domain the path where variable is stored
	@return true if variable was found and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadBoolArray(char* name, bool data[], int range, const char* domain)
{
	return realcfg->ReadBoolArray(name, data, range, domain);
}

/**
	Reads an array of integer from the config file.
	@param name the symbolic name of variable
	@param data where the value of the variable will be stored
	@param range the number of items to read
	@param domain the path where variable is stored
	@return true if variable was found and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadIntArray(char* name, int data[], int range, const char* domain)
{
	return realcfg->ReadIntArray(name, data, range, domain);
}

/**
	Reads an array of double from the config file.
	@param name the symbolic name of variable
	@param data where the value of the variable will be stored
	@param range the number of items to read
	@param domain the path where variable is stored
	@return true if variable was found and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadDoubleArray(char* name, double data[], int range, const char* domain)
{
	return realcfg->ReadDoubleArray(name, data, range, domain);
}

/**
	Reads an array of char* from the config file.
	@param name the symbolic name of variable
	@param data an array of pointers to char - the pointers must be set to already allocated memory.
	@param max the size of allocated memory for each one string
	@param range the number of items to read
	@param domain the path where variable is stored
	@return true if variable was found and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadStringArray(char* name, char *data[], int max, int range, const char* domain)
{
	return realcfg->ReadStringArray(name, data, max, range, domain);
}

/**
	Reads an array of binary items from the config file.
	@param name the symbolic name of variable
	@param data an array of pointers to char - the pointers must be set to already allocated memory.
	@param length_of_item the size of allocated memory for each one object
	@param range the number of items to read
	@param domain the path where variable is stored
	@return true if variable was found and loaded correctly to the data parameter. If false the 'data' parameter is untouched.
*/
bool Config::ReadHexArray(char* name, char *data[], int length_of_item, int range, const char* domain)
{
	return realcfg->ReadHexArray(name, data, length_of_item, range, domain);
}


