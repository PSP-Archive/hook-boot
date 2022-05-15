#include "common.h"

#if 0
int strcpy(char *str1,const char *str2)
{
	while(*str1)
	{
		*str1 = *str2;
		str1++;
		str2++;
	}
}
#endif

int str_cmp(const char *str1,const char *str2)
{
	int ret;
	while(1)
	{
		ret=(*str1|0x20)-(*str2|0x20);
		if(ret || *str1==0 ) break;
		str1++;
		str2++;
	}
	return ret;
}

int str_cmp2(const char *str1,const char *str2)
{
	int ret;
	while(1)
	{
		if(*str1==0 || *str2==0) return 0;
		ret=(*str1|0x20)-(*str2|0x20);
		if(ret) break;
		str1++;
		str2++;
	}
	return ret;
}

char *str_chr(const char *str,int chr)
{
	while(1)
	{
		if(*str==0) return 0;
		if(*str==chr) break;
		str++;
	}
	return (char *)str;
}

/////////////////////////////////////////////////////////////////////////////
// get string data from line
/////////////////////////////////////////////////////////////////////////////

unsigned char *get_string(unsigned char *buf,char **name_str)
{
	char chr;

	*name_str = NULL;

	if(!buf) return NULL;

	// seatch name top
	while(1)
	{
		chr = *buf;
		if(!chr || chr>' ') break;
		buf++;
	}
	if(!chr) return NULL;

	*name_str = buf;

	// search name end (token)
	while(1)
	{
		chr = *buf;
		if(chr<' ' || chr==' ') break;
		buf++;
	}
	// set token
	*buf = 0x00; // EOM
	buf++;

	if(chr != 0x0d)
	{
		// search next line
		while(1)
		{
			chr = *buf;
			if((chr==0) || (chr==0x0d)) break;
			buf++;
		}
	}
	while(1)
	{
		chr = *buf;
		if((chr==0) || (chr>=0x20)) break;
		buf++;
	}
	//
	return buf;
}

int str2num(const char **str_list, const char *str)
{
	int i;

	for(i=0;str_list[i];i++)
		if(strcmp(str_list[i],str)==0) return i;
	return -1;
}

unsigned int str2hex(const char *str)
{
	unsigned int val_hex = 0;
	int val;

	while(1)
	{
		val = *str-'0';
		if(val<0) break;
		if(val>9)
		{
			val = (*str|0x20)-'a' + 10;
			if(val<10 || val>15) break;
		}
		val_hex = val_hex*16 + val;
		str++;
	}
	return val_hex;
}

char *get_line_args(char *ptr,int *num_arg,char **arg)
{
	char *next_line;
	int num;
	int i;

	num = 0;
	// separate line
	next_line = strchr(ptr,0x0d);
	if(next_line)
	{
		*next_line++ = 0x00;
		if(*next_line==0x0a) next_line++;
		// EOF?
		if( (*next_line==0x00) || (*next_line==0x1a) ) next_line = NULL;
	}
//printf("get_line '%s'\n",ptr);

	// arg0
	arg[num] = ptr;
	ptr = strchr(ptr,'=');
	if(ptr!=NULL)
	{
		*ptr++ = 0x00;
	}
//printf("arg%d '%s'\n",num,arg[num]);
	num++;

	while(ptr!=NULL)
	{
		arg[num] = ptr;
		ptr = strchr(ptr,',');
		if(ptr)
		{
			*ptr++ = 0x00;
		}
//printf("arg%d '%s'\n",num,arg[num]);
		num++;
	}

	// strip ALL args
	for(i=0;i<num;i++)
	{
		// delete top ' '
		while(arg[i][0]==' ')
			arg[i]++;
		// delete last ' '
		while( (ptr=strchr(arg[i],' '))!=NULL)
			*ptr = 0x00;
//
//printf("ARG %d='%s'\n",i,arg[i]);
//
	}
	*num_arg = num;
	return next_line;
}

static char rep_buf[MAX_FILE_PATH];
int str_rep(char *path,const char *search,const char *rep)
{
	// hit ?
//printf("str_rep(%08X,%08X)\n",(int)path,(int)search);

	if(str_cmp2(path,search)) return 0;

	// copy bottom
	strcpy(rep_buf,&path[strlen(search)]);
	
	// make
	sprintf(path,"%s%s",rep,rep_buf);

	return 1;
}

int is_str(const char *str)
{
//	if( ((unsigned int)str>=PSP_UMEM_TOP) && ((unsigned int)str<PSP_UMEM_END) )
	if( ((unsigned int)str>=0x08400000) && ((unsigned int)str<0x0a000000) )
		return 1;

	if( ((unsigned int)str>=PSP_KMEM_TOP) && ((unsigned int)str<PSP_KMEM_END) )
		return 1;
	return 0;
}
