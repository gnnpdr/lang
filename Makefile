CC = g++

CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

HOME = $(shell cd)
CFLAGS += -I $(HOME)

ERRDIR = errors
OBJDIR  = obj
DRAWDIR = draw
STRUCTDIR = structure
LEXDIR = lex_analysis
SYNDIR = syn_analysis
INTERACTIONSDIR = interactions
TREELIBDIR = tree_lib
STKLIBDIR = stack_lib
USERDIR = user

TARGET = differentiator

SOURCES = main.cpp
ERR = errors/errors.cpp 
DRAW = draw/draw_tree.cpp 
STRUCT = structure/tree_structure.cpp 
LEX = lex_analysis/lex_analysis.cpp
SYN = syn_analysis/syn_analysis.cpp
INTERACTIONS = interactions/diff.cpp interactions/talor.cpp interactions/latex_dump.cpp interactions/write_math_expression.cpp  
TREELIB = tree_lib/get_database.cpp 
STKLIB = stack_lib/stk.cpp 
USER = user/user.cpp 

OBJECTS = $(SOURCES:%.cpp = $(OBJDIR)/%.o)
ERROBJ = $(ERR:$(ERRDIR)/%.cpp = $(OBJDIR)/%.o)
DRAWOBJ = $(DRAW:$(DRAWDIR)/%.cpp = $(OBJDIR)/%.o)
STRUCTOBJ = $(STRUCT:$(STRUCTDIR)/%.cpp = $(OBJDIR)/%.o)
LEXOBJ = $(LEX:$(LEXDIR)/%.cpp = $(OBJDIR)/%.o)
SYNOBJ = $(SYN:$(SYNDIR)/%.cpp = $(OBJDIR)/%.o)
INTERACTIONSOBJ = $(INTERACTIONS:$(INTERACTIONSDIR)/%.cpp = $(OBJDIR)/%.o)
TREELIBOBJ = $(TREELIB:$(TREELIBDIR)/%.cpp = $(OBJDIR)/%.o)
STKLIBOBJ = $(STKLIB:$(STKLIBDIR)/%.cpp = $(OBJDIR)/%.o)
USEROBJ = $(USER:$(USERDIR)/%.cpp = $(USER)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(ERROBJ) $(STRUCTOBJ) $(DRAWOBJ) $(TREELIBOBJ) $(STKLIBOBJ) $(LEXOBJ) $(SYNOBJ) $(INTERACTIONSOBJ) $(USEROBJ)
	$(CC) $^ -o $@ $(CFLAGS)

$(OBJDIR)/%.o : %.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(ERRDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(STRUCTDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(DRAWDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(TREELIBDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(STKLIBDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(LEXDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(SYNDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(INTERACTIONSDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(USERDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)
 
clean:
	del -rf $(TARGET) $(OBJDIR)/*.o