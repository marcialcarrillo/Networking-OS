all: Lector Emitter

# Client:
# 	g++ -Wall -Wextra -o client clientTest.cpp
Lector:
	g++ -Wall -Wextra -o lector Lector.cpp Mailbox.cpp 
Emitter:
	g++ -Wall -Wextra -o emitter Emitter.cpp Mailbox.cpp -lpthread
