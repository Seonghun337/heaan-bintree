RM = rm -rf
CC = g++
CFLAGS = -std=c++11
TERGET = main.cpp
OUTPUT = output

HEAAN_DIR = ../HEAAN/

FOLDER_NAMES = DEFAULT BOOT PLAIN ENC TEST

DEFAULT_DIR = 
DEFAULT_FILES = PrintUtils MaskingGenerator

BOOT_DIR = ../bootsrc/
BOOT_FILES = common new_bootstrapping

PLAIN_DIR = plain/
PLAIN_FILES = CyclicArray PlainSorting

ENC_DIR = enc/
ENC_FILES = EncAlgorithm BootScheme BootAlgorithm EncSorting

TEST_DIR = test/
TEST_FILES = TestMask TestPlain TestEnc TestBoot TestSort

OBJS = $(foreach NAME, $(FOLDER_NAMES), $(addprefix $($(NAME)_DIR), $(addsuffix .o, $($(NAME)_FILES))))

LOCAL_LIBS = -lntl -lgmp -lm -lpthread
BOOT_LIBS = ../eigen
LIBS = -L/usr/local/lib

# All Target
all : main run


# g++ main.cpp -o output -L/usr/local/lib -I ../eigen ../HEAAN/lib/libHEAAN.a -lntl -lgmp -lm -std=c++11 -O3 -I/../../src -pthread
# Tool invocations
main : $(OBJS)
	@echo 'Building target: $@'
	$(CC) $(TERGET) -o $(OUTPUT) $(OBJS) $(LIBS) $(CFLAGS) -I $(BOOT_LIBS) $(HEAAN_DIR)lib/libHEAAN.a $(LOCAL_LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

.cpp.o :
	@echo 'Building file: $<'
	$(CC) -c $< -o $@ $(CFLAGS) -I $(BOOT_LIBS)
	@echo 'Finished building: $<'
	@echo ' '

# Other Targets
clean :
	$(RM) $(OUTPUT) $(OBJS)

run :
	./$(OUTPUT)