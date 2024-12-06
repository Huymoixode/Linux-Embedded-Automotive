tg ?= main.c
OUTPUT = $(basename $(tg))
all: $(OUTPUT)

$(OUTPUT):$(tg)
	gcc $< -o $@
clean: 
	rm -rf *.o $(OUTPUT)