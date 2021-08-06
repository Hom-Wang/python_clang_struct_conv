#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define PYTHON_CLASS_FORMAT         "from ctypes import *\n\nclass %s(Structure):\n    _fields_ = [\n%s    ]\n"

const char pythontype[][16] = {
    "c_ubyte",      // 0
    "c_ushort",     // 1
    "c_uint",       // 2
    "c_ulonglong",  // 3
    "c_byte",       // 4
    "c_short",      // 5
    "c_int",        // 6
    "c_longlong",   // 7
    "c_float",      // 8
    "c_double",     // 9
};

const char clangtype[][16] = {
    "uint8_t",      // 0
    "uint16_t",     // 1
    "uint32_t",     // 2
    "uint64_t",     // 3
    "int8_t",       // 4
    "int16_t",      // 5
    "int32_t",      // 6
    "int64_t",      // 7
    "float",        // 8
    "double",       // 9
};

// argv[0]
// argv[1]  outputfilename
// argv[2]  inputfilename
// argv[2]  typename
char outputfilename[1024] = {0};
char inputfilename[1024] = {0};
char typename[1024] = {0};

typedef struct {
    int size;
    const char *type;
    char name[256];
} var_t;

int get_clang_typedef(char *filename, char *typename, var_t *var, int *lens);
int create_python_class(char *filename, var_t *vat, int lens);

int main(int argc, char **argv)
{
#if 0
    printf("argc = %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    printf("\n");
#endif

    var_t varlist[256] = {0};
    var_t *var = varlist;
    int typelens = 0;

    strcpy(outputfilename, argv[1]);
    strcpy(inputfilename, argv[2]);
    strcpy(typename, argv[3]);
    get_clang_typedef(inputfilename, typename, var, &typelens);
#if 1
    printf("\n-- Variable List [%d] ----------\n", typelens);
    for (int i = 0; i < typelens; i++)
    {
        // printf("[%2d] %s(%d) ... %-s\n", i, var[i].type, var[i].size, var[i].name);
        printf("[%2d] %-24s %s(%d)\n", i+1, var[i].name, var[i].type, var[i].size);
    }
#endif
    create_python_class(outputfilename, var, typelens);
    return 0;
}

int get_variable_size(char *varname)
{
    int size = 1;
    while (*varname != '\0')
    {
        if (*varname == '[')
        {
            *varname = '\0';
            size = strtoul(&varname[1], NULL, 0);
        }
        varname++;
    }
    return size;
}

int parsing_variable(char *line, var_t *var)
{
    char *pstr = NULL;
    int typenum = sizeof(clangtype) /  sizeof(clangtype[0]);
    int offset;
    for (int i = 0; i < typenum; i++)
    {
        pstr = strstr(line, clangtype[i]);
        if ((pstr != NULL) && (offset > 0))
        {
            int nameidx = 0;
            pstr += strlen(clangtype[i]);
            var->type = pythontype[i];
            while (*pstr != '\0')
            {
                var->name[nameidx] = *pstr;
                if (*pstr != ' ')
                {
                    nameidx++;
                }
                if (*pstr == ';')
                {
                    var->name[nameidx-1] = '\0';
                    var->size = get_variable_size(var->name);
                    return 1;
                }
                *pstr++;
            }
        }
    }
    return 0;
}

int get_clang_typedef(char *filename, char *typename, var_t *var, int *lens)
{
    FILE *fp = fopen(inputfilename, "rb");
    if (fp == NULL)
    {
        printf("file not found ... '%s'\n", inputfilename);
        return -1;
    }

    int startparsing = false;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    *lens = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (strstr(line, "typedef struct") != NULL)
        {
            startparsing = true;
            *lens = 0;
        }
        else if (strstr(line, typename) != NULL)
        {
            break;
        }
        if (startparsing)
        {
            if (parsing_variable(line, &var[*lens]))
            {
                (*lens)++;
            }
        }
    }
    fclose(fp);
    return 1;
}

int get_filename(const char *string, char *name)
{
    // find last slash and last dot character
    int slash = -1, dot = -1;
    for (int i = 0; i < strlen(string); i++)
    {
        if ((string[i] == '/') || (string[i] == '\\'))
        {
            slash = i;
        }
        else if (string[i] == '.')
        {
            dot = i;
        }
    }

    if (dot < 0)
    {
        return -1;
    }
    if (slash < 0)
    {
        strcpy(name, string);
        name[dot] = '\0';
    }
    else
    {
        strcpy(name, &string[slash+1]);
        name[dot-slash-1] = '\0';
    }
    return 1;
}

int create_python_class(char *fullfilename, var_t *var, int lens)
{
    char outputfile[8192] = {0};
    char filename[256] = {0};
    char information[8192] = {0};

    FILE *fp = fopen(fullfilename, "wb");
    if (fp == NULL)
    {
        printf("can't create file ... '%s'\n", fullfilename);
        return -1;
    }
    get_filename(fullfilename, filename);
    int idx = 0;
    for (int i = 0; i < lens; i++)
    {
        if (var[i].size > 1)
        {
            idx += sprintf(&information[idx], "        (\"%s\", %s*%d),\n",
                var[i].name, var[i].type, var[i].size);
        }
        else
        {
            idx += sprintf(&information[idx], "        (\"%s\", %s),\n",
                var[i].name, var[i].type);
        }
    }
    int strlens = sprintf(outputfile, PYTHON_CLASS_FORMAT, filename, information);
    fprintf(fp, "%s", outputfile);
    fclose(fp);
    return 0;
}
