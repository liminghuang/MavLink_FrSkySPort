
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

  -- Initalise shared variables
  local function initShVars()
    shvars.prearmheading = 0
    shvars.watthours = 0
    shvars.LocationLat = 0
    shvars.LocationLon = 0
    shvars.pilotlat = 0
    shvars.pilotlon = 0
    shvars.is22 = false
    shvars.speedUnits = 1
    shvars.altUnits = 0
    shvars.apType= 0
    shvars.offsetmah = 0
    shvars.offsetwh = 0
    shvars.whCap = 0
  end

  -- Local OpenTx 2.2 checker function
  local function is22()
    local ver, radio, maj, minor, rev = getVersion()
    shvars.is22 = maj == 2 and minor == 2
  end

  --Init Timer 0 - runs while vehicle is armed
  local function setupTimers()
    model.setTimer(0, {mode=0, start=0, value=0, countdownBeep=0, minuteBeep=true, persistent=1})
    if model.getTimer(1).persistent == 2 and model.getTimer(1).start == 0 and model.getTimer(1).value>0 then --if timer 2 already exists
      model.setTimer(1, {mode=0, start=0, value=model.getTimer(1).value, countdownBeep=0, minuteBeep=false, persistent=2}) --keep existing value
    else
      model.setTimer(1, {mode=0, start=0, value=0, countdownBeep=0, minuteBeep=false, persistent=2}) --configure timer from scratch starting at 0
    end
  end

  --Read configuration file if one exists
  local function readCfg()
    local cfgFile = "/SCRIPTS/TELEMETRY/DATA/" .. model.getInfo().name .. ".cfg"
    local i, j
    local f = io.open(cfgFile, "r")
    if f ~= nil then
  		io.close(f)
      local cfgTab = loadfile(cfgFile)()
      if cfgTab ~= nil then
        for i, j in pairs(cfgTab) do
          shvars[i] = j
        end
      end
    else
      print("**************** no file exists **********************")
    end
  end

  remfuncs.runInit = function()
    initShVars()
    is22()
    setupTimers()
    readCfg()
  end
