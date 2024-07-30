# Makefile para mi_proyecto

# El compilador
CXX = g++

# Opciones de compilación
CXXFLAGS = -std=c++17 -I/usr/include

# Opciones de enlace
LDFLAGS = 

# Directorios de Boost
BOOST_DIR = /usr/include
BOOST_LIB_DIR = /usr/lib

# Nombre del ejecutable
TARGET = mi_proyecto

# Archivos fuente
SRCS = main.cpp

# Archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Regla para construir el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) -lboost_system

# Regla para construir archivos objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET) $(OBJS)
