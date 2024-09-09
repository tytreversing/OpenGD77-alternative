CC                = gcc
OBJS              = languages_builder.o
SRCS              = languages_builder.c
TARGET            = languages_builder

DIST_FILE         = Languages.zip

CFLAGS            = -Wall -O2
LDFLAGS           =
INCLUDES          = -I../
LDLIBS            =

.PHONY: all clean

.SUFFIXES: .o .c

%.o: %.c
	@echo "Compiling $< ..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $<


$(TARGET): $(OBJS)
	@echo "Linking $(OBJS) to $(TARGET) ..."
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)


all: $(OBJS) $(TARGET)


dist: clean $(TARGET)
	@echo "Running $(TARGET) ..."
	@rm -rf languages; mkdir -p languages
	(cd languages ; ../$(TARGET))
	@rm -f $(DIST_FILE)
	@echo "Compressing" *.gla "to $(DIST_FILE) ..."
	zip -r9 $(DIST_FILE) languages


check: clean all
	./$(TARGET) --check-languages


dist-clean: clean
	rm -rf languages


clean:
	rm -rf *~ *.o *.gla *.zip $(TARGET)
