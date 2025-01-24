CPP := c++

NAME := webserv

INCLUDES := -I./includes \
			-I./includes/parser \
			-I./includes/FdClasses \
			-I./includes/ConfigParser \

FSAN := address

WWW :=
CXXFLAGS :=  $(WWW) -std=c++17 -g -fsanitize=$(FSAN) -O0 \
			-Wconversion -Wsign-conversion $(INCLUDES) \

#-Wno-shadow -Wshadow

SRCS_DIR := srcs/
SRCS := main.cpp \
		Parser.cpp \
		Lexer.cpp \
		Token.cpp \
		utils.cpp \
		to_string.cpp \
		config_parser.cpp \
		Exceptions.cpp \
		Manager.cpp \
		FdClasses/BaseFd.cpp \
		FdClasses/Server.cpp \
		FdClasses/Client.cpp \
		FdClasses/ReadFd.cpp \
		ConfigParser/ConfigParser.cpp \
		ConfigParser/DefaultErrorPages.cpp \
		ConfigParser/LocationConfigFile.cpp\
		ConfigParser/ServerConfigFile.cpp \
		Request.cpp \
		StringArray.cpp \


SRCS := $(SRCS:%=$(SRCS_DIR)%)

OBJS_DIR := objs/
OBJS := $(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_DIR)%.o)

GREEN	=	\033[0;32m
YELLOW	=	\033[33m
CYAN	=	\033[0;36m
CLEAR	=	\033[0m

.PHONY: all normal leaks clean fclean re compile_commands.json

all: $(NAME) client

$(NAME): $(OBJS)
	$(CPP) $(CXXFLAGS) $(OBJS) -o $(NAME)
	cp -f $(NAME) ./docker_testing/server

normal: $(NAME)
	@echo "$(GREEN) Compiled $(NAME) $(CLEAR)"

client: client.cpp
	$(CPP) $(CXXFLAGS) client.cpp -o client

leaks: clean
	make CXXFLAGS="$(CXXFLAGS) -g -DLEAKS"
	@echo "$(GREEN) Compiled $(NAME) for 'system(\"leaks $(NAME)\");' on Mac-OS $(CLEAR)"

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	@mkdir -p $(@D)
	@$(CC) -c $< -o $@ $(CXXFLAGS)

clean:
	@rm -f $(OBJS)
	@rm -f client.o
	@echo "$(YELLOW) Cleaned object files $(CLEAR)"

fclean:
	@rm -rf $(OBJS_DIR)
	@rm -f $(NAME)
	@rm -f client
	@echo "$(YELLOW) Cleaned object files, build directories and executables \
		$(CLEAR)"

re: fclean normal





PWD = $(shell pwd)
compile_commands.json:
	@echo '[' > compile_commands.json
	@$(foreach src, $(SRCS), \
		echo "\t{" >> compile_commands.json; \
		echo "\t\t\"directory\": \"$(PWD)\"," >> compile_commands.json; \
		echo "\t\t\"command\": \"$(CPP) $(CXXFLAGS) -o $(OBJS_DIR)$$(basename $(src) .cpp).o $(src)\"," >> compile_commands.json; \
		echo "\t\t\"file\": \"$(src)\"" >> compile_commands.json; \
		echo "\t}," >> compile_commands.json;)
	@sed -i '' -e '$$ d' compile_commands.json
	@echo "\t}" >> compile_commands.json
	@echo ']' >> compile_commands.json
	@echo "$(YELLOW) Pseudo compile_commands.json generated $(CLEAR)"

