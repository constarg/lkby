# Getting started
This library is designed for keyboard-sensitive applications and employs the client-server
model to fulfill this purpose. In its architecture, the library runs a server with root 
privileges to access keyboard event files. The server can be launched by executing 
the command below in the terminal:
```bash
cd bin/
sudo ./lkby
```
Following this, the server is prepared to accept any incoming clients!.

# How to use the library
in this section, we'll explain the interface that the Lkby library offers to users.
## Structures
This library includes a crucial structure. This structure, in fact, contains all the 
information about the keystroke that is pressed, released, or autorepeated. 
The structure is defined as follows:
```c
struct
{
  char       *kb_name;    // The name of the keyboard that occure an event.
  __u16       kb_code;    // The code of the pressed/released key.
  lkby_status kb_status;  // The status of the event, if the key is pressed/relased
} lkby_trans_key;
```
This structure, in reality, is a member of a larger union, which includes two 
additional members. However, for the user utilizing this library, 
the focus is solely on this structure.
More specifically, the member kb_name of the structure holds a string
representing the name of the keyboard that triggered the key event. 
The kb_code member corresponds to the code of the keystroke in question.
The kb_status corresponds to the state of the keystroke, indicating whether 
it is released, pressed, or in autorepeat.

## Functions
The library also supplies a single function that manages all aspects related to 
connection and keystroke handling. The function signature is given below:
```c
int lkby_lib_establish_connection(const char *restrict name, void (*lkby_lib_callback)(lkby_info *))
```
This function's role is to attempt to establish a connection to the Lkby server. Additionally, it 
calls the callback function lkby_lib_callback for every keystroke received from the Lkby server to the client.

## Macros
Macros in C prove to be incredibly useful! That's why the Lkby library provides
a set of very handy macros, outlined above.

```C
#define LKBY_LIB_KEYBOARD_NAME(lkby_info)
```
**LKBY_LIB_KEYBOARD_NAME** macro function takes the lkby_info structure as an argument and translates 
it into a member of the lkby_info structure that corresponds to the name of 
the keyboard to which the pressed keystroke belongs.

```c
#define LKBY_LIB_KEYBOARD_CODE(lkby_info) 
```
**LKBY_LIB_KEYBOARD_CODE** macro function takes the lkby_info structure as an argument and translates 
it into a member of the lkby_info structure that corresponds to the code of 
the keystroke that is pressed/released.

```c
#define LKBY_LIB_KEYBOARD_STATUS(lkby_info)
```
**LKBY_LIB_KEYBOARD_STATUS** macro function takes the lkby_info structure as an argument and translates 
it into a member of the lkby_info structure that corresponds to the status of 
the keystroke (The status can be one of the three following: pressed, released or autorepeat).

```c
#define LKBY_LIB_KEY_RELEASED   0x0
```
**LKBY_LIB_KEY_RELEASED** macro serves the purpose of checking whether a keystroke is in the released state.

```c
#define LKBY_LIB_KEY_PRESSED    0x1
```
**LKBY_LIB_KEY_PRESSED** macro serves the purpose of checking whether a keystroke is in the pressed state.

```c
#define LKBY_LIB_KEY_AUTOREPEAT 0x2
```
**LKBY_LIB_KEY_AUTOREPEAT** macro serves the purpose of checking whether a keystroke is in the autorepeat state.

The following macro functions are identical to **LKBY_LIB_KEY_RELEASED**, **LKBY_LIB_KEY_PRESSED**, and 
**LKBY_LIB_KEY_AUTOREPEAT**, with the sole difference that they are function-like, returning false 
if the condition is invalid and true if it is valid.
```c
#define LKBY_LIB_IS_KEY_PRESSED(lkby_info) 
#define LKBY_LIB_IS_KEY_RELEASE(lkby_info) 
#define LKBY_LIB_IS_KEY_AUTOREPEAT(lkby_info) 
```
