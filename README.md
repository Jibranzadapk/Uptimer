# ⚡ Uptimer

> A smarter uptime tracker for Linux — not just *now*, but *history*.

---

## 🎯 What is Uptimer?

`uptimer` is a lightweight, fast, and native C++ tool that goes beyond the traditional `uptime` command.

Instead of only showing your current session uptime, **Uptimer tracks, stores, and logs uptime across sessions**, giving you a persistent history of your system activity.

---

## 🚀 Why Uptimer?

The default Linux `uptime` command only tells you:

* How long your system has been running **right now**

But once you reboot?

💥 That data is gone.

---

### 🧠 Core Features:

* 📦 Stores uptime data across sessions
* 🧾 Maintains structured logs
* 🔎 Lets you revisit previous sessions
* 🆔 Identifies sessions with unique IDs
* 📊 Builds a history of system usage over time

---

## 🖥️ Preview

![Uptimer](https://github.com/user-attachments/assets/0a3e10f5-b0f7-42e4-abb3-30ee54f1ef24)


---

## 📦 Installation

### 🔹 Option 1: Using `.deb` (Recommended) For Debian, Ubuntu etc

```bash
sudo dpkg -i uptimer.deb
```

---

### 🔹 Option 2: Manual Install Script ( Simple way )

```bash
git clone https://github.com/Jibranzadapk/Uptimer.git
cd Uptimer
chmod +x install
./install
```

---

### 🔹 Option 3: Build from Source

```bash
git clone https://github.com/Jibranzadapk/Uptimer.git
cd Uptimer
make
./uptimer
```

---

## 📁 Project Structure

```text
Uptimer/
├── src/
│   ├── main.cpp        # Entry point
│   ├── uptime.cpp      # Core uptime logic
│   ├── dashboard.cpp   # Output & display
│   ├── logger.cpp      # Logging system
│
├── include/            # Header files
├── install             # Install script
├── Makefile            # Build system
└── uptimer.deb         # Package (optional)
```

---

## 🤝 Contributing

Contributions are welcome!

Feel free to:

* Fork the repo
* Open issues
* Submit pull requests

---

## 📜 License

This project is open-source and available under the MIT License.

---

## 💬 Author

Made with ❤️ by **Jibran**

---

## ⭐ Support

If you like this project:

* ⭐ Star the repo
* 🍴 Fork it
* 🧠 Share ideas

---

> “ Visit : WWW.JIBRANZADA.COM ”
