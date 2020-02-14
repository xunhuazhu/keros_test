#ifndef __UNI_CLI_H__
#define __UNI_CLI_H__

typedef int cli_cmd_handle(int argc, char *argv[]);

struct cli_param{
	char *data;
	struct cli_param *next;
};

struct uni_cli_param{
	int		count;
	struct	cli_param *params;
	char	**argv;
};

typedef struct uni_cli_cmd{
	char			*cmd;
	char			*descript;
	cli_cmd_handle	*handle;
	struct uni_cli_cmd *next;
}uni_cli_cmd_t;

struct uni_cli_ctrl{
	char			recvbuf[256];
	int				index;
	uni_cli_cmd_t	*head;
};

int uni_cli_init(void);

int uni_cli_cmd_register(char *cmd_name,cli_cmd_handle *function,char *descript);

#endif
