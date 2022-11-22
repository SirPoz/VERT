# TW-Mailer Basic

## Building

### Requirements

This project requires UUID - Library. On Debian-based systems, you can install it with:

    sudo apt install uuid-dev

### Building

Build by using `make` to build the desired target.

Example:

```bash
> make          # Makes all targets
> make server   # Makes server
> make client   # Makes client
> make clean    # Deletes executables
```

## How to use TW-Mailer

### TW-Mailer Server

The TW-Mailer requires a server to run. The server is responsible for handling the TW-Mailer protocol and sending the emails to the recipients. The server can be started by running the `twmailer-server` executable.

```bash
> ./twmailer-server <PORT> <MAILSPOOL DIRECTORY>
```

`<PORT>` is the port the server will listen on. `<MAILSPOOL DIRECTORY>` is the directory where the server will store the emails.

Example:

```bash
> ./twmailer-server 10000 ./mailspool
```

</br>

---

</br>

### TW-Mailer Client

The TW-Mailer client is used to send, list, read and delete emails on the server. The client can be started by running the `twmailer-client` executable.

```bash
> ./twmailer-client <SERVER ADDRESS> <SERVER PORT>
```

`<SERVER ADDRESS>` is the address of the server. `<SERVER PORT>` is the port the server is listening on.

Example:

```bash
> ./twmailer-client localhost 10000
```

</br>
The client will then prompt you for a command. The following commands are available:

#### Send email

```bash
Please enter your commands...
>> SEND
Enter your Username: if21b248
Enter Receiver: if21b241
Enter Subject (max. 80 char): This is a test email
Enter Message:
This is a test email.
This is the second line of the message.
.
```

Note: The message is terminated by a `.` on a line by itself.

#### List emails

```bash
Please enter your commands...
>> LIST
Enter your Username: if21b248

<< OK
1: if21b241: This is a test email
```

#### Read email

```bash
Please enter your commands...
>> READ
Enter your Username: if21b248
Enter Email ID: 1

<< OK
From: if21b241
Subject: This is a test email
Time: 2021-06-01 12:00:00

This is a test email.
This is the second line of the message.
```

#### Delete email

```bash
Please enter your commands...
>> DEL
Enter your Username: if21b248
Enter Email ID: 1

<< OK
```

#### Quit

```bash
Please enter your commands...
>> QUIT
```

</br>

#### Error handling

If an error occurs, the client will print the error message and prompt you for a new command.
