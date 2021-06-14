import processing.net.*;

Client c2;


Robot_control r1;
Robot_control r2;
Robot_control r3;

float linear = 0, angular = 0;
float old_linear, old_angular;

void setup() 
{
  c2 = new Client(this, "127.0.0.2", 12345); // Replace with your server's IP and port
  r1 = new Robot_control(3);
  r2 = new Robot_control(4);
  r3 = new Robot_control(5);

  r1.set_vel_robot(0, 0); 
  r2.set_vel_robot(0, 0);
  r3.set_vel_robot(0, 0);

}

void draw() 
{
 if(linear !=  old_linear || angular != old_angular){
   r3.set_vel_robot(linear, angular); 
 }
 
 old_linear = linear;
 old_angular = angular;
}

void keyPressed(){
   if(keyCode == UP){
    linear = 2.5;
  }
  
  if(keyCode == DOWN){
    linear = -2.5;
  }
  
  if(keyCode == LEFT){
    angular = 2.5;
  }
  
  if(keyCode == RIGHT){
    angular = -2.5;
  }
  
}

void keyReleased(){
   if(keyCode == UP || keyCode == DOWN || keyCode == LEFT || keyCode == RIGHT){
     linear = 0.0;
     angular = 0.0;
   }
   
}
