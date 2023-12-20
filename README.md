# lkby
**Lkby** is a library designed for creating keyboard-sensitive applications without requiring a background window manager. Its architecture follows the **client-server model**, utilizing **UNIX** sockets to establish communication between the server and clients.

# Installation Requirements

Before diving into the **lkby**, make sure to install the necessary packages:

```bash
sudo apt-get install build-essential git gcc
```

# Download & Build

Get started by cloning the repository and navigating to the lkby folder:
```
git clone https://github.com/constarg/lkby.git
cd lkby/
```
Now, let's build the program:

```
mkdir build
cd build/
cmake ../
make
```

After this the program will be ready to run.

# Doc
The documentation for the library is accessible in the **"docs"** folder as well as on the wiki page of this repository.

# Test
An illustrative instance of employing this library is available in the **test** folder.
