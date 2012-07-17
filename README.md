TheIceRink
==========

First experiment with asynchronous networking using Boost.Asio

I made this a while ago as a fun experiment while I was working at Inlight.

You give yourself a name and choose a color, then are able to enter a server with other players. You can then float around with some half-assed simulated physics (hence the name Ice Rink), and other players will be able to see you as a colored square with your name.

After programming this, I tested it out in the office with my coworkers and it worked reasonably well! There was a bit of lag, but I think that was mostly due to me writing poor quality networking code. After all, this is my first try... There are actually quite a lot of questionable programming practices taken in this project. Oh well, that's how you learn I guess.

I have a Windows build available in the build folder, which includes both the server and client software and a template configuration file.

Right now I need to write some SCons scripts to make this easier to build for non-Windows platforms. (This was originally made using Visual Studio, which is not available for Linux.) The source code is a huge mess, so I need to sort that out sometime before writing my SCons scripts. Also, I think I might have used a development build of SFML. That means I'll likely need to do some rafactoring.

It was a lot of fun writing code with Asio. I need to do more of that!
