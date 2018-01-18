/*
 * CControlComp.cpp
 *
 *  Created on: Dec 14, 2017
 *      Author: Michael Weber, Nikola Fischer
 */

#include "CControlComp.h"

#include "Global.h"
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#define _USE_MATH_DEFINES


CControlComp::CControlComp(CContainer &pContainer) : myContainer(pContainer),
														alpha(2.295082),
														alpha_complement(0.98),
														m_X1(1679.12),
														b_X1(729.78),
														m_X2(1675.03),
														b_X2(762.06),
														m_Y1(-1688.10),
														b_Y1(-139.55),
														m_Y2(-1678.80),
														b_Y2(-114.07),
														K1(2.2583),
														K2(0.1462),
														K3(0.0007),
														mPhi_C_PREV(0.0)
{
}


void CControlComp::run()
{


	while (true)
	{
		//Um Zeitdifferenzen ausrechnen zu können
		gettimeofday(&timebegin, (struct timezone*)0);

		fetch();
		kalib();
		filter();
		regler();
		motor();

		gettimeofday(&timeend, (struct timezone*)0);
		seconds = timeend.tv_sec - timebegin.tv_sec;
		useconds = timeend.tv_usec - timebegin.tv_usec;
		useconds += 1000*1000*seconds;
		cout << "Zyklus in us " << useconds << endl;


		usleep(20000 - useconds);
		//myContainer.signalReader(); // Semaphore Give
	}

}

void CControlComp::init()
{
	cout<<"ControleInit"<<endl;
}

void CControlComp::fetch()
{
	myHW.fetchValues(ADCvalue, mySensorData1, mySensorData2);
	//myContainer.writeSensor1Data(mySensorData1);
	//myContainer.writeSensor2Data(mySensorData2);
	//myContainer.writeADCValue(ADCvalue);
}
void CControlComp::kalib()
{
	double x1_dd = (mySensorData1.mX__dd-b_X1)/m_X1;
	double x2_dd = (mySensorData2.mX__dd-b_X2)/m_X2;
	double y1_dd = (mySensorData1.mY__dd-b_Y1)/m_Y1;
	double y2_dd = (mySensorData2.mY__dd-b_Y2)/m_Y2;

	Zustandsvektor.mPhi_A = (-1)*atan((x1_dd-alpha*x2_dd)/(y1_dd-alpha*y2_dd));		//Berechnung phi in rad
	Zustandsvektor.mPhi__d = ((mySensorData1.mPhi__d +12) *0.00106);				//Berechnung phi_d in rad/s aus Gyroskop --> Angepasste Konstanten, Was ist mit 2. Sensor?

}
void CControlComp::filter()
{
	Zustandsvektor.mPhi_C = alpha_complement*(mPhi_C_PREV+(0.02*Zustandsvektor.mPhi__d)) + (1-alpha_complement)*Zustandsvektor.mPhi_A;		// Komplementärfilter
	mPhi_C_PREV = Zustandsvektor.mPhi_C;
}
void CControlComp::regler()
{
	Zustandsvektor.mPsi__d = (ADCvalue-2026)*0.48975/3.333;  //Alter Faktor: 4.8975
	Zustandsvektor.mPhi_C += 0.16F;
	torque = (K1*Zustandsvektor.mPhi_C+K2*Zustandsvektor.mPhi__d+K3*Zustandsvektor.mPsi__d);
}
void CControlComp::motor()
{
	myHW.enableMotor();
	float phi_abs = Zustandsvektor.mPhi_C > 0.0 ? Zustandsvektor.mPhi_C : -Zustandsvektor.mPhi_C;
	if(phi_abs < 0.15F)
	{
		myHW.setTorque(-torque);
	}
	else
	{
		myHW.setTorque(0.0F);
	}

}
