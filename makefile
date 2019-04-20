CC				= g++
STD				= -std=c++17
WARNINGS		= -Wall -Wextra -Wpedantic -Werror
CXXFLAGS		= $(STD) $(WARNINGS)
LDFLAGS			= $(STD) $(WARNINGS) -lpthread
CPP				= cpp

SERVER_EXE		= server
CLIENT_EXE		= client
SERVER_SRC		= server_main.cpp server.cpp common.cpp
CLIENT_SRC		= client_main.cpp client.cpp common.cpp

EXE_DIR			=
SRC_DIR			= src/
OBJ_DIR			= obj/

SERVER_OBJ		= $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ		= $(CLIENT_SRC:.cpp=.o)
DEP				= $(SERVER_OBJ_PATH:.o=.d) $(CLIENT_OBJ_PATH:.o=.d)

SERVER_EXE_PATH	= $(join $(addsuffix $(EXE_DIR), $(dir $(SERVER_EXE))), $(notdir $(SERVER_EXE)))
CLIENT_EXE_PATH	= $(join $(addsuffix $(EXE_DIR), $(dir $(CLIENT_EXE))), $(notdir $(CLIENT_EXE)))
SERVER_SRC_PATH	= $(join $(addsuffix $(SRC_DIR), $(dir $(SERVER_SRC))), $(notdir $(SERVER_SRC)))
CLIENT_SRC_PATH	= $(join $(addsuffix $(SRC_DIR), $(dir $(CLIENT_SRC))), $(notdir $(CLIENT_SRC)))
SERVER_OBJ_PATH	= $(join $(addsuffix $(OBJ_DIR), $(dir $(SERVER_OBJ))), $(notdir $(SERVER_OBJ)))
CLIENT_OBJ_PATH	= $(join $(addsuffix $(OBJ_DIR), $(dir $(CLIENT_OBJ))), $(notdir $(CLIENT_OBJ)))


all: $(SERVER_EXE_PATH) $(CLIENT_EXE_PATH)

$(SERVER_EXE_PATH): $(SERVER_OBJ_PATH)
	$(CC) -o $@ $^ $(LDFLAGS)

$(CLIENT_EXE_PATH): $(CLIENT_OBJ_PATH)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(DEP)

$(OBJ_DIR)%.d: $(SRC_DIR)%.cpp
	$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(SERVER_OBJ_PATH) $(SERVER_EXE_PATH) $(CLIENT_OBJ_PATH) $(CLIENT_EXE_PATH)

.PHONY: cleandep
cleandep:
	rm -f $(DEP)