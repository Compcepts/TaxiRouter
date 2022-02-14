# TAXIROUTER - a simple, extensible autonomous vehicle routing system

## ABOUT:
This project is designed as a proof-of-concept for simple WiFi-based vehicle 
routing. It uses a server/client model as a means to  coordinate  movement 
between a server and any number of vehicles in the network. The server code
is capable of being run on any Linux machine, possibly other OS's too with
some slight modification. The client code is made for Raspberry Pi's  
running Linux as well. 

## HOW TO USE:
If you are interested in running this project on your own, there are a few
steps you can take to do so:

1. Edit the server/defs/const.h file to include the number of carts, 
vertical roads, and horizontal roads you plan on using.
2. Make the pathfinder object file by running 'make' in the server
directory
3. Feel free to use the included cart designs or design your own
4. Install Raspbian or similar Linux distro onto RPi
5. Install Python3 and RPiO library
6. Edit cartdriver.py:
    1. To make a functioning cart, you only need to define the 'drive', 'turn_left', 'turn_right', 'turn_around', 'stop', and 'shutdown' functions.
    2. You can choose how you want to implement these functions with your choice of sensors and motors - just make sure you can guarantee proper movement along your map.
7. To run server, run the pathfinder object file
8. To run the cart, run the command 'python3 pythsock <cart_index>'
    
### NOTE:
When making your map, it needs to be made as a grid, and your cart must
be able to detect intersections while driving, then stop and return from
the drive function. Likwise it should be able to detect when it has
successfully turned and return from turn functions as well.

