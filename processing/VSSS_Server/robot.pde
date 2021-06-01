class Robot{
  Point position;
  float angle;
  float side_length;
  int team, role;
  Robot(){}
  Robot(float side_length, int team, int role){
    this.side_length = side_length;
    this.team        = team;
    this.role        = role;
  }
  void set_position(Point position){
    this.position = position;
  }
  void set_angle(float angle){
    this.angle = angle;
  }
  void _draw(){
    rectMode(CENTER);
    pushMatrix();
    translate(position.x, position.y);
    rotate(-angle);
    
    fill(robots_color);
    rect(0, 0, side_length, side_length);
    if(team == 1){
      fill(team_1_color);
    } else{
      fill(team_2_color);
    }
    
    
    
    if(role == 1){
      fill(goalkeeper_color);
    } else if(role == 2){
      fill(midfield_color);
    } else{
      fill(forward_color);
    }
    circle(side_length/4., -side_length/4., side_length/4.);
    popMatrix();
  }
  void _print(){
    print("position: ");
    position._print();
    println("angle: " + angle);
  }
};
