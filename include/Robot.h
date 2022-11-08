#include <unistd.h> // sleep
#include <vector>
using namespace std;


class Robot
{
public:
    Robot();
    ~Robot();
    void control_task();
    void motor_task();

    vector<double> cmd;
    vector<double> state_fb;

};

Robot::Robot()
{
    cmd.resize(3, 0.);
    state_fb.resize(3, 0.);
}

Robot::~Robot()
{

}

void Robot::control_task()
{
    usleep(200); // 0.2ms
    // 读反馈写指令
    cmd[0] = state_fb[0] + 1;
    cmd[1] = state_fb[1] + 1;
    cmd[2] = state_fb[2] + 1;
}

void Robot::motor_task()
{
    usleep(3*1000); // 3ms
    // 读指令写反馈
    state_fb[0] = cmd[0];
    state_fb[1] = cmd[1];
    state_fb[2] = cmd[2];
}
