## Multi-Client Chat Application

**Version:** v1.0.0
**Status:** Stable

### About The Project

This project is a Linux-based multi-client chat application developed in C using TCP/IP socket programming and POSIX threads.

The application follows a client-server architecture where multiple clients can connect to a central server and communicate in real time. Each client joins the chat using a username, and messages are broadcast to all connected users.

The primary objective of this project was to gain practical experience with Linux System Programming concepts such as socket programming, multithreading, synchronization, and network communication.

---

### Features

* Multi-client support
* Real-time message broadcasting
* Username-based communication
* Concurrent client handling using POSIX threads
* Thread-safe client management using mutexes
* Join and leave notifications
* Graceful client disconnection using `/quit`

---

### Technologies Used

* C Programming Language
* Linux System Programming
* TCP/IP Socket Programming
* POSIX Threads (pthread)
* GCC Compiler

---

### Project Structure

```text
.
├── header.h
├── Server
│   └── server.c
└── Client
    └── client.c
```

---

### Compilation

#### Server

```bash
gcc Server/server.c -I. -o server -lpthread
```

#### Client

```bash
gcc Client/client.c -I. -o client -lpthread
```

---

### Running

Start the server:

```bash
./server
```

Start the client:

```bash
./client
```

Enter a username when prompted and begin chatting.

---

### Future Enhancements

* Private messaging, User authentication, Chat rooms, File transfer support, Message logging and End-to-end encryption
---

#### Author

Darshan Gowda R
