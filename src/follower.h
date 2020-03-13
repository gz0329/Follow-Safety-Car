#ifndef __FOLLOWER_AGENT__H
#define __FOLLOWER_AGENT__H 

#include "enviro.h"

using namespace enviro;
using namespace std;

class FollowerController : public Process, public AgentInterface {

    public:
    // Initialize the follower controller
    FollowerController() : Process(), AgentInterface() {}

    // Watch location update from the previous agent
    // Watch location update from the leader
    // Watch user click event. Self removed if clicked.
    void init() 
    {
        id = AgentInterface::id(); 

        watch( "location update" + to_string( id - 1 ), [this](Event e){
            goal_x = e.value()["x"];
            goal_y = e.value()["y"];
            goal_v = e.value()["v"];
            goal_r = e.value()["r"];
            goal_a = e.value()["a"];
        });

        watch("leader move forward", [this](Event e){
            turn_x = e.value()["x"];
            turn_y = e.value()["y"];
            go_turn = true;
        });

        watch("agent_click", [this](Event e){
            if ( e.value()["id"] == id )
            {
                remove_agent( id );
            }
        });
    }
    
    // No need to turn at beginning
    void start() { go_turn = false; }

    // Check if it is the right spot to make a turn
    bool is_turn_point()
    {   
        double x_ = AgentInterface::position().x;
        double y_ = AgentInterface::position().y;
        return ( x_ == turn_x ) && ( y_ == turn_y );
    }

    // Updated current location according to sensor value
    // Updated current location to agent behind.
    void update() 
    {
        double x_ = AgentInterface::position().x;
        double y_ = AgentInterface::position().y;
        double v_x = AgentInterface::velocity().x;
        double v_y = AgentInterface::velocity().y;
        double v_ = sqrt( v_x * v_x + v_y * v_y );
        double r_ = AgentInterface::angular_velocity();
        double a_ = AgentInterface::angle();
        
        emit( Event( "location update" + to_string( id ), 
                    { 
                        { "x", x_ }, 
                        { "y", y_ }, 
                        { "v", v_ }, 
                        { "r", r_ },
                        { "a", a_ },
                    } ) );
                
        if ( sensor_value( 0 ) > 30 
            && sensor_value( 1 ) > 30
            && sensor_value( 2 ) > 30
            && sensor_value( 3 ) > 30
            && sensor_value( 4 ) > 30  )
        {
            if( go_turn )
            {
                move_toward( turn_x, turn_y, 80, 40 );
                go_turn = !is_turn_point();
            }else 
            {
                move_toward( goal_x, goal_y, 80, 40 );
            }     
        } else if ( sensor_value( 0 ) <= 30 
            && sensor_value( 1 ) > 30 
            && sensor_value( 2 ) > 30 
            && sensor_value( 3 ) > 30
            && sensor_value( 4 ) > 30 ) 
        {
            damp_movement();
        } else if ( sensor_value( 0 ) <= 30 && sensor_value( 5 ) > sensor_value( 6 ) )
        {
            track_velocity( 0, -5  );
        } else if ( sensor_value( 0 ) <= 30 && sensor_value( 5 ) < sensor_value( 6 ) )
        {
            track_velocity( 0, 5  );
        } else if ( sqrt( ( x_ - goal_x )*( x_ - goal_x ) + ( y_ - goal_y )*( y_ - goal_y ) > 50.0 ) )
        {
            track_velocity( 0, 10  );
        }
        else
        {
            double safe_distance = sensor_value( 1 ) < sensor_value( 2 ) ? sensor_value( 1 ) : sensor_value( 2 );
            safe_distance -= 25; 
            double safe_speed = safe_distance > 0 ? safe_distance / 20 : 0;
            double r =  goal_r > 0 ? ( 5 + goal_r ) : ( -5 - goal_r );
            if ( goal_r == 0 ) r = 0;
            track_velocity( safe_speed, r );
        }
        
    }
    void stop() {}
    double goal_x, goal_y, goal_v, goal_r, goal_a;
    double turn_x, turn_y;
    bool go_turn;
    int id;
};

class Follower : public Agent {
    public:
    Follower(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    FollowerController c;
};

DECLARE_INTERFACE(Follower)

#endif