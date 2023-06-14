all: udp_blaster
udp_blaster: udp_blaster.c
	$(CC) $< -o $@
