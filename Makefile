CC = g++

CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

HOME = $(shell cd)
CFLAGS += -I $(HOME)

ERRDIR = errors
DRAWDIR = draw
DIFFDIR = differentiator_lib
LEXDIR = lex_analysis
SYNDIR = syn_analysis

TARGET = soviet_lang

SOURCES = main.cpp
ERR = errors/errors.cpp 
DRAW = draw/draw_tree.cpp 
DIFF = differentiator_lib/tree_structure.cpp differentiator_lib/get_database.cpp
LEX = lex_analysis/lex_analysis.cpp
SYN = syn_analysis/syn_analysis.cpp

OBJECTS = $(SOURCES:%.cpp = $(OBJDIR)/%.o)
ERROBJ = $(ERR:$(ERRDIR)/%.cpp = $(OBJDIR)/%.o)
DRAWOBJ = $(DRAW:$(DRAWDIR)/%.cpp = $(OBJDIR)/%.o)
DIFFOBJ = $(DIFF:$(DIFFDIR)/%.cpp = $(OBJDIR)/%.o)
LEXOBJ = $(LEX:$(LEXDIR)/%.cpp = $(OBJDIR)/%.o)
SYNOBJ = $(SYN:$(SYNDIR)/%.cpp = $(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(ERROBJ) $(DIFFOBJ) $(LEXOBJ) $(DRAWOBJ) $(SYNOBJ)
	$(CC) $^ -o $@ $(CFLAGS)

$(OBJDIR)/%.o : %.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(ERRDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(DIFFDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(LEXDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(DRAWDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)

$(OBJDIR)/%.o : $(SYNDIR)/%.cpp
	$(CC) -c $^ - o $@ $(CFLAGS)
 
clean:
	del -rf $(TARGET) $(OBJDIR)/*.o