# Multi-Client TCP Messaging Server in C

## Overview
This project is a custom-built, multi-client messaging system developed in C. Instead of relying on high-level frameworks, it is built directly on top of Linux system calls and TCP/IP sockets. It demonstrates a fundamental, under-the-hood understanding of network protocols, concurrent connection handling, and low-level file system management.

This project was developed to solidify my understanding of the OSI Transport Layer (Layer 4) and POSIX socket programming.

## Key Technical Features
*   **TCP/IP Socket Programming:** Implemented robust server-client architecture using `socket()`, `bind()`, `listen()`, `accept()`, and `connect()`.
*   **I/O Multiplexing:** Utilized the `select()` system call to handle multiple concurrent client connections asynchronously without the overhead of multithreading.
*   **Custom Application Protocol:** Designed a lightweight, integer-based routing protocol to parse incoming data streams and route messages to specific user IDs.
*   **Dynamic File System Management:** Uses Linux `<sys/stat.h>` to dynamically generate user-specific directories and manage persistent text-based databases for user credentials and friend lists.

## Architecture
1.  **Server (`server.c`):** Binds to port `8080` and listens for incoming connections. It maintains an array of active client file descriptors and uses `select()` to read incoming messages without blocking. It also handles writing and reading from the `users/` directory structure.
2.  **Client (`client.c`):** Connects to the server with a specific user ID. Provides a terminal-based interface to send messages, view inbox, list friends, and add new friends.

## How to Compile and Run

### Prerequisites
*   A Linux/Unix-based environment (or WSL on Windows).
*   GCC Compiler.

### Compilation
Compile the server and client source files separately:
```bash
gcc server.c -o server
gcc client.c -o client
```

### Execution
1. Start the server first. It will automatically create the necessary `users` directories and `users.txt` file.
```bash
./server
```

2. Open a new terminal instance and start a client by passing a numeric Client ID as an argument.
```bash
./client 1
```

3. Open another terminal instance to start a second client to test messaging.
```bash
./client 2
```

## Usage Example
Once connected, the client interface provides the following options:
1. **Send a message:** Enter the target User ID and your message.
2. **Read messages:** Fetch unread messages from the server.
3. **List friends:** View the list of IDs added as friends.
4. **Add a new friend:** Add a target User ID to your local friend list.
