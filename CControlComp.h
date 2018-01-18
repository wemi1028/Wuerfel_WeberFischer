/*
 * CControlComp.h
 *
 *  Created on: Dec 11, 2017
 *      Author: vmuser
 */

#ifndef CCONTROLCOMP_H_
#define CCONTROLCOMP_H_

#include "CContainer.h"
#include "IRunnable.h"
#include "CBBBHardware.h"
#include "SMPU6050Data.h"

#include <iostream>
using namespace std;

class CControlComp: public IRunnable
{
public:
	CControlComp(CContainer &pContainer);

	void fetch();
	void kalib();
	void filter();
	void regler();
	void motor();

	void run();
	void init();

private:
	CContainer &myContainer;

	CBBBHardware myHW;
	SMPU6050Data mySensorData1;
	SMPU6050Data mySensorData2;

	UInt16 ADCvalue;
	SStateVectorData Zustandsvektor;
	float mPhi_C_PREV;
	float torque;

	struct timeval timebegin, timeid, timeend;
	time_t seconds, useconds;

	const double alpha;				// Verhältnis von Sensor-Radien
	const double alpha_complement;  // alpha aus Komplementärfilter, hängt von Abtastrate und Tau ab
	const double m_X1;		// m = Steigung von X-Wert Sensor 1
	const double b_X1;		// b = Offset von X-Wert Sensor 1
	const double m_X2;		// m = Steigung von X-Wert Sensor 2
	const double b_X2;		// b = Offset von X-Wert Sensor 2
	const double m_Y1;		// m = Steigung von y-Wert Sensor 1
	const double b_Y1;		// b = Offset von y-Wert Sensor 1
	const double m_Y2;		// m = Steigung von y-Wert Sensor 2
	const double b_Y2;		// b = Offset von y-Wert Sensor 2
	const double K1;		// Reglerkonstante
	const double K2;		// Reglerkonstante
	const double K3;		// Reglerkonstante

};




#endif /* CCONTROLCOMP_H_ */
