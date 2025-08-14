### 6
普通的断开网络可以重连，普通的ssh也可以做到
断开网络适配器两者都不能主动恢复连接
### 7
 -f      Requests ssh to go to background just before command execution.  This is useful if ssh is going to ask for pass‐
		 words or passphrases, but the user wants it in the background.  This implies -n.  The recommended way to start
		 X11 programs at a remote site is with something like ssh -f host xterm.

		 If the ExitOnForwardFailure configuration option is set to “yes”, then a client started with -f will wait for all
		 remote port forwards to be successfully established before placing itself in the background.  Refer to the de‐
		 scription of ForkAfterAuthentication in ssh_config(5) for details.

 -N      Do not execute a remote command.  This is useful for just forwarding ports.  Refer to the description of
		 SessionType in ssh_config(5) for details.

TCP FORWARDING
     Forwarding of arbitrary TCP connections over a secure channel can be specified either on the command line or in a
     configuration file.  One possible application of TCP forwarding is a secure connection to a mail server; another is going
     through firewalls.

     In the example below, we look at encrypting communication for an IRC client, even though the IRC server it connects to does not directly support encrypted communication.  This works as follows: the user connects to the remote host using ssh,
     specifying the ports to be used to forward the connection.  After that it is possible to start the program locally, and ssh will encrypt and forward the connection to the remote server.

     The following example tunnels an IRC session from the client to an IRC server at “server.example.com”, joining channel
     “#users”, nickname “pinky”, using the standard IRC port, 6667:

         $ ssh -f -L 6667:localhost:6667 server.example.com sleep 10
         $ irc -c '#users' pinky IRC/127.0.0.1

     The -f option backgrounds ssh and the remote command “sleep 10” is specified to allow an amount of time (10 seconds, in the example) to start the program which is going to use the tunnel.  If no connections are made within the time specified, ssh will exit.

-N就是不执行remote处的命令，有助于端口转发。 -f就是让ssh在执行命令前转到后台。
所以可以用 ssh -Nf -L 9999:localhost:8888 vm
