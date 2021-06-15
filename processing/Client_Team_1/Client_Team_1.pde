import processing.net.*;

Client c1;


String input;
float data[];

int number_of_robots = 6;

float Vl=-1.0;
float Va=1.0;
float idx=0;
int aux=0;

Point gk_velocity = new Point();

String stream[];
Positions status = new Positions(number_of_robots);

Robot_control r1;
Robot_control r2;
Robot_control r3;



void setup() 
{
  c1 = new Client(this, "127.0.0.2", 12345); // Replace with your server's IP and port
  r1 = new Robot_control(0);
  r2 = new Robot_control(1);
  r3 = new Robot_control(2);

  //r1.set_vel_robot(-3,1); 
  //r2.set_vel_robot(-1,0);
  //r3.set_vel_robot(0,12);
  
  thread("fieldTCP");

}

void predictImpact(){
  float threshold = 10;
  if(status.ball_old_pos.y < status.robots_pos[0].y + threshold){
    if(  status.robots_pos[0].y > 355){
      gk_velocity.x = 2.0;
    }else{
      gk_velocity.x = 0.0;
    }
    
  }else{
    if(  status.robots_pos[0].y < 655){
      gk_velocity.x = -2.0;
    }else{
      gk_velocity.x = 0.0;
    }
  }
  
  
}

void draw() 
{
  //float m = (status.ball_old_pos.y - status.ball_pos.y) / (status.ball_old_pos.x - status.ball_pos.x);
  predictImpact();
  println(status.robots_pos[0].y + " vel: " + gk_velocity.x);
  
  r1.set_vel_robot( gk_velocity.x, 0);
  
  delay(500);
}


void fieldTCP(){
   //Recieve data from clients
  while(true){
    while(c1.available() == 0);
    input = c1.readString();
    input = input.substring(0, input.indexOf("\n"));
    data = float(split(input, ' '));
    //println(data);
    status.update_ball(new Point(data[1], data[2]));
    //println(status.robots_pos.length);
    for(int i = 0; i < number_of_robots; ++i){
      status.update_robot(i, new Point(data[i*3 + 3], data[i*3 + 4]), data[i*3 + 5]);
    }
    //status._print();
  }
  
}
