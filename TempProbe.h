/******************************************************************************
  TempProbe.cpp
 
  Description: The TempProbe and SmokerProbe classes simplify the process of measuring temperatures using a thermistor probe.
  In addition to handling the sampling and conversion to temperature units, the class can also
  monitor trends over time.
 
  Copyright(c) 2009 Mark A. Patel  All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.


 *****************************************************************************/

#ifndef TEMPPROBE_H_
#define TEMPPROBE_H_



typedef enum s {
	NORMAL, // Normal operating state
	HEATING, // Heating after temp change or lid opening
	LOW_FIRE // Low fire level detected
} smokerState;
	
class TempProbe {


	public:
		TempProbe(char pin);
		int getTemp();

		virtual char probe_task();	

	/**
	 * Current value (tenths of deg F)
	 */
	int value;
	
	/**
	 * Analog pin used by this TempProbe
	 */
	char pin;
	
};


class SmokerProbe : public TempProbe {
	
public:
	
	int targetTemp;
    
	float longTermAvg;
	
	float shortTermAvg;
	
	char fan_pin;
	
	int fanPct;
	
	float fanAvg;
	
	char warmUpMode;
	
	char coldIndicatorPin;
	char normalIndicatorPin;
	char hotIndicatorPin;
	
	SmokerProbe(char pin, char fan_pin, char coldPin, char normalPin, char hotPin);
	
	virtual smokerState probe_task2();	
	
};

#endif /* TEMPPROBE_H_ */
