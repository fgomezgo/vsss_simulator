class Position{
  
  float robot_pos[][];
  float x, y;
  
  Position(){
    this.x = this.y = 0.0;
  }
  void update_robot(int idx, float x, float y, float a){
     println("Updating robot " + idx + " " + x + " " + y + " " + a);
    robot_pos[idx][0]=x;
    robot_pos[idx][1]=y;
    robot_pos[idx][2]=a; 
  }
  void setBallPosition(float x, float y){
    println("Updating ball " + x + " " + y );
    this.x = x;
    this.y = y;
  }
};
