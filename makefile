exec_dir				=	./bin
obj_dir 				= 	./build
util_dir				=	./util
source_dir				=	./src
packet_pars_dir			=	./src/parsing_packet
config_pars_dir			=	./src/parsing_config
round_rr_dir			=	./src/round_robin
VPATH 					= 	$(source_dir) \
							$(util_dir)	\
							$(packet_pars_dir) \
							$(config_pars_dir) \
							$(round_rr_dir)
sources 				=	main.c \
						   	token.c \
						   	file_read.c \
							parsing_packet.c \
							linkedList_packet.c \
							init_packet.c \
							finalize_packet.c \
						   	parsing_config.c \
						   	linkedList_config.c \
						   	init_config.c \
						   	finalize_config.c \
						   	round_robin.c

objects 			= $(foreach src,$(sources),$(obj_dir)/$(src:.c=.o))

all : $(objects) exec

exec : $(objects)
	gcc -o $(exec_dir)/exec $^			# Using Automatic Expanded Variable
	
clean :
	rm $(obj_dir)/* $(exec_dir)/*
	
$(obj_dir)/%.o : %.c
	echo "Starting Pattern Rules ..."
	@test -d $(obj_dir) || mkdir -p $(obj_dir)
#	$(CC) -MMD -MF $*.d $(CFLAGS) -o $@ -c $<
	$(CC) -o $@ -c $<
