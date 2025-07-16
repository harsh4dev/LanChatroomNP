harsh2: # 💬 LAN Chat Room with File Transfer using Winsock

> 📡 A real-time multi-user chat and file sharing system over a local area network (LAN), built using C++ and Winsock on Windows.

---

## 🛠️ Features

- ✅ **Multi-User Chat:** Real-time message exchange over LAN
- 📂 **File Transfer Support:** Send and receive files via server relay
- 💬 **Simple Commands:** `/file <filename>`, `/list`, `/exit`
- 🧵 **Multi-Threaded Server:** Handles multiple clients simultaneously
- 🎨 **Colored Console Output:** Sender, receiver, and events are color-coded
- ⏱️ **Message Timestamps:** Every message has a [HH:MM] prefix
- 🙋‍♂️ **You-style Echo:** Sender sees their own messages as `You: message`
- 🖥️ **Console-Based Interface:** Clean, no-GUI terminal chat

---

## 🖼️ Architecture

**Client-Server Model using TCP Sockets (Winsock2):**

```
+-----------+       +-------------+       +-----------+
|  Client 1 | <---> |   Server    | <---> |  Client 2 |
+-----harsh2: ------+       +-------------+       +-----------+
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
| Compiler/IDE      | VS Code / Code::Blocks |
| Protocol          | TCP/IP         |
| Version Control   | Git            |

---

## 📂 Project Structure

```
/LANChatRoom/
├── client.cpp       # Chat + file client (with color, timestamps, etc.)
├── server.cpp       # Multi-threaded server (logs all messages)
├── common.h         # Shared constants & headers
├── README.md        # This file
└── sample.txt       # Sample file to test transfer
```

---

## 🚀 How to Run

### �harsh2: ��� Prerequisites
- Windows machine
- C++ compiler (MinGW, MSVC, Code::Blocks)
- Winsock2 (already in Windows SDK)

### ✅ Compile

**VS Code or Terminal:**
```bash
g++ server.cpp -o server.exe -lws2_32
g++ client.cpp -o client.exe -lws2_32
```

**Code::Blocks:**
- Add `ws2_32` to linker settings
- Add all .cpp and .h files to project

### ✅ Run

1. Run `server.exe` in one terminal
2. Run `client.exe` in 2+ terminals
3. Enter usernames and start chatting!

---

## 💬 Commands

| Command              | Description                       |
|----------------------|-----------------------------------|
| `/file filename.txt` | Send a file to other users        |
| `/list`              | List currently connected users    |
| `/exit`              | Disconnect from server            |

---

## 🧪 Demo Scenario

1. Start `server.exe`
2. Run `client.exe` in two terminals
3. Enter usernames
4. Type messages and watch chat appear with colors + timestamps
5. Send:
   ```bashharsh2: 
   /file sample.txt
   /list
   /exit
   ```

---

## 🙏 Acknowledgment

Built as a final project for the **Network Programming** course at **Nepal College of Information Technology**, under the guidance of **Mr. Madan Kadariya**.

---

## 🧠 Authors

- 👨‍💻 Harsh Chaudhary Kalwar - `harsh.221715@ncit.edu.np`
- 👨‍💻 Pranil Poudel - `pranil.221734@ncit.edu.np`

---

## 💥 We Didn’t Just Use Sockets — We Built Communication.
> "From raw bytes to real messages — this chat system is ours." 💬⚡
FILE_END
