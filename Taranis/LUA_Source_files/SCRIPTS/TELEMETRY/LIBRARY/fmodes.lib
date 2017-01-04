
-- fmodes.lua part of of MavLink_FrSkySPort
--		https://github.com/Clooney82/MavLink_FrSkySPort
--
-- created by Paul Atherton (c) 2016
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

local apType = ...

if apType == "copter" or apType == 0 then
  return {
    "Stabilize",
    "Acro",
    "Altitude Hold",
    "Auto",
    "Guided",
    "Loiter",
    "Return to launch",
    "Circle",
    "Invalid Mode",
    "Land",
    "Optical Loiter",
    "Drift",
    "Invalid Mode",
    "Sport",
    "Flip Mode",
    "Auto Tune",
    "Position Hold",
    "Brake"
    }
elseif apType == "plane" or apType == 1 then
  return {
    "Manual",
    "Circle",
    "Stabilize",
    "Training",
    "Acro",
    "Fly By Wire A",
    "Fly By Wire B",
    "Cruise",
    "Auto Tune",
    "Invalid Mode",
    "Auto",
    "Return to launch",
    "Loiter",
    "Invalid Mode",
    "Invalid Mode",
    "Guided"
  }
else
  return nil
end
