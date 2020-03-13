EE520 Final Project
===

Follow Safety Car
---
In motorsport, a safety car is a car which limits the speed of competing cars on a racetrack during a caution period. 

With self-driving technology, safety car may be able to drive itself in the future.

This project simulates the scenario by using Elma and Enviro. User plays the role of the president of the race event who has the right to:
> Stop/Run the safety car.
> Enable/Disable the self-driving mode
> Eliminate competing cars.

ENVIRO and Elma links
===
If user is interested in how ENVIRO and Elma works, please feel free to use the following links:
The git repo for ENVIRO > https://github.com/klavinslab/enviro
The git repo for Elma > https://github.com/klavinslab/elma
Documents about Elma > http://klavinslab.org/elma

How to Run the Project
===
To run this project, users need to start the Docker container with ENVIRO pre-loaded into it, do:

```bash
docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:v1.3 bash
esm start
```

The above commands do the following:

- The `-p80:80 option maps *port* 80 from the Docker container to ports on your host computer. This should allow you to go to 
    > http://localhost
    with your web browser and see the ENVIRO client. It should say "Error: Failed to fetch. Is the server running?" at this point. 

- The `-p8765:8765` option allows the ENVIRO client to communicate with the `enviro` server (not yet running) via a *WebSocket*.

- The command `esm start` uses the <u>e</u>nviro <u>s</u>etup <u>m</u>anager to start a web server from which your browser can access the ENVRIO client. 

Once the steps above are completed, users should be able to see this error message:
```bash
Enviro Error: No connection. Is the server running? See here for help.
```

Next step is simply Make the project and start the enviro sever as follows.
```bash
make
enviro
```
Users can press `Ctrl-C` to stop the enviro server.

Key Challenges and Solutions
===
Turn Direction
---
Under self-driving mode, it is difficult to make sure that the safety car makes a correct turn. For example, if the safety car meets a corner and it always turn to the same side, maybe it is still facing the wall after making a turn. So I added multple sensors besides the robot and determine which way to turn based on distances to both sides.

Turn with Forward Velocity 
---
It is also difficult to simulate the turn of a real car. Because cars are not like robots, they don't stay still when make a turn. To make the simulation look more realistic, the angular speed is related to the forward speed. The forward speed is related to the value of front sensor. 

Followers' Path
---
To make sure that competing cars don't pass each other and follow the safety car in a relatively similar route, all the agents only follows the location of the previous agent. 

Features
===
Change the Look of the Robots
---
The shape of safety car and competing cars are defined in the file `defs/leader.json` and `defs/follower.json`
Users are welcomed to change the style, however, there is the risk of making the sensors stop working.
Here is the link to a website which can help users to decide the shape and coordinates:
https://www.math10.com/en/geometry/geogebra/geogebra.html

Stop/Run the Safety Car
---
As mentioned in the beginning, users have the right to stop the safety car and resume it. To do that, user can click the button on top right of the web browser. It has the label `Go/Stop`.
The simulation will start with `Go` status and once user click the button once, it will enter `Stop` status. Click again will resume the simulation.

Enable/Disable the Self-driving Mode
---
As mentioned in the beginning, users have the right to enable or disable the self-driving mode. To do that, user can click the button on top right of the web browser. It has the label `Auto/Manual`.
The simulation will start with `Auto` status and once user click the button once, it will enter `Manual` status.

User is able to use keyboard to control the movement of the leader:
`w` > move forward
`a` > turn left
`s` > move backward
`d` > turn right

Eliminate Race Car
---
As mentioned in the beginning, users also have the right to eliminate competing cars which are crashed. User can click the car and it will be removed. 

