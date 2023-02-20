//#pragma once

#include "C:\Users\G\source\repos\Protocol\Protocol\ACBaseComponent.h"

extern "C" {
#include "extApi.h"
}

int sign(float a)
{
	if (a != 0) { return a / abs(a); }
	else return 0;
}

class PID
{
public:
	float kp, ki, kd, u, I, E_prev, u_prev, dt;
	float U_max = 15;
	float V_max = 10.5;
	//float ky;

	PID(float  kp = 20, float  ki = 0, float  kd = 0) // 20 | 1 | 0.8  //// 10 | 1 | 0.8 //
	{ 
		this->kp = kp;  
		this->ki = ki;  
		this->kd = kd;
		I = 0;
		E_prev = 0;
		u_prev = 0;
	}
	
	int get_ctrl_signal(float E)
	{
		//float E = g - x;
		float up = kp * E;

		I += E * dt;
		float ui = ki * I;

		float ud = kd * (E - E_prev) / dt;
		E_prev = E;

		u = up + ui + ud;
		if (abs(u) > U_max) { u = U_max; }
		if (abs(u - u_prev) / dt > V_max) { u = u_prev + V_max * dt*sign(u - u_prev); }
		u_prev = u;

		return u;
	}
};

class Ctrl_Sys : public ACBaseComponent
{
protected:
	SAddress addr_navsys;

	void timer_work()
	{
		if (is_update_time())
		{			
			//simxGetObjectPosition(clientID, robot, -1, rob_pos, simx_opmode_oneshot);////
			simxGetObjectOrientation(clientID, robot, -1, rob_orient, simx_opmode_oneshot);////
			//simxGetObjectVelocity(clientID, robot, linVel, angVel, simx_opmode_oneshot);////
			
			goal_pos[0] = (des_rob_pos[0] - rob_pos[0]) * cos(rob_orient[2]) + (des_rob_pos[1] - rob_pos[1]) * sin(rob_orient[2]);
			goal_pos[1] = -(des_rob_pos[0] - rob_pos[0]) * sin(rob_orient[2]) + (des_rob_pos[1] - rob_pos[1]) * cos(rob_orient[2]);

			cout << "\t" << goal_pos[0] << "\t" << goal_pos[1] << "\n";

			float dist = sqrt( pow(des_rob_pos[0] - rob_pos[0], 2) + pow(des_rob_pos[1] - rob_pos[1], 2) );
			////float des_ang = atan2(des_rob_pos[1] - rob_pos[1], des_rob_pos[0] - rob_pos[0]);
			float ang = atan2(goal_pos[1], goal_pos[0]); //des_ang - rob_orient[2];
			
			//cout << "\t" << ang * 57.3 << "\t" << dist << "\n";
			
			float vel = sqrt( pow(linVel[0], 2) + pow(linVel[1], 2) + pow(linVel[2], 2) );
			float vel_max = 0.3;

			pid.dt = update_time_millis;//last_time - start_time;
			//cout << " " << pid.dt << "\n";
			pid.get_ctrl_signal( dist );
	
			int k_ang = 10;
			int ky = 1;

			float motor_vel = ky * pid.u;
			
			if (dist < 0.1)
			{
				k_ang = 0;
				motor_vel = 0;
			}
			else if (vel > vel_max)
			{
				motor_vel = vel_max;
			}
			
			//cout << "\t" << ang * 57.3 << "\n"; //vel << endl;

			////if (dist > 0.1)// && vel < vel_max) 
			////{
			
			simxSetJointTargetVelocity(clientID, leftMotor, motor_vel - k_ang * ang, simx_opmode_oneshot);////
			simxSetJointTargetVelocity(clientID, rightMotor, motor_vel + k_ang * ang, simx_opmode_oneshot);////
			
			////}
			////else
			////{
			//	simxSetJointTargetVelocity(clientID, leftMotor, 0, simx_opmode_oneshot);////
			//	simxSetJointTargetVelocity(clientID, rightMotor, 0, simx_opmode_oneshot);////
				
				////simxStopSimulation(clientID, simx_opmode_oneshot_wait);
			////}
			////cout << "\t" <<  des_ang * 57.3 << "\t" << rob_orient[2] * 57.3 <<"\t"<< ang*57.3 << "\n"; //pid.u << "\tctrl_signal\n";
		}
	}
	int run(CMessage* msg)
	{
		if (msg)
		{
			switch (msg->typeMsg)
			{
				//case POINTS_MOVE:
					//{
					//	SMsg_Type::CPoints_move *pm = new SMsg_Type::CPoints_move;
					//	memcpy(pm, msg->content, msg->cont_length);
					//	cout << " " << msg->msg_length << "\t" << msg->cont_length << "\t" << pm->ptsn.velocity << "\t" << int(pm->ptsn.count) << "\t" << pm->ptsn.mode << endl;//ms.msg_length << "\t" << int(message->typeMsg) << endl;
					//	
					//	delete pm;
					//	pm = nullptr;
					//	
					//	break;
					//}
				case NAVIG:
				{
					SMsg_Type::Navig *nav = new SMsg_Type::Navig;
					memcpy(nav, msg->content, msg->cont_length);
				
					rob_pos[0] = nav->navidata.X;	 rob_pos[1] = nav->navidata.Y; 	  rob_pos[2] = nav->navidata.Z;
					//rob_orient[0] = nav->navidata.phi;	  rob_orient[1] = nav->navidata.theta;	 rob_orient[2] = nav->navidata.psi;
					linVel[0] = nav->navidata.vx;	 linVel[1] = nav->navidata.vy;	  linVel[2] = nav->navidata.vz;
					angVel[0] = nav->navidata.vx;	 angVel[1] = nav->navidata.wy;	  angVel[2] = nav->navidata.wz;
					linAccel[0] = nav->navidata.ax;		linAccel[1] = nav->navidata.ay;		linAccel[2] = nav->navidata.az;
					angAccel[0] = nav->navidata.Ex;		angAccel[1] = nav->navidata.Ey;		angAccel[2] = nav->navidata.Ez;

					/*cout << " " << nav->navidata.X << "\t" << nav->navidata.Y << "\t" << nav->navidata.Z << "\t\n" 
						 << " " << nav->navidata.phi << "\t" << nav->navidata.theta << "\t" << nav->navidata.psi << "\t\n"
				 		 << " " << nav->navidata.vx << "\t" << nav->navidata.vy << "\t" << nav->navidata.vz << "\t\n"
						 << " " << nav->navidata.wx << "\t" << nav->navidata.wy << "\t" << nav->navidata.wz << "\t\n"
						 << " " << nav->navidata.ax << "\t" << nav->navidata.ay << "\t" << nav->navidata.az << "\t\n"
						 << " " << nav->navidata.Ex << "\t" << nav->navidata.Ey << "\t" << nav->navidata.Ez << "\t\n\n";*/
					
					delete nav;
					nav = nullptr;

					break;
				}
				case CLOSE_ALL:
				{
					cout << "\n\tClose\n\n";
					//simxStopSimulation(clientID, simx_opmode_oneshot_wait);
					return 1;
				}
			}
		
			//SMsg_Type::CGet_Navig_data get_nvdt;
			//CMessage msg_send(addr_navsys, addr_current, EPriority::HIGH, EConfimation::NOT_CONFIRM, &get_nvdt);
			//msg_send.typeMsg = ETypeMsg::GET_NAVIG;//?????????????????????????????????????????????????????????????????????????????????????????????????????
			//excInterface->send(msg_send);
					
		}

		SMsg_Type::CGet_Navig_data *get_nvdt = new SMsg_Type::CGet_Navig_data;
		CMessage msg_send( addr_navsys, addr_current, EPriority::HIGH, EConfimation::NOT_CONFIRM, get_nvdt);
		msg_send.typeMsg = ETypeMsg::GET_NAVIG;//?????????????????????????????????????????????????????????????????????????????????????????????????????
		
		excInterface->send(msg_send);

		delete get_nvdt;
		get_nvdt = nullptr;

		return 0;
	}

public:
	simxInt leftMotor;
	simxInt rightMotor;
	simxInt clientID;
	simxInt robot;

	PID pid; 

	float des_rob_pos[3] = { 6, 0, 0 }; // P O I N T

	float rob_pos[3] = {0, 0, 0};
	float rob_orient[3] = { 0, 0, 0 };
	float linVel[3] = { 0, 0, 0 };
	float angVel[3] = { 0, 0, 0 };
	float linAccel[3] = { 0, 0, 0 };
	float angAccel[3] = { 0, 0, 0 };
	
	float goal_pos[3] = { 0, 0, 0 };

	Ctrl_Sys(ACExchangeIntf* intf_manager, const unsigned int _update_time_millis = 50)
	{
		//		Component address
		addr_current.addr_subsystem = 0xA;
		addr_current.addr_NODE = 0xB;
		addr_current.addr_cmp = 0xD;
		//		Manager_address
		addr_manager.addr_subsystem = 0xA;
		addr_manager.addr_NODE = 0xB;
		addr_manager.addr_cmp = 0x0;
		//		NavSys_address
		addr_navsys.addr_subsystem = 0xA;
		addr_navsys.addr_NODE = 0xB;
		addr_navsys.addr_cmp = 0xC;


		start_time = last_time = get_time();
		update_time_millis = _update_time_millis;
		excInterface = intf_manager;

		excInterface->open();

		init();
	}

	~Ctrl_Sys()
	{
		excInterface->close();
	}

	int init()
	{
		SMsg_Type::CCmp_registion reg;
		char str[20] = "CS\tControl system";
		for (int i = 0; i < 20; i++) { reg.name[i] = str[i]; }

		bool conf = false;

		CMessage msg_regis(addr_manager, addr_current, EPriority::HIGH, EConfimation::NOT_CONFIRM_NEED, &reg);
		while (!conf)
		{
			CMessage* regis_msg_recv = excInterface->recv();
			if (regis_msg_recv)
			{
				if (regis_msg_recv->typeMsg == CMP_REGISTRATION && regis_msg_recv->confirm == CONFIRM)
				{
					//start_time = last_time = get_time();/////////////
					cout << "CS\tConfirmation\n";
					conf = true;
					//break;
				}
				delete excInterface->msg;
				excInterface->msg = nullptr;
			}
			excInterface->send(msg_regis);
		}


		clientID = simxStart("127.0.0.1", 19998, true, true, 3000, 5);

		simxGetObjectHandle(clientID, "Pioneer_p3dx", &robot, simx_opmode_oneshot_wait);
		simxGetObjectHandle(clientID, "Pioneer_p3dx_leftMotor", &leftMotor, simx_opmode_oneshot_wait);
		simxGetObjectHandle(clientID, "Pioneer_p3dx_rightMotor", &rightMotor, simx_opmode_oneshot_wait);

		if (clientID != -1)
		{
			cout << "Connected to remote API server\n";
			//simxStartSimulation(clientID, simx_opmode_oneshot_wait);////
		}
		return 0;
	};
};

int main()
{
	//simxInt clientID = simxStart("127.0.0.2", 19998, true, true, 1000, 5);
	//simxStartSimulation(clientID, simx_opmode_oneshot_wait);

	UDP_client clnt("127.0.0.1", 54000);

	Ctrl_Sys cs(&clnt, 5);
	
	Sleep(100);

	int err = 0;
	while (!(err = cs.start()));
	if (err != 1 && err != 0)
	{
		cout << "Error: " << err << "!\n";
	}





	//Sleep(3000);
	//simxStopSimulation(clientID, simx_opmode_oneshot_wait);


	return 0;
}