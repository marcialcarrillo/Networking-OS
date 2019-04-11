all: Emisor Lector

Emisor: Emisor.cpp
	g++ -Wall -Wextra -o emisor Emisor.cpp Mailbox.cpp -fno-stack-protector -pthread
Lector: Lector.cpp
	g++ -Wall -Wextra -o lector Lector.cpp Mailbox.cpp

