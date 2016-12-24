
-- maininit.lua part of of MavLink_FrSkySPort
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

-- Setup shared tables
  local shvars, remfuncs = ...

--  check if globals are set and if not set defaults
--  local function checkGlobals()
--    if gSpeed_multi == nil then gSpeed_multi = 3.6; gSpeed_units = "kph" end
--    if gAlt_multi == nil then gAlt_multi = 1; gAlt_units = "m" end
--    if gOffsetmah == nil then gOffsetmah = 0 end
--    if gOffsetwatth == nil then gOffsetwatth = 0 end
--    if gBatcapwh == nil then gBatcapwh = 30 end
--    if gAPType == nil then gAPType = 1 end
--  end
  
  -- Local OpenTx 2.2 checker function
  local function is22()
    local ver, radio, maj, minor, rev = getVersion()
    shvars.is22 = maj == 2 and minor == 2
  end
  
  remfuncs.mainInit = function()
    --checkGlobals()
    is22()
  end