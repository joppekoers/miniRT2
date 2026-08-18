#include "get_next_line.h"
#include "constants.h"
#include <fcntl.h>

void read_obj_file(t_gui* gui, const char* path)
{

	(void)(gui);
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		exit_e(ft_strjoin("Cannot open ", path));
	char* line;

	while (get_next_line(fd, &line) > 0)
	{
		printf("%s\n", line);
	}
}
