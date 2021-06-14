import processing.net.*;

Client c1;
String input;
int data[];
float Vl=-1.0;
float Va=1.0;
float idx=0;
int aux=0;

Position pos;

Robot_control r1;
Robot_control r2;
Robot_control r3;

void setup() 
{
  c1 = new Client(this, "127.0.0.2", 12345); // Replace with your server's IP and port
  pos = new Position();
  r1 = new Robot_control(0);
  r2 = new Robot_control(1);
  r3 = new Robot_control(2);

  r1.set_vel_robot(-3,1); 
  r2.set_vel_robot(-1,0);
  r3.set_vel_robot(0,12);

}

void draw() 
{
  if (c.available() > 0) {
    input = c.readString();
    input = input.substring(0, input.indexOf("\n")); // Only up to the newline
    data = int(split(input, ' ')); // Split values into an array
    if (data[0] == 'r'){
      pos.update_robot(data[1], data[2], data[3], data[4]);
    }else if (data[0] == 'b'){
      pos.update_ball(data[1],data[2]);
    }
    
 
}
