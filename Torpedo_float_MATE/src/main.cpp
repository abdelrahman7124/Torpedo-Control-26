#include "main.h"
   
void setup() 
{
    Serial.begin(9600);
    Wire.begin(SDA_PIN,SCL_PIN);
    bmp.init();
    connection.connect_init();
    sampling_current_time = 0;
    sampling_prev_time = 0;
    first_dive_flag = false;
    first_stop_flag = false;
    second_stop_flag = false;
    second_dive_flag = false;
    rise_flag = false;

    reset_enable = false;
}

void loop()
{
    mission_state = execute_mission();

    if (mission_state == MISSION_COMPLETE)
    {
        send_msg();
    }

    if((mission_state == RESET) && (reset_enable))
    {
        first_dive_flag = false;
        first_stop_flag = false;
        second_stop_flag = false;
        second_dive_flag = false;
        rise_flag = false;
    }
}

//-------------------------------------------------Functions---------------------------------------------------------
void get_bmp_readings()
{
    pressure_data data;
    data.time_stamp = bmp.getTime();
    data.depth = bmp.readDepth();
    data.pressure = bmp.readPressure();
    pressure_log.push(data);
}


void send_msg()
{
    while(!pressure_log.empty())
    { 
        depth = pressure_log.front().depth;
        pressure = pressure_log.front().pressure;
        time_stamp = pressure_log.front().time_stamp;

        msg = time_stamp + "," + String(depth) + "," + String(pressure);
        
        connection.send_data(msg);
        pressure_log.pop();
    }
}


void sample_readings()
{
    sampling_current_time = millis();
    if((sampling_current_time - sampling_prev_time) >= SAMPLE_DURATION)
    {
        get_bmp_readings();
        sampling_prev_time = sampling_current_time;
    }
}


MovementState execute_mission()
{
    if(!first_dive_flag)
    {
        sample_readings();

        if(action.dive(FIRST_DEPTH) == IDLE )
        {
            first_dive_flag = true;
            return FIRST_DIVE_COMPLETE;
        }
        
        return DIVING;
    }

    else if(!first_stop_flag)
    {
        sample_readings();
        
        if(action.hold(FIRST_STOP_DURATION) == IDLE)
        {
            first_stop_flag = true;
            return FIRST_STOP_COMPLETE;
        }

        return HOVERING;
    }

    if(!second_dive_flag)
    {
        sample_readings();
        
        if(action.dive(SECOND_DEPTH) == IDLE)
        {
            second_dive_flag = true;
            return SECOND_DIVE_COMPLETE;
        }
        return DIVING;
        
    }

    if(!second_stop_flag)
    {
        sample_readings();
        
        if(action.hold(SECOND_STOP_DURATION) == IDLE)
        {
            second_stop_flag = true;
            return SECOND_STOP_COMPLETE;
        }
        return HOVERING;
    }

    if(!rise_flag)
    {
        sample_readings();

        if(action.rise_to_surface() == IDLE)
        {
            rise_flag = true;
            return MISSION_COMPLETE;
        }
        return SURFACING;
    }
    return RESET;
}