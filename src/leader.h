#ifndef __LEADER_AGENT__H
#define __LEADER_AGENT__H 

#include "enviro.h"

using namespace enviro;
using namespace std;

class MovingForward : public State, public AgentInterface 
{
    public:
    void entry(const Event& e) {}
    
    // Judge whether to turn left or right based on sensor values.
    // Update the status
    void during() { 
        if ( sensor_value(0) < 80 ) 
        {
            if ( sensor_value( 1 ) < sensor_value( 2 ) )
            {
                emit( Event( "leader turn right" ) );
            } else
            {
                double x_ = AgentInterface::position().x;
                double y_ = AgentInterface::position().y;
                emit( Event( "leader turn left" ) );
            }
        } else
        {     
            if ( sensor_value( 1 ) < 30 )
            {
                track_velocity( 8, -0.3 );
            } else if ( sensor_value( 2 ) < 30 )
            {
                track_velocity( 8, 0.3 );
            } else
            {
                track_velocity( 10, 0 );
            }
        }              
    }
    void exit(const Event& e) {}
};

class TurnLeft : public State, public AgentInterface 
{
    public:

    // Label the agent if turning
    void entry(const Event& e) 
    {
        decorate("<circle x='-5' y='5' r='5' style='fill: black'></circle>");
        label(sensor_reflection_type(0), 20, 5);
    }

    // Control the turning speed and moving speed based on sensor value
    // Update the status
    void during() { 
        double safe_distance = ( sensor_value( 0 ) - 30 );
        double safe_speed = safe_distance > 0 ? safe_distance / 20 : 0;
        double r = ( 0.5 + safe_speed );
        track_velocity( safe_speed, r ); 
        if ( sensor_value( 0 ) > 120 ) 
        {
            track_velocity( 3, 0 );
            double x_ = AgentInterface::position().x;
            double y_ = AgentInterface::position().y;
            emit( Event( "leader move forward",{ { "x", x_ }, { "y", y_ } } ) );
        }
    }

    // Clear the turning label
    void exit(const Event& e) 
    {
        decorate("");
        clear_label();
    } 

    double dif;      
};

class TurnRight : public State, public AgentInterface 
{
    public:

    // Label the agent if turning
    void entry(const Event& e) 
    {
        decorate("<circle x='-5' y='5' r='5' style='fill: black'></circle>");
        label(sensor_reflection_type(0), 20, 5);
    }

    // Control the turning speed and moving speed based on sensor value
    // Update the status
    void during() { 
        double safe_distance = ( sensor_value( 0 ) - 30 );
        double safe_speed = safe_distance > 0 ? safe_distance / 20 : 0;
        double r = -( 0.5 + safe_speed );
        track_velocity( safe_speed, r );  
        if ( sensor_value( 0 ) > 120 ) 
        {
            track_velocity( 3, 0 );
            double x_ = AgentInterface::position().x;
            double y_ = AgentInterface::position().y;
            emit( Event( "leader move forward", { { "x", x_ }, { "y", y_ } } ) );
        }
    }

    // Clear the turning label
    void exit(const Event& e) 
    {
        decorate("");
        clear_label();
    }        
};

class Stop : public State, public AgentInterface 
{
    public:
    // Label the agent if it is stopped
    void entry(const Event& e) 
    {
        decorate("<circle x='-5' y='5' r='5' style='fill: black'></circle>");
        label(sensor_reflection_type(0), 20, 5);
    }

    // Stop the agent
    void during() { 
        track_velocity( 0, 0 );  
    }

    // Clear the label
    void exit(const Event& e) 
    {
        decorate("");
        clear_label();
    }        
};

class TargetController : public Process, public AgentInterface {

    public:
    // Initialize the target controller
    TargetController() : Process(), AgentInterface() {}

    // Watch user click. Update the corresponding status
    // Take care of the key pressed by users if manual mode is enabled
    void init() {
        is_stop = false;
        is_auto = true;
        watch("button_click", [this](Event e)
        {
            std::cout << e.value() << "\n";
            auto b = e.value()["value"].get<std::string>();
            if ( b == "Go/Stop" )
            {
                is_stop = !is_stop;
                if ( is_stop )
                {
                    emit( Event( "stop state" ) );
                } else
                {
                    emit( Event( "back to normal" ) );
                }
            } else if ( b == "Auto/Manual" )
            {
                is_auto = !is_auto;
                if ( is_auto )
                {
                    emit( Event( "back to normal" ) );
                } else
                {
                    track_velocity( 0, 0 ); 
                }
            } 
        });

        watch("keydown", [this](Event e)
        {
            auto k = e.value()["key"].get<std::string>();
            if ( k == "w" ) {
                    f = magnitude;              
            } else if ( k == "s" ) {
                    f = -magnitude;  
            } else if ( k == "a" ) {
                    tau = -magnitude;
            } else if ( k == "d" ) {
                    tau = magnitude;
            } 
        });

        watch("keyup", [&](Event &e) 
        {
            auto k = e.value()["key"].get<std::string>();
            if ( k == "w" || k == "s" ) {
                    f = 0;               
            } else if ( k == "a" ) {
                    tau = 0;
            } else if ( k == "d" ) {
                    tau = 0;
            } 
        });
    }

    void start() {}

    // Update the current location, speed and id to other agents
    void update()
    {
        double x_ = AgentInterface::position().x;
        double y_ = AgentInterface::position().y;
        double v_x = AgentInterface::velocity().x;
        double v_y = AgentInterface::velocity().y;
        double v_ = sqrt( v_x * v_x + v_y * v_y );
        double r_ = AgentInterface::angular_velocity();
        double a_ = AgentInterface::angle();
        int id = AgentInterface::id();
        emit( Event( "location update" + to_string( id ), 
                    { 
                        { "x", x_ }, 
                        { "y", y_ }, 
                        { "v", v_ }, 
                        { "r", r_ },
                        { "a", a_ },
                    } ) );
        
        if ( !is_auto )
        {
            apply_force( f, tau );
        }
    }
    void stop() {}

    bool is_stop;
    bool is_auto;
    double f, tau;
    double const magnitude = 200;
};

class LeaderController : public StateMachine, public AgentInterface {

    public:
        // Initialize the state machine
        // Add transitions of the leader
        LeaderController() : StateMachine() 
        {
            set_initial( moving_forward );
            add_transition( "leader turn left", moving_forward, turn_left );
            add_transition( "leader turn right", moving_forward, turn_right );
            add_transition( "leader move forward", turn_left, moving_forward );
            add_transition( "leader move forward", turn_right, moving_forward );
            add_transition( "stop state", moving_forward, stop_state );
            add_transition( "stop state", turn_left, stop_state );
            add_transition( "stop state", turn_right, stop_state );
            add_transition( "back to normal", stop_state, moving_forward );
        }

        MovingForward moving_forward;
        TurnLeft turn_left;
        TurnRight turn_right;
        Stop stop_state;
};

class Leader : public Agent {
    public:
    Leader(json spec, World& world) : Agent(spec, world) {
        add_process(c);
        add_process( tc );
    }
    private:
    LeaderController c;
    TargetController tc;
};

DECLARE_INTERFACE(Leader)

#endif