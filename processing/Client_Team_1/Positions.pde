class Positions{
  
  Point ball_pos;
  Point robots_pos[];
  float robots_angle[];
  
  Positions(int number_of_robots){
    robots_pos   = new Point[number_of_robots];
    robots_angle = new float[number_of_robots];
  }
  void update_robot(int idx, Point position, float angle){
    robots_pos[idx] = position;
    robots_angle[idx] = angle;
    //println("Updating robot " + idx + " " + robots_pos[idx].x + " " + robots_pos[idx].y + " " + robots_angle[idx]);
  }
  void update_ball(Point position){
    ball_pos = position;
    //println("Updating ball " + ball_pos.x + " " + ball_pos.y);
  }
  void _print(){
    print("Ball position: ");
    ball_pos._print();
    for(int i = 0; i < robots_pos.length; ++i){
      println("Robot " + i + ": ");
      print("position: ");
      robots_pos[i]._print();
      println("angle: " + robots_angle[i]);
    }
  }
};
