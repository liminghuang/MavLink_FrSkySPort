--
-- offset.lua part of of MavLink_FrSkySPort
--		https://github.com/Clooney82/MavLink_FrSkySPort
--
-- Copyright (C) 2014 Michael Wolkstein
--	 https://github.com/Clooney82/MavLink_FrSkySPort
--
-- modified by
--	(c) 2015 Jochen Kielkopf
--	 https://github.com/Clooney82/MavLink_FrSkySPort
--
-- modified by
--	(c) 2016 Paul Atherton
--	 https://github.com/Clooney82/MavLink_FrSkySPort
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY, without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, see <http://www.gnu.org/licenses>.
--

local inputs = {
	{"O-SET mAh% ", VALUE,-100, 100,   0},
	{"O-SET Wh%",   VALUE,-100, 100,   0},
	{"BatCap Wh",   VALUE,   0, 250,  123},
	{"SpeedUnits",  VALUE,   1, 3,  1},
	{"AltUnits",    VALUE,   1, 2, 1}
	}

local function run_func(offsetmah, offsetwatth, batcapwh, spunits, aunits)
  gOffsetmah = offsetmah
  gOffsetwatth = offsetwatth
  gBatcapwh = batcapwh
  
	if spunits == 2 then --check SpeedUnits input & set global vars
	  gSpeed_multi = 3.6
	  gSpeed_units = "kph"
	elseif spunits == 3 then
	  gSpeed_multi = 2.23694
	  gSpeed_units = "mph"
  else
    gSpeed_multi = 1
  	gSpeed_units = "m/s"
	end

	if aunits == 2 then  --check AltUnits input & set global vars
	  gAlt_multi = 3.28084
	  gAlt_units = "f"
  else
    gAlt_multi = 1
	  gAlt_units = "m"
	end
	
end

return {run=run_func, input=inputs}
