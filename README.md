# CS118 Project 1

This is the repo for spring 2022 cs118 project 1.

## Makefile

This provides a couple make targets for things.
By default (all target), it makes the `server` executables.

It provides a `clean` target, and `tarball` target to create the submission file as well.

You will need to modify the `Makefile` USERID to add your userid for the `.tar.gz` turn-in at the top of the file.

## Academic Integrity Note

You are encouraged to host your code in private repositories on [GitHub](https://github.com/), [GitLab](https://gitlab.com), or other places.  At the same time, you are PROHIBITED to make your code for the class project public during the class or any time after the class.  If you do so, you will be violating academic honestly policy that you have signed, as well as the student code of conduct and be subject to serious sanctions.

## Provided Files

`server.c` is the entry points for the server part of the project.

## Authors

Justin Kyle Chang, jchangz01@g.ucla.edu, 205388820
Oliver De Visser, oliverdevisser@g.ucla.edu,605405016

## High Level Design

To setup our HTTP server, we create a socket file decriptor, set general socket options, bind the socket to port 15635 (specified port from project specs), and begin listening for connections. Our server then waits for clients to send requests. When a client sends a request, our server parses the request body for the requested file name. This filename is located on the first line of the request message following the HTTP request type. We filter out the filename for any URL encodings (e.g. %20 -> SPACE and %25 -> %). We check if the file exists within our server; in the case that it does not exist, we send a "404 Not Found" error. Otherwise; we send a "200 OK" status code, the content-length of the requested file, the content-type of the requested file (based on file extension), and the requested file. Our server currently only supports the file types .html, .txt, .jpg, .png, .gif and files with no extension (will send the binary data of the file). 

## Challenges

A challenge we had was with the filename of the HTTP request not being the same as how we typed it in the web browser. For example, when typing a space, it would get changed to a '%20' so we had to add a function that would filter these out and make them spaces. We also had a problem when including '%' characters, as the web browser in Ubuntu would remove the rest of the filename after the '%'. We looked up the URL encoding for '%', which was '%25', so we replaced all '%' with '%25', and it worked. After that, when just typing '%', it would work anyways so we weren't sure if this was due to our code or something with HTTP's behavior that we were unaware of.

Another challenge we had was with being able to distinguish the file types and send the proper file type back to the client. We had trouble getting the right content-type for the header file, which we managed to figure out by making a function that gets the file extension. From there, being able to have the server check for the requested file and return the requested file was challenging but we could figure it out throught the textbook and different API pages.

A final challenge we had was with running the VirtualBox with the proper Ubuntu OS. When trying to start the remote machine, we got the error message that we didn't have the proper kernel installed, and that the virtual machine would unexpectedly quit during startup. This caused us a lot of trouble and time and we fixed it by looking through different pages online and trying different things. Uninstalling and reinstalling VirtualBox ended up solving a lot of our issues with that problem.

## Acknowledgements

[TA Simple Server Example](https://github.com/ZhaoweiTan/CS118-S22-Dis1B/blob/main/week1/simple-server-client/server.c)

[Common MIME Types](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types)

[Man Pages for Socket API](https://man7.org/linux/man-pages/index.html)