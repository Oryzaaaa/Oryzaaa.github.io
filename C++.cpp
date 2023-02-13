#include "Main.h" //move 
void main ( void )
{
freq=0; // 0=1khz (red), 1=10kHz(green beacon)
SetDigitalOutput ( 10 , freq ) ; // turn to 1kHz (red beacon)
ambient_level = 200; // esed in 'move'
slow_level = 5000; // used in 'move'
stop_level = 6000; // used in 'move'
expose_time = 3; // used in expose_and_read
steer_sensitivity = 20; // used in 'move'
forward_speed = 70; // forward speed, used in 'move' 35
slow_speed = 40; // slow speed, used in 'move' 25
spin_speed = 45; // spin speed (for searching mode), used in 'move' 50
state = 0;
bumper = 1;
distance = 0;
while ( state == 0 )
{
bumper = GetDigitalInput ( 2 ) ;
if ( bumper == 0 )
{
state = 1 ;
}
}
while ( state == 1 )
{
Read_PD ( ) ;
find_max ( ) ;
move ( ) ;
limit = GetDigitalInput ( 1 ) ;
if ( limit == 0 )
{
SetMotor ( 2 , 0 ) ;
SetMotor ( 3 , 0 ) ;
state = 2 ;
}
}
while ( state == 2 )
{
SetMotor ( 1 , 80 ) ;
Wait ( 500 ) ;
SetMotor ( 1 , -80 ) ;
Wait ( 500 ) ;
SetMotor ( 1 , 0 ) ;
Wait ( 500 ) ;
Read_PD ( ) ;
find_max ( ) ;
if ( PD_sum < 400 )
{
SetMotor ( 2 , -100 ) ;
SetMotor ( 3 , 100 ) ;
Wait ( 1000 ) ;
state = 3 ;
}
}
while ( state == 3 )
{
freq = 1; // 0=1khz (red), 1=10kHz(green beacon)
SetDigitalOutput ( 10 , freq ) ; // turn to 1kHz (red beacon)
Read_PD ( ) ;
find_max ( ) ;
move ( ) ;
limit = GetDigitalInput ( 1 ) ;
if ( limit == 0 )
{
SetMotor ( 2 , 0 ) ;
SetMotor ( 3 , 0 ) ;
state = 4 ;
}
}
if ( state == 4 )
{
SetMotor ( 1 , 80 ) ;
Wait ( 500 ) ;
SetMotor ( 1 , 0 ) ;
Wait ( 500 ) ;
state = 5;
}
while ( state == 5 )
{
StartUltrasonic ( 4 , 5 ) ;
distance = GetUltrasonic ( 4 , 5 ) ;
SetMotor ( 2 , -127 ) ;
SetMotor ( 3 , 127 ) ;
if ( distance <= 20 )
{
SetMotor ( 2 , -127 ) ;
SetMotor ( 3 , 127 ) ;
Wait ( 500 ) ;
}
} 
}
#include "Main.h"//read_PD
void Read_PD ( void ) 
{
SetDigitalOutput ( 11 , 1 ) ; // close shutter, clear film, and increment counter
SetDigitalOutput ( 12 , 1 ) ; // initialize conter value to '0'
SetDigitalOutput ( 12 , 0 ) ; // allow counter to count
Wait ( 5 ) ; // 5mS wait for things settling down
SetDigitalOutput ( 11 , 0 ) ; // open shutter and expose film
Wait ( expose_time ) ; // exposer time = 3mS to 8mS
PD0 = GetAnalogInput ( 1 ) ; // read intensity
PD1 = expose_and_read ( ) ;
PD2 = expose_and_read ( ) ;
PD3 = expose_and_read ( ) ;
PD4 = expose_and_read ( ) ;
PD5 = expose_and_read ( ) ;
PD6 = expose_and_read ( ) ;
PD7 = expose_and_read ( ) ;
PD_sum=PD0+PD1+PD2+PD3+PD4+PD5+PD6+PD7; 
}
#include "Main.h"
int expose_and_read ( void ) 
{
int intensity;
SetDigitalOutput ( 11 , 1 ) ; // close shutter, clear film, increment counter
Wait ( 5 ) ; // Wait 5mS for things to settle
SetDigitalOutput ( 11 , 0 ) ; // open shutter for exposure
Wait ( expose_time ) ; // expose time = 3mS to 8mS (can be adjusted)
intensity = GetAnalogInput ( 1 ) ; // get the IR intensity reading
return intensity ; 
}
#include "Main.h"
void find_max ( void ) 
{
max_val = PD0 ;
max_no =0;
if ( PD1 > max_val )
{
max_val = PD1;
max_no =1;
}
if ( PD2 > max_val )
{
max_val = PD2;
max_no =2;
}
if ( PD3 > max_val )
{
max_val = PD3;
max_no =3;
}
if ( PD4 > max_val )
{
max_val = PD4;
max_no =4;
}
if ( PD5 > max_val )
{
max_val = PD5;
max_no =5;
}
if ( PD6 > max_val )
{
max_val = PD6;
max_no =6;
}
if ( PD7 > max_val )
{
max_val = PD7 ;
max_no =7;
} 
}
#include "Main.h" //limit PWM 
int limit_pwm ( int temp ) 
{
int limited;
if ( temp > 127 )
{
limited =127;
}
else if ( temp < -127 )
{
limited = -127;
}
else
{
limited =temp;
}
return limited ; 
}
#include "Main.h"//move
void move ( void ) 
{
int temp;
int steer;
int error;
int speed;
error=4-max_no; // heading direction error, if PD4 ==max_no, no error
steer=error*steer_sensitivity; // steering effort is proportional to heading error
speed=forward_speed; // forward speed
if ( PD_sum<ambient_level ) // If <background noise level => search mode.
{
speed=0; // search mode => no forward motion
steer=-spin_speed; // search mode => spin
}
if ( PD_sum>slow_level ) // Beacon is near
{
speed=slow_speed; // Slow down
}
if ( PD_sum>stop_level ) // Found the beacon
{
speed=0; // Stop
steer=0; // no steering
}
temp=limit_pwm(0+steer+speed);
SetMotor ( 2 , temp ) ;
temp=limit_pwm(0+steer-speed);
SetMotor ( 3 , temp ) ; 
}
