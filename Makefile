# Roll No: MT25061
# PA02 – Analysis of Network I/O primitives

CC = gcc
CFLAGS = -O2 -Wall -pthread

# Targets
A1_SERVER = server_a1
A1_CLIENT = client_a1

A2_SERVER = server_a2
A2_CLIENT = client_a2

A3_SERVER = server_a3
A3_CLIENT = client_a3

# Default target
all: a1 a2 a3

# -------- Part A1 (Two-copy: send/recv) --------
a1: $(A1_SERVER) $(A1_CLIENT)

$(A1_SERVER): MT25061_Part_A1_Server.c
	$(CC) $(CFLAGS) -o $(A1_SERVER) MT25061_Part_A1_Server.c

$(A1_CLIENT): MT25061_Part_A1_Client.c
	$(CC) $(CFLAGS) -o $(A1_CLIENT) MT25061_Part_A1_Client.c

# -------- Part A2 (One-copy: sendmsg) --------
a2: $(A2_SERVER) $(A2_CLIENT)

$(A2_SERVER): MT25061_Part_A2_Server.c
	$(CC) $(CFLAGS) -o $(A2_SERVER) MT25061_Part_A2_Server.c

$(A2_CLIENT): MT25061_Part_A2_Client.c
	$(CC) $(CFLAGS) -o $(A2_CLIENT) MT25061_Part_A2_Client.c

# -------- Part A3 (Zero-copy: MSG_ZEROCOPY) --------
a3: $(A3_SERVER) $(A3_CLIENT)

$(A3_SERVER): MT25061_Part_A3_Server.c
	$(CC) $(CFLAGS) -o $(A3_SERVER) MT25061_Part_A3_Server.c

$(A3_CLIENT): MT25061_Part_A3_Client.c
	$(CC) $(CFLAGS) -o $(A3_CLIENT) MT25061_Part_A3_Client.c

# -------- Clean --------
clean:
	rm -f server_a1 client_a1 \
	      server_a2 client_a2 \
	      server_a3 client_a3 \
	      *.o *.png *.txt 

