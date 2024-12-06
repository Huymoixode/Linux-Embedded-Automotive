CC = gcc
TARGET = word
TARGET_DIR = output
SO_TARGET = libhello.so
A_TARGET = libhello.a
OBJ =word.o
%.o : %.c
	$(CC) -c $^ -o $@
buildso:$(OBJ) $(SO_TARGET) $(TARGET_DIR)
	@mkdir -p $(TARGET_DIR)
	$(CC) $< -L. -lhello -o $(TARGET_DIR)/$(TARGET)
builda: $(OBJ) $(A_TARGET)
	$(CC) $< -L. -lhello -o $(TARGET)
$(SO_TARGET): hello.c
	$(CC) $< -o $@ -shared
$(A_TARGET): hello.o
	ar rcs $@ $^
clean:
	rm -rf *.o *.so *.a $(TARGET)