# 💬 LAN Chat Room with File Transfer using Winsock

> 📡 A real-time multi-user chat and file sharing system over a local area network (LAN), built using C++ and Winsock on Windows.

---

## 🛠️ Features

- ✅ **Multi-User Chat:** Real-time message exchange over LAN
- 📂 **File Transfer Support:** Send and receive files via server relay
- 🧵 **Multi-Threaded Server:** Handles multiple clients simultaneously
- 💬 **Simple Command Interface:**
  - `/file <filename>` — send a file
  - `/list` — view online users
  - `/exit` — leave the chat
- 🔐 **Optional Encryption (Pluggable):** Can add XOR/Caesar cipher
- 🖥️ **Console-Based Interface:** No GUI, just clean terminal interaction

---

## 🖼️ Architecture

**Client-Server Model using TCP Sockets (Winsock2):**

```
+-----------+       +-------------+       +-----------+
|  Client 1 | <---> |   Server    | <---> |  Client 2 |
+-----------+       +-------------+       +-----------+
        \                                   /
         \------------------+--------------/
                            |
                         File Relay
```

---

## ⚙️ Tech Stack

| Component         | Technology     |
|------------------|----------------|
| Language          | C++            |
| Network API       | Winsock2       |
| Platform          | Windows 11     |
| UI                | Console-Based  |
| Compiler/IDE      | Code::Blocks   |
| Protocol          | TCP/IP         |
| Version Control   | Git            |

---

## 📂 Project Structure

```
/LANChatRoom/
├── client.cpp       # Chat + file client
├── server.cpp       # Multi-threaded server
├── common.h         # Shared constants & headers
└── README.md        # This file
```

---

## 🚀 How to Run

### 🧱 Prerequisites
- Windows machine
- C++ compiler (Code::Blocks / MinGW / MSVC)
- Winsock2 (included in Windows SDK)

### ✅ Compile

1. Open project in Code::Blocks **or** compile manually:

```bash
g++ server.cpp -o server -lws2_32
g++ client.cpp -o client -lws2_32
```

2. Make sure all `.cpp` files and `common.h` are in the same folder.

### ✅ Run

1. Open one terminal, run the **server**:
```bash
server.exe
```

2. Open 2+ more terminals, run the **clients**:
```bash
client.exe
```

3. Enter your username, start chatting!

---

## 📡 Commands

| Command              | Description                       |
|----------------------|-----------------------------------|
| `/file filename.txt` | Send a file to other users        |
| `/list`              | List currently connected users    |
| `/exit`              | Disconnect from server            |

---

## 📁 Demo Scenario

1. Start `server.exe`
2. Run `client.exe` in two terminals
3. Enter usernames
4. Type messages and watch real-time chat
5. Try:
   ```bash
   /file sample.txt
   /list
   /exit
   ```

---

## 🔐 Optional Enhancements

- ✅ Add XOR encryption for message/file security
- ✅ Add username validation or color-coded output
- ✅ Add file save confirmation on client side

---

## 🙏 Acknowledgment

Built as a final project for the **Network Programming** course at **Nepal College of Information Technology**, under the guidance of **Mr. Madan Kadariya**.

---

## 🧠 Authors

- 👨‍💻 Harsh Chaudhary Kalwar - `harsh.221715@ncit.edu.np`
- 👨‍💻 Pranil Poudel - `pranil.221734@ncit.edu.np`

---

## 💥 Let’s Chat Like It’s 1999
> "We didn’t just learn sockets, we used them to connect people — byte by byte." 💬⚡
