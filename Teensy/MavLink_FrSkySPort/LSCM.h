/**
 *
 * Class LSCM, part of MavLink_FrSkySPort
 * 
 * Copyright (C) 2015 Michael Wolkstein
 * https://github.com/Clooney82/MavLink_FrSkySPort
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * 
 * 
 * Class definition of Wolke lipo-single-cell-monitor
 * *******************************************
 * This will monitor 1 - 8 cells of your lipo.
 * It display the low cell, the high cell and the difference between this cells
 * this will give you a quick overview about your lipo status and if the lipo is well balanced
 *
 * detailed informations and schematics here
 *
 */

#ifndef _LSCM_H_
#define _LSCM_H_

#include "Arduino.h"

class LSCM {

public:
    
    /**
    \ingroup MavLink_FrSkySPort
    \brief Overloaded Constructor of LSCM Object
    \descriptions construct LSCM minimal with cells count.
    \example LSCM lscm(3, 13, 0.99);
    \args
    \cells how many cells connected
    \analogReadResolution on teensy3.x 13 bit as default
    \smoothness value of low pass filtering, 0.0001 is max 0.99 is off (default)
    */
    LSCM(uint8_t cells, uint8_t analogReadResolution, float smoothness);
    LSCM(uint8_t cells, uint8_t analogReadResolution);
    LSCM(uint8_t cells);
    
    /**
    \brief main-process-call of LSCM. 
    \descriptions: must called in main loop.
    */
    void process();
    
    /**
    \brief returns disered Cell voltage 0-n based on lipocells. 
    \descriptions returned an uint32_t of called lipocell.
    */   
    uint32_t getCellVoltageAsUint32_T(uint8_t cell);

    /**
    \brief returns Cell voltage sum from al Cells. 
    \descriptions returned an int32_t Cell sum.
    */ 
    int32_t getAllLipoCells();
    
    /**
    \brief returns quantity of really connected cells. 
    \descriptions returned an uint8_t of real connected cells.
    */ 
    uint8_t getCellsInUse();
    
    /**
    \brief quantity of cells at init. 
    \descriptions returned an uint8_t of constructer maxcells.
    */
    uint8_t getMaxCells();
    
    /**
    \brief debug. 
    \descriptions enable disable debug.
    */
    void setDebug(bool debug);

    /**
    \brief Overloaded Function of LSCM setCustomCelldivider
    */
    void setCustomCellDivider(double a);
    void setCustomCellDivider(double a, double b);
    void setCustomCellDivider(double a, double b, double c);
    void setCustomCellDivider(double a, double b, double c, double d);
    void setCustomCellDivider(double a, double b, double c, double d, double e);
    void setCustomCellDivider(double a, double b, double c, double d, double e, double f);
    void setCustomCellDivider(double a, double b, double c, double d, double e, double f,
                              double g);
    void setCustomCellDivider(double a, double b, double c, double d, double e, double f,
                              double g, double h);
    void setCustomCellDivider(double a, double b, double c, double d, double e, double f,
                              double g, double h, double i);
    void setCustomCellDivider(double a, double b, double c, double d, double e, double f,
                              double g, double h, double i, double j);
    void setCustomCellDivider(double a, double b, double c, double d, double e, double f,
                              double g, double h, double i, double j, double k);
    void setCustomCellDivider(double a, double b, double c, double d, double e, double f,
                              double g, double h, double i, double j, double k, double l);
    
    
private:
    
    /**
    \descriptions init LSCM object variables and arrays. automatically called by constructor
    */
    void initLSCM(uint8_t cells, uint8_t analogReadReso, float smoothness);

    /**
    \descriptions set Celldivider
    */
    void setCellDivider(double a, double b, double c, double d, double e, double f,
                        double g, double h, double i, double j, double k, double l);    
    
    /**
    \brief main Array which holds software deviders. 
    \descriptions this 13bit default divider are used if LSCM object will init. do not change this!
    */
    double _LIPOCELL_1TO8[13] =
    {
      1913.528953519,
      933.688035297,
      622.955076603,
      473.787040052,
      373.105567418,
      317.423580786,
      0.0, // diverders 7-12 not defined because my network includes only 6 voltage dividers
      0.0,
      0.0,
      0.0,
      0.0,
      0.0
    };
    
    bool _debug;
    double *_individualcelldivider;
    uint8_t _analogread_threshold;         // threshold for connected zelldetection in mV
    uint8_t _cells_in_use;
    int32_t *_zelle;
    double *_cell;
    int32_t _alllipocells;
    float _lp_filter_val; // this determines smoothness  - .0001 is max  0.99 is off (no smoothing)
    double *_smoothedVal; // this holds the last loop value
    uint8_t _maxcells;
    
    
};
#endif /* _LSCM_H_ */
 
