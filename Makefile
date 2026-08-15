# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: jkoers <jkoers@student.codam.nl>             +#+                      #
#                                                    +#+                       #
#    Created: 2020/11/05 15:36:08 by jkoers        #+#    #+#                  #
#    Updated: 2021/01/17 13:35:42 by jkoers        ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME      		= miniRT

CC          	= gcc
CFLAGS      	= -Wall -Wextra -Werror -Wuninitialized -O3
# CFLAGS			= -Wall -Wextra -Wuninitialized -O0 # debug

SRCEXT      	= c
SRCDIR      	= src
HEADERDIR		= include
OBJEXT      	= o
BUILDDIR    	= obj

SETTINGS		= settings.h
LIBDIR			= lib
ifeq ($(shell uname),Linux)
MLXDIR			= minilibx-linux/
LINKSRC			= -lm -lpthread
LINKS			= $(LINKSRC) -L$(LIBDIR)/$(MLXDIR) -lmlx -lXext -lX11
LIBS			= $(LIBDIR)/minilibx-linux/libmlx.a \
				  $(LIBDIR)/libft/bin/libft.a
else
MLXDIR			= minilibx_mms_20200219/
LINKSRC			=
LINKS			=
LIBS			= $(LIBDIR)/libft/bin/libft.a libmlx.dylib
endif

HEADERS			= $(shell find $(HEADERDIR) -type f -name '*.h')
SRC				= $(shell find $(SRCDIR) -type f -name '*.c')
OBJ				= $(foreach src,$(SRC),$(BUILDDIR)/$(notdir $(src:.$(SRCEXT)=.$(OBJEXT))))

CLANG_FORMAT	= $(shell command -v clang-format 2>/dev/null \
					|| find $(HOME)/.vscode/extensions -type f -name clang-format 2>/dev/null | head -n 1)

STARTGREEN		= @echo "\033[38;2;0;255;0m\c"
RESETCOLOR		= @echo "\033[0m\c"
TESTRT			= rt/dragon.rt

VPATH = $(shell find $(SRCDIR) -type d | tr '\n' ':' | sed -E 's/(.*):/\1/')

all:
	make -j14 $(NAME)


$(NAME): $(BUILDDIR)/ $(OBJ) $(HEADERS) $(LIBS) $(SETTINGS)
	$(CC) $(CFLAGS) -I$(HEADERDIR) $(BUILDDIR)/*.$(OBJEXT) -o $(NAME) \
$(LIBS) $(LINKS)

# sources

$(BUILDDIR)/%.$(OBJEXT): %.$(SRCEXT) $(HEADERS) $(SETTINGS)
	$(CC) $(CFLAGS) -I$(HEADERDIR) -c $< -o $(BUILDDIR)/$(notdir $@) $(LINKSRC)

# libs

ifeq ($(shell uname),Linux)
$(LIBDIR)/minilibx-linux/libmlx.a:
	$(MAKE) -C $(LIBDIR)/minilibx-linux/
else
libmlx.dylib:
	make -C $(LIBDIR)/minilibx_mms_20200219/
	cp $(LIBDIR)/minilibx_mms_20200219/libmlx.dylib .
endif

$(LIBDIR)/libft/bin/libft.a:
	$(MAKE) -C $(LIBDIR)/libft/

clean:
	make -C $(LIBDIR)/minilibx-linux/ clean
	make -C $(LIBDIR)/libft/ clean
ifneq ($(BUILDDIR),.)
	/bin/rm -rf $(BUILDDIR)/
endif
	/bin/rm -rf obj_asan/

fclean:
	$(MAKE) clean
# make -C $(LIBDIR)/minilibx-linux/ fclean
	make -C $(LIBDIR)/libft/ fclean
	/bin/rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

$(BUILDDIR)/:
	mkdir -p $(BUILDDIR)

format:
	@test -n "$(CLANG_FORMAT)" || { echo "clang-format not found"; exit 1; }
	$(CLANG_FORMAT) --style=file -i $(SRC) $(HEADERS) $(SETTINGS)

silent:
	@$(MAKE) all > /dev/null

SANITIZE = -fsanitize=address -g

debug:
	@/bin/rm -f $(NAME)
	@$(MAKE) all BUILDDIR=obj_asan CFLAGS="$(CFLAGS) $(SANITIZE)" > /dev/null
	@./$(NAME) $(TESTRT) --save
	@open scene.bmp

standard:
	@/bin/rm -f $(NAME)
	@$(MAKE) all > /dev/null
	@./$(NAME) $(TESTRT) --save
	@open scene.bmp

rt:
	@$(MAKE) all > /dev/null
	@find rt/ -name "*.rt" -exec echo {} \; \
-exec ./$(NAME) {} --save \; \
-exec mv scene.bmp {}.bmp \; \
-exec echo "" \;

rttest:
	@$(MAKE) all > /dev/null
	@find rt_test/ -name "*.rt" -exec echo {} \; \
-exec ./$(NAME) {} --save \; \
-exec mv scene.bmp {}.bmp \; \
-exec echo "" \;

.PHONY: all clean fclean re silent eval evalclean rt rtall format
