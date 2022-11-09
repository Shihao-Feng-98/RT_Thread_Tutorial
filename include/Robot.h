#include <unistd.h> // sleep
#include <vector>
using namespace std;


class Robot
{
public:
    Robot();
    ~Robot();
    void control_task();
    void motor_task_FR();
    void motor_task_FL();
    void motor_task_RR();
    void motor_task_RL();

    vector<double> cmd_FR, cmd_FL, cmd_RR, cmd_RL;
    vector<double> state_FR, state_FL, state_RR, state_RL;

};

Robot::Robot()
{
    cmd_FR.resize(3, 0.);
    state_FR.resize(3, 0.);

    cmd_FL.resize(3, 0.);
    state_FL.resize(3, 0.);

    cmd_RR.resize(3, 0.);
    state_RR.resize(3, 0.);

    cmd_RL.resize(3, 0.);
    state_RL.resize(3, 0.);
}

Robot::~Robot()
{

}

void Robot::control_task()
{
    usleep(150); // 0.15ms
    // 读反馈写指令
    cmd_FR[0] = state_FR[0] + 1;
    cmd_FR[1] = state_FR[1] + 1;
    cmd_FR[2] = state_FR[2] + 1;

    cmd_FL[0] = state_FL[0] + 1;
    cmd_FL[1] = state_FL[1] + 1;
    cmd_FL[2] = state_FL[2] + 1;

    cmd_RR[0] = state_RR[0] + 1;
    cmd_RR[1] = state_RR[1] + 1;
    cmd_RR[2] = state_RR[2] + 1;

    cmd_RL[0] = state_RL[0] + 1;
    cmd_RL[1] = state_RL[1] + 1;
    cmd_RL[2] = state_RL[2] + 1;
}

void Robot::motor_task_FR()
{
    usleep(3*1000); // 3ms
    // 读指令写反馈
    state_FR[0] = cmd_FR[0];
    state_FR[1] = cmd_FR[1];
    state_FR[2] = cmd_FR[2];
}

void Robot::motor_task_FL()
{
    usleep(3*1000); // 3ms
    // 读指令写反馈
    state_FL[0] = cmd_FL[0];
    state_FL[1] = cmd_FL[1];
    state_FL[2] = cmd_FL[2];
}

void Robot::motor_task_RR()
{
    usleep(3*1000); // 3ms
    // 读指令写反馈
    state_RR[0] = cmd_RR[0];
    state_RR[1] = cmd_RR[1];
    state_RR[2] = cmd_RR[2];
}

void Robot::motor_task_RL()
{
    usleep(3*1000); // 3ms
    // 读指令写反馈
    state_RL[0] = cmd_RL[0];
    state_RL[1] = cmd_RL[1];
    state_RL[2] = cmd_RL[2];
}
