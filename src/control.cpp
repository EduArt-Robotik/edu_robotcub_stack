#include "control.h"

/*
navigation: 
0 geradeaus fahren
1 rampe hoch fahren
2 winkel fahren - vor knick
3 rampe runter fahren
4 gerade Fahren
5 gerade zurück fahren
6 rampe hoch fahren
7 winkel fahren - vor knick
8 rampe nach unten fahren
9 geradeaus fahren zurück
*/

Control::Control(ClientService *map1ServerService, ClientService *map2ServerService, ClientService *map3ServerService, ClientService *map4ServerService) {
    m_map1ServerService = map1ServerService;
    m_map2ServerService = map2ServerService;
    m_map3ServerService = map3ServerService;
    m_map4ServerService = map4ServerService;

    m_yaw_z = 0;
    m_x = 0;
    m_y = 0;
    m_angle = 0;
    m_speed = 0;
    m_x_dest = 3.1;
    m_y_dest = 0;
    m_navigation = 0;
    m_speed_var = 1;
    m_slowDown = 20;

    m_activeMapServer = m_map1ServerService;

}

void Control::calculateAngleSpeed() {

    if(m_navigation == 0){
        if(m_x > 2){   
            control_navigation();
        }
    }
    else if( m_navigation == 2){
        if( (m_pitch_y >= 0.10 )&& (m_y > 0.5) && (m_x > 2.5) ) {
            control_navigation();
            m_slowDown = 100;
            m_slowDownReduce = 2;

            m_activeMapServer = m_map3ServerService;
            m_map2ServerService->deactiveService();
        }
    }
    else if(m_navigation == 3){
        if(m_x > 4.1 && (m_pitch_y < 0.2 || m_pitch_y > -0.2) &&  (m_roll_x > -0.2 || m_roll_x < 0.2)) {
            m_slowDown = 200;
            m_slowDownReduce = 20;
            control_navigation();
            m_activeMapServer = m_map4ServerService;
            m_map3ServerService->deactiveService();
        }  
    }
    else if(m_navigation == 5){
        if(m_x < 4 && (m_pitch_y < 0.2 || m_pitch_y > -0.2) &&  (m_roll_x > -0.2 )) {
            m_slowDown = 200;
            m_slowDownReduce = 20;
            control_navigation();
            m_activeMapServer = m_map3ServerService;
            m_map4ServerService->deactiveService();
        }  
    }
    else if( m_navigation == 7){
        if( (m_pitch_y >= 0.10 )&& (m_y < 0.5) && (m_x > 4) ) {
            control_navigation();
            m_slowDown = 100;
            m_slowDownReduce = 2;
            m_activeMapServer = m_map2ServerService;
            m_map3ServerService->deactiveService();
        }
    }
    else if(m_navigation == 8){
        if(m_x < 2 && (m_pitch_y < 0.2 || m_pitch_y > -0.2) &&  (m_roll_x > -0.2 || m_roll_x < 0.2)) {
            m_slowDown = 200;
            m_slowDownReduce = 20;
            control_navigation();
            m_activeMapServer = m_map4ServerService;
            m_map3ServerService->deactiveService();
        }  
    }


    float delta_x = m_x_dest - m_x;
    float delta_y = m_y_dest - m_y;
    
    //Ausgabe
    //std::cout << "x:" << x <4<< std::endl;
    //std::cout << "delta_x:" << delta_x << std::endl;
    //std::cout << "delta_y:" << delta_y << std::endl;
    
    float delta_dist = sqrt((delta_x*delta_x) + (delta_y*delta_y));
    
    //Ausgabe
    //std::cout << "delta_dist:" << delta_dist << std::endl;
    
    float delta_phi = (atan2(delta_y, delta_x))- m_yaw_z; 
    
    // Begrenzung des Drehwinkels Phi
    if(delta_phi < -M_PI){
        delta_phi += 2*M_PI;
        }
    if(delta_phi > M_PI){
        delta_phi -= 2*M_PI;
        };

    //std::cout << "delta_phi:" << delta_phi << std::endl;
    
    m_angle = delta_phi;
    
    if((delta_phi > 0.2) || (delta_phi < -0.2)) {
        m_speed = 0.001;
        m_slowDown = 200;
        m_slowDownReduce = 20;
    } 
    else {
        m_slowDown = m_slowDown - m_slowDownReduce;
     if( m_slowDown < 20){
        m_slowDown = 20;
     }
        m_speed = (delta_dist) / (float) m_slowDown;
        if(m_speed < 0.3){
            m_speed  = 0.3;   
        }
    } 
    if ((delta_dist < 0.1)) {
        control_navigation();
        calculateAngleSpeed();
    }

}

void Control::control_navigation(){
    m_navigation++;
    m_navigation = m_navigation%10;
    if( m_navigation == 0 ){
        m_x_dest = 3.05;
        m_y_dest = 0;
        m_speed_var = 1;
    }
    if( m_navigation == 1 ){
        m_activeMapServer = m_map2ServerService;
        m_map1ServerService->deactiveService();
    }
    if( m_navigation == 2 ){
        m_x_dest = 3.35;
        m_y_dest = 1.3;
        m_speed_var = 1;
    }
    if( m_navigation == 3 ){
        m_x_dest = 5.5;
        m_y_dest = 1.3;
        m_speed_var = 1;
    }
    if( m_navigation == 5 ){
        m_x_dest = 3.5;
        m_y_dest = 1.1;
    }
    if( m_navigation == 7 ){
        m_x_dest = 3.0;
        m_y_dest = 0;
    }
    if( m_navigation == 8 ){
        m_x_dest = 0;
        m_y_dest = 0;
    }

}

void Control::setPosYaw(float x,float y,float yaw_z){
    m_x = x;
    m_y = y;
    m_yaw_z = yaw_z;
    calculateAngleSpeed();
}

void Control::setPitchY(float pitch_y){
    m_pitch_y = pitch_y;

}

void Control::setYawZ(float yaw_z){
    m_yaw_z = yaw_z;
    calculateAngleSpeed();
}

void Control::setRollX(float roll_x){
    m_roll_x = roll_x;
}

void Control::setX(float x){
    m_x = x;
    calculateAngleSpeed();
}

void Control::setY(float y){
    m_y = y;
    calculateAngleSpeed();
}

float Control::getAngle(){
    return m_angle;
}

float Control::getSpeed(){
    return m_speed;
}

int Control::getNavigationStep(){
    return m_navigation;
}
geometry_msgs::msg::Pose Control::getInitialpose()
{
    m_newInitialpose = false;
    return m_initialpose;
}
bool Control::newInitialpose()
{   
    return m_newInitialpose;
}