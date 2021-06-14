class Position{
  int id;
  float robot_pos[][];
  float ball_pos_x;
  float ball_pos_y;
  Position(){
    
  }
  void update_robot(int idx, float x, float y, float a){
    robot_pos[i][0]=x;
    robot_pos[i][1]=y;
    robot_pos[i][2]=a; 
  }
  void update_ball(float x, float y){
    ball_pos_x=x;
    ball_pos_y=y;
  }
};
