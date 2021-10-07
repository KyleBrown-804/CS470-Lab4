# CS470-Lab4

[Compiling & Execution]
 
    [Server]
    To compile and run this program an instance of the hotel server must first be live for clients to interact with it. In order to compile and run the hotel server use the
    command:
    'g++ -o hotel utilities.cpp Server.cpp -pthread' 
    and then to run: './hotel <port> <hotel floors> <rooms per floor>' for example './hotel 16000 5 5' would create a hotel which runs on port 16000 and has 5 floors with 5 rooms
    per floor.
 
    [Client]
    Likewise the client program must be compiled first using the command: 'g++ -o agency utilities.cpp Client.cpp -pthread'
    From there the client may be ran in two modes, manual or automatic following the following formats:
    
        - Manual -
        './agency <host ip address> <port> <manual>'

        - Automatic -
        './agency <host ip address> <port> <automatic>'
        
        - Examples -
        './agency 127.0.0.1 16000 manual'
        './agency 127.0.0.1 16000 automatic'
    
    Note that the ip address and port must match what the server is running on.
    
[Terminal Output]

    [Server]
        When the server is live you will be shown a visual display of the Hotel's current occupancy by a grid of 0's and 1's which looks like (for a 5x5):

        [ ---------- Server Live ---------- ]

        [ --- Hotel Occupancy ---]
        0 0 0 0 0
        0 0 0 0 0
        0 0 0 0 0
        0 0 0 0 0
        0 0 0 0 0
        [ -----------------------]

        On every connection established or connection lost event, an appropriate notice will be given while also specifies which client process ID it pertains to. For example:

        [Server] A connection has been established with [Client #159]
        [Server] Current number of rooms availible is 25
        
        or
        [Server] Connection to [Client #159] was lost...
        
        on every hotel room booking request the client's request will display with a server response stating if the booking was successful (requiring the room is not already
        booked). For example:
        
        [Client #160] would like to request Floor 4, Room 4
        [Server] Successfully booked Floor 4, Room 4 for [Client #160]
        
        or 
        [Client #164] would like to request Floor 2, Room 3
        [Server] Could not book Floor 2, Room 3 for [Client #164], it's already taken
        
    [Clients]
        For a manual client you will be repeatedly asked to choose a floor number and room number to book until the hotel is completely filled up. This interaction is labeled 
        with [Server] and [Client #PID] in order to make the communication between requests more clear. This may look like the following for example:

            [Server] Our hotel currently has 18 rooms available.
            We have 5 floors and 5 rooms per floor, for a total of 25 suites!

            What floor would you like? (Choose between 1-5)
            2

            What room number on that floor would you like? (Choose between 1-5)
            3

            [Client #164] sending reservation for Floor 2, Room 3 to the server
            [Server] Unfortunately we could not book your request, Floor 2, Room 3 is already reserved.
            [Client #164] There are currently 18 rooms left available
            -------------------------------------------------------------------
            What floor would you like? (Choose between 1-5)
    
        An automatic client will continuously keep booking rooms at random until the hotel is completely filled up and wait a random amount of time between 1-5 seconds after
        each request. This behavior might look like:
        
            [Server] Unfortunately we could not book your request, Floor 2, Room 3 is already reserved.
            [Client #182] There are currently 16 rooms left available
            [Client #182] (Automatic Mode): Now sleeping for 5 seconds...
            -------------------------------------------------------------------
            [Server] We have successfully booked your stay at Floor 5, Room 2! We look forward to your visit
            [Client #182] There are currently 15 rooms left available
            [Client #182] (Automatic Mode): Now sleeping for 1 seconds...
            -------------------------------------------------------------------
            [Server] We have successfully booked your stay at Floor 1, Room 1! We look forward to your visit
            [Client #182] There are currently 14 rooms left available
            [Client #182] (Automatic Mode): Now sleeping for 4 seconds...
            -------------------------------------------------------------------
    
    After every booking the server will refresh the hotel occupancy display to show 1's with booked rooms and 0's with unbooked. The first floor is displayed to match visually
    the bottom most floor and rooms go from left to right.
    
        [ --- Hotel Occupancy ---]
        0 1 1 0 1   <---- floor 5 room 5 booked
        0 0 1 1 0
        1 0 1 0 0
        0 0 1 1 1
        1 0 0 0 0   <---- floor 1 room 1 booked
        [ -----------------------]
        
    [Error Outputs]
    Upon any encountered errors either client or server side, the appropriate application will be notified. For example if the server is offline Clients will make 4 attempts to
    create a socket connection waiting 3 seconds after each attempt such as:
    
        [Client #184] Error: Connection refused
        [Client #184] Retrying connection in 3 seconds...
        [Client #184] Error: Connection refused
        [Client #184] Retrying connection in 3 seconds...
        [Client #184] Error: Connection refused
        [Client #184] Retrying connection in 3 seconds...
        [Client #184] Error: Connection refused
        4 Attempts were made to connect and all failed, please check that the server is online
    
    [Completion]
    When the hotel server is completely booked it will send a closing notification to all clients which will then close their socket connections and end the program.
    
        [Client] There are no more rooms available the connection will now close
