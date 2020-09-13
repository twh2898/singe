
LD=$(CXX)

SFML=`pkg-config --cflags sfml-all glew`
INCLUDES=-Isrc/includes
CFLAGS=-O0 -g $(SFML) $(INCLUDES)
CPPFLAGS=-O0 -g -std=c++2a $(SFML) $(INCLUDES)
LDFLAGS=`pkg-config --libs sfml-all glew gl`

TARGET=main
OBJECTS=main.o Camera.o Menu.o Shader.o
HEADERS=vbo.hpp Camera.hpp Menu.hpp Shader.hpp

SRCDIR=src
OBJDIR=obj

_OBJECTS=$(addprefix $(OBJDIR)/, $(OBJECTS))

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(_OBJECTS)
	$(LD) $(LDFLAGS) $(_OBJECTS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJDIR)/main.o: $(addprefix $(SRCDIR)/includes/, $(HEADERS))
$(OBJDIR)/Camera.o: $(addprefix $(SRCDIR)/, Camera.cpp includes/Camera.hpp)
$(OBJDIR)/Menu.o: $(addprefix $(SRCDIR)/, Menu.cpp includes/Menu.hpp)
$(OBJDIR)/Shader.o: $(addprefix $(SRCDIR)/, Shader.cpp includes/Shader.hpp)

clean: $(OBJDIR)
	$(RM) $(TARGET) $(OBJDIR)/*

