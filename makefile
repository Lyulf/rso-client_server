CC				= g++
STD				= -std=c++17
WARNINGS		= -Wall -Wextra -Wpedantic -Werror
CXXFLAGS		= $(STD) $(WARNINGS) -I$(INC_DIR)
LDFLAGS			= $(STD) $(WARNINGS) -pthread
DEPFLAGS 		= -MT $@ -MD -MP -MF $(DEP_DIR)/$*.dep

SERVER_EXE		= server
CLIENT_EXE		= client
SERVER_SRC		= server_main.cpp server.cpp common.cpp
CLIENT_SRC		= client_main.cpp client.cpp common.cpp

EXE_DIR			= .
SRC_DIR			= src
INC_DIR			= include
OBJ_DIR			= .obj
DEP_DIR			= .dep

SERVER_OBJ		= $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ		= $(CLIENT_SRC:.cpp=.o)
DEP				= $(SERVER_OBJ_PATH:.o=.d) $(CLIENT_OBJ_PATH:.o=.d)

SERVER_EXE_PATH	= $(patsubst %, $(EXE_DIR)/%, $(SERVER_EXE))
CLIENT_EXE_PATH	= $(patsubst %, $(EXE_DIR)/%, $(CLIENT_EXE))
SERVER_OBJ_PATH	= $(patsubst %, $(OBJ_DIR)/%, $(SERVER_OBJ))
CLIENT_OBJ_PATH	= $(patsubst %, $(OBJ_DIR)/%, $(CLIENT_OBJ))

all: $(SERVER_EXE_PATH) $(CLIENT_EXE_PATH)

$(SERVER_EXE_PATH): $(SERVER_OBJ_PATH)
	$(CC) -o $@ $^ $(LDFLAGS)

$(CLIENT_EXE_PATH): $(CLIENT_OBJ_PATH)
	$(CC) -o $@ $^ $(LDFLAGS)

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEP_DIR)/%.d
	$(CC) $(DEPFLAGS) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o $(SERVER_EXE_PATH) $(CLIENT_EXE_PATH)

.PHONY: cleandep
cleandep:
	rm -f $(DEP_DIR)/*.d

.PRECIOUS: $(DEP_DIR)/%.d
$(DEP_DIR)/%.d: ;

-include $(DEP)