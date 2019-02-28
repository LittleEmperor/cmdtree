
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

struct trie_node{
	char me;
	char padding[7];
	struct trie_node *next_level;
	struct trie_node *this_level;
	void *cmd_func;
};
struct trie_node *g_cmd_tree_root;

#if DEBUG 
// for debug 
long g_node_counter = 0;
#endif

static int __cmd_insert(struct trie_node **root, const char *cmd, void *cmd_func)
{
	if(!cmd && !cmd_func){
		//printf("fatal on func %s: param err\n", __func__);
		return -1;
	}

	/**
	 * insert a cmd to a trie struct
	 * for example:
	 * 	we insert 2 cmds of [find] and [file],
	 * 	and the trie struct looks like this:
	 * 	
	 *                                  /this is final-status
	 *                                 /
	 *   next_level                   *
	 * f ----------> i -----> l ---->*e*---->NULL
	 * |             |        |       |
	 * |this_level   |        |      NULL
	 * |             |        |              /this is a final-status
	 * |             |        |       *-----/
	 * NULL        NULL       n ---->*d*----------> NULL
	 *                        |       |
	 *                        |       |
	 *                        |       |
	 *                       NULL    NULL
	 *
	 *
	 * NOTE that [e] of 'file' and [d] of 'find' is a final-status, which trie_node->cmd_func is a valid value.
	 * for all other nodes, trie_node->cmd_func is NULL, which means it is NOT a valid cmd.
	 *
	 * */
	unsigned long i = 0;
	struct trie_node *trie_node = *root;
	struct trie_node *trie_node_bak = *root;
	struct trie_node **new_node = root;
	while(cmd[i]){
		if(!trie_node){
			trie_node = (struct trie_node *)malloc(sizeof(struct trie_node));
			if(!trie_node){
				printf("fatal on func %s: malloc err\n", __func__);
				exit(-1);
			}
			trie_node->me = cmd[i];
			trie_node->cmd_func = NULL;
			trie_node->next_level = NULL;
			trie_node->this_level = NULL;
			*new_node = trie_node;
#if DEBUG
			g_node_counter++;
			printf("add char [%c], nr:%lu\n", cmd[i], g_node_counter);
#endif
		}

		trie_node_bak = trie_node;

		if(trie_node->me == cmd[i]){
			new_node = &trie_node->next_level;
			trie_node = trie_node->next_level;
			i++;
		}else{
			new_node = &trie_node->this_level;
			trie_node = trie_node->this_level;
		}

	}
	trie_node_bak->cmd_func = cmd_func;

	return 0;
}
static void *__cmd_search(struct trie_node *root, const char *cmd)
{
	long i = 0;
	struct trie_node *trie_node = root;
	struct trie_node *trie_node_bak = root;

	if(!cmd){
		//printf("fatal on func %s: param err\n", __func__);
		return NULL;
	}

	while(cmd[i]){

		trie_node_bak = trie_node;

		if(!trie_node)
			break;

		if(trie_node->me == cmd[i]){
			trie_node = trie_node->next_level;
			i++;
		}else{
			trie_node = trie_node->this_level;
		}
	}
	if(trie_node_bak)
		return (void *)(trie_node_bak->cmd_func);
	return NULL;
}
static void __trie_node_print(struct trie_node *root)
{
	struct trie_node *node = root;
	if(node){
		printf("[%c], func:%p\n", node->me, node->cmd_func);
	}else{
		printf("node is NULL\n");
		return ;
	}
	printf("print node[%c]->next_level:\t", node->me);
	__trie_node_print(node->next_level);

	printf("print node[%c]->this_level:\t", node->me);
	__trie_node_print(node->this_level);

}

void *my_cmd_func_file(void *args)
{
	(void)args;
	printf("this is func file\n");
	return NULL;
}
void *my_cmd_func_find(void *args)
{
	(void)args;
	printf("this is func find\n");
	return NULL;
}
void *my_cmd_func_frame(void *args)
{
	(void)args;
	printf("this is func frame\n");
	return NULL;
}
void *my_cmd_func_break(void *args)
{
	(void)args;
	printf("this is func break\n");
	return NULL;
}
void *my_cmd_func_bt(void *args)
{
	(void)args;
	printf("this is func bt\n");
	return NULL;
}
void *my_cmd_func_flush(void *args)
{
	(void)args;
	printf("this is func flush\n");
	return NULL;
}
void *my_cmd_func_bookmark(void *args)
{
	(void)args;
	printf("this is func bookmark\n");
	return NULL;
}
void *my_cmd_func_book(void *args)
{
	(void)args;
	printf("this is func book\n");
	return NULL;
}
void *my_cmd_func_run(void *args)
{
	(void)args;
	printf("this is func run\n");
	return NULL;
}
void *my_cmd_func_return(void *args)
{
	(void)args;
	printf("this is func return\n");
	return NULL;
}

int main()
{
#define MAX_CMD_NR 10
	struct cmd_param{
		const char *cmd;
		void *(*cmd_func)(void *);
	};
	struct cmd_param param[MAX_CMD_NR] = {
		{"file", my_cmd_func_file}, 
		{"find", my_cmd_func_find}, 
		{"frame", my_cmd_func_frame}, 
		{"break", my_cmd_func_break}, 
		{"bt", my_cmd_func_bt}, 
		{"flush", my_cmd_func_flush}, 
		{"bookmark", my_cmd_func_bookmark}, 
		{"book", my_cmd_func_book}, 
		{"run", my_cmd_func_run}, 
		{"return", my_cmd_func_return}, 
	};
	unsigned i;
	for(i=0; i<MAX_CMD_NR; i++){
		__cmd_insert(&g_cmd_tree_root, param[i].cmd, (void *)param[i].cmd_func);
	}

#if DEBUG
	printf("node count:%lu\n", g_node_counter);
	__trie_node_print(g_cmd_tree_root);
#endif

	/**
	 * find cmd in a trie and call cmd_func
	 * */
	void *cmd_func_x = NULL;
	const char *cmd_param_2[MAX_CMD_NR] = {"find","ru", "retur", "flush", "bt", "brkae", "book", "bookmar"};
	for(i=0; i<MAX_CMD_NR; i++){
		if(!cmd_param_2[i])continue;

		printf("we are finding cmd [%s]:\t", cmd_param_2[i]);
		cmd_func_x = __cmd_search(g_cmd_tree_root, cmd_param_2[i]);
		if(!cmd_func_x)
			printf("cmd [%s] NOT found\n", cmd_param_2[i]);
		else
			((void *(*)(void *))cmd_func_x)(NULL);
	}

	return 0;
}
