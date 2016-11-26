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
	{"BatCap Wh",   VALUE,   0, 250,  30},
	{"SpeedUnits",  VALUE,   1, 3,  1},
	{"AltUnits",    VALUE,   1, 2, 1}
	}

speed_multi=1
speed_units="m/s"
alt_multi=1
alt_units="m"
local oldoffsetmah=0
local oldoffsetwatth=0
local oldbatcapwh=0
local used_flightmode=8

local function run_func(offsetmah, offsetwatth, batcapwh, spunits, aunits)
	if oldoffsetmah ~= offsetmah or oldoffsetwatth ~= offsetwatth or oldbatcapwh ~= batcapwh then
	  model.setGlobalVariable(1, used_flightmode, offsetmah)   --mAh
    model.setGlobalVariable(2, used_flightmode, offsetwatth) --Wh
    model.setGlobalVariable(3, used_flightmode, batcapwh)    --Wh
	  oldoffsetmah   = offsetmah
	  oldoffsetwatth = offsetwatth
	  oldbatcapwh    = batcapwh
	end
	if spunits == 1 then speed_multi = 1; speed_units = "m/s"
	elseif spunits == 2 then speed_multi = 3.6; speed_units = "kph"
	elseif spunits == 3 then speed_multi = 2.23694; speed_units = "mph"
	end
	if aunits == 1 then alt_multi = 1; alt_units = "m"
	elseif aunits == 2 then alt_multi = 3.28084; alt_units = "f"
	end
end

return {run=run_func, input=inputs}
