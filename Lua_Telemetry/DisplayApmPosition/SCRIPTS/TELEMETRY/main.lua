
-- main lua part of of MavLink_FrSkySPort
--
-- Copyright (C) 2014 Luis Vale Gonçalves
--   https://github.com/lvale/MavLink_FrSkySPort
--
--  Improved by:
--    (2015) Michael Wolkstein
--   https://github.com/wolkstein/MavLink_FrSkySPort
--
--    (2015) Jochen Kielkopf
--    https://github.com/Clooney82/MavLink_FrSkySPort
--
--    (2016) Paul Atherton
--    https://github.com/Clooney82/MavLink_FrSkySPort
--
--   Recent changes include:
--   OpenTx 2.1.7 (and newer) compatibility (will not work with older 2.1 revisions). Separate repo for OpenTx 2.0.
--   Both Copter and Plane versions of Ardupilot now supported in this one telemetry script
--     Use offset.lua mixer script to choose between 1 = Copter, or 2 = Plane
--   Also includes several new unit display options selectable in offset.lua including:
--     SpeedUnits (to select units for ground and air speed) 1 = m/s, 2 = kph, 3 = mph
--     AltUnits (to select units of altitude) 1 = m (and m/s for climb rate), 2 = f (and f/s for climb rate)
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
-- Auxiliary files on github under dir BMP and SOUNDS/en
-- https://github.com/Clooney82/MavLink_FrSkySPort/tree/s-c-l-v-rc-opentx2.1/Lua_Telemetry/DisplayApmPosition

--Init Local Variables
  local SumFlight = 0
	local lastarmed = 0
	local apmarmed = 0
  local	WavSfx
	local FmodeNr
	local last_flight_mode = 1
	local last_apm_message_played = 0
	local t2 = 0
	local localtime = 0
	local oldlocaltime= 0
	local localtimetwo = 0
	local oldlocaltimetwo= 0
	local status_severity = 0
	local status_textnr = 0
  local isrunning_main = false
	local gpsLatLon = {}
--Init Flight Tables
  local flightMode = {{
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
  },
  {
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
  }}
  local shvars = {  --Init shared variables table
    prearmheading = 0,
    watthours = 0,
    LocationLat = 0,
    LocationLon = 0,
    pilotlat = 0,
    pilotlon = 0,
    is22 = false,
    speedUnits = 1,
    altUnits = 0,
    apType= 0,
    offsetmah = 0,
    offsetwh = 0,
    whCap = 0
  }
  local remfuncs = {}  --Init remote functions table
	local apm_status_message = {severity = 0, textnr = 0, timestamp=0}  --Init status message table
  local req_mainscr = true
  local scr_loaded = ""
  
  
  
--Setup Timer 0 - time while vehicle is armed
	model.setTimer(0, {mode=0, start=0, value=0, countdownBeep=0, minuteBeep=true, persistent=1})

--Clear table contents
  local function clearTable(tab_name)
    for i in pairs(tab_name) do
      tab_name[i] = nil
    end
    collectgarbage()
  end

------------------------------------------------
-- Background functions - always in use so 
-- located permanently here in main
------------------------------------------------

-- Calculate watthours
	local function calcWattHs()
		localtime = localtime + (getTime() - oldlocaltime)
		if localtime >=10 then --100 ms
			shvars.watthours = shvars.watthours + ( getValue("Watt") * (localtime/360000) )
			localtime = 0
		end
		oldlocaltime = getTime()
	end


--APM Armed and errors
	local function armed_status()
		t2 = getValue("Tmp2")
		apmarmed = t2%0x02
		gpsLatLon = getValue("GPS")
		if (type(gpsLatLon) == "table") then
			if gpsLatLon["lat"] ~= NIL then
				shvars.LocationLat = gpsLatLon["lat"]
			end
			if gpsLatLon["lon"] ~= NIL then
				shvars.LocationLon = gpsLatLon["lon"]
			end
		end
		if apmarmed ~=1 then -- report last heading bevor arming. this can used for display position relative to copter
			shvars.prearmheading=getValue("Hdg")
			shvars.pilotlat = math.rad(shvars.LocationLat)
			shvars.pilotlon = math.rad(shvars.LocationLon)
		end
		if lastarmed~=apmarmed then
			lastarmed=apmarmed
			if apmarmed==1 then
				model.setTimer(0,{ mode=1, start=0, value=SumFlight, countdownBeep=0, minuteBeep=true, persistent=1 })
				model.setTimer(1,{ mode=1, start=0, value=PersitentSumFlight, countdownBeep=0, minuteBeep=false, persistent=2 })
				playFile("/SOUNDS/en/TELEM/SARM.wav")
				playFile("/SOUNDS/en/TELEM/AVFM"..(FmodeNr-1)..WavSfx..".wav")
			else
				SumFlight = model.getTimer(0).value
				model.setTimer(0,{ mode=0, start=0, value=model.getTimer(0).value, countdownBeep=0, minuteBeep=true, persistent=1 })
				model.setTimer(1,{ mode=0, start=0, value=model.getTimer(1).value, countdownBeep=0, minuteBeep=false, persistent=2 })
				playFile("/SOUNDS/en/TELEM/SDISAR.wav")
			end
		end
		t2 = (t2-apmarmed)/0x02
		status_severity = t2%0x10
		t2 = (t2-status_severity)/0x10
		status_textnr = t2%0x400
		if(status_severity > 0) then
			if status_severity ~= apm_status_message.severity or status_textnr ~= apm_status_message.textnr then
				apm_status_message.severity = status_severity
				apm_status_message.textnr = status_textnr
				apm_status_message.timestamp = getTime()
			end
		end
		if apm_status_message.timestamp > 0 and (apm_status_message.timestamp + 2*100) < getTime() then
			apm_status_message.severity = 0
			apm_status_message.textnr = 0
			apm_status_message.timestamp = 0
			last_apm_message_played = 0
		end
		if apm_status_message.textnr >0 then -- play sound
			if last_apm_message_played ~= apm_status_message.textnr then
				playFile("/SOUNDS/en/TELEM/MSG"..apm_status_message.textnr..".wav")
				last_apm_message_played = apm_status_message.textnr
			end
		end
	end


--FlightModes
	local function Flight_modes()
	  if shvars.apType == 1 then
	    WavSfx = "A"
	  else
	    WavSfx = "P"
	  end
		FmodeNr = getValue("Fuel")+1
		if FmodeNr<1 or FmodeNr>#flightMode[shvars.apType+1] then
			if shvars.apType == 1 then
			  FmodeNr=13 -- This is an invalid flight number for Copter when no data available
			else
			  FmodeNr=10 -- This is an invalid flight number for Plane when no data available
			end
		end
		if FmodeNr~=last_flight_mode then
			playFile("/SOUNDS/en/TELEM/AVFM"..(FmodeNr-1)..WavSfx..".wav")
			last_flight_mode=FmodeNr
		end
	end


-- play alarm wh reach maximum level
	local function playMaxWhReached()
		if shvars.whCap > 0 and (shvars.watthours + shvars.watthours*shvars.offsetwh/100) >= shvars.whCap then
			localtimetwo = localtimetwo + (getTime() - oldlocaltimetwo)
			if localtimetwo >=800 then --8s
				playFile("/SOUNDS/en/TELEM/ALARM3K.wav")
				localtimetwo = 0
			end
			oldlocaltimetwo = getTime()
		end
	end

------------------------------------------------
--Init
------------------------------------------------
	local function init()
    --loadfile("/SCRIPTS/TELEMETRY/LIBRARY/maininit.lua")(shvars, remfuncs)
    --remfuncs.mainInit()
    --clearTable(remfuncs)
	  -- Local OpenTx 2.2 checker function
    local ver, radio, maj, minor, rev = getVersion()
    shvars.is22 = maj == 2 and minor == 2
  end

------------------------------------------------
--Background
------------------------------------------------
	local function background()
		armed_status()
		Flight_modes()
		calcWattHs()
		playMaxWhReached()
	end

------------------------------------------------
--Main
------------------------------------------------
	local function run(event)
		if event == EVT_MENU_BREAK then
      req_mainscr = not req_mainscr
    end
    if req_mainscr then
      if scr_loaded ~= "mainrun" then
        clearTable(remfuncs)
        loadfile("/SCRIPTS/TELEMETRY/LIBRARY/mainrun.lua")(shvars, remfuncs)
        scr_loaded = "mainrun"
      end
      remfuncs.mainRun(flightMode[shvars.apType+1][FmodeNr])
    else
      if scr_loaded ~= "confrun" then
        clearTable(remfuncs)
        loadfile("/SCRIPTS/TELEMETRY/LIBRARY/confrun.lua")(shvars, remfuncs)
        scr_loaded = "confrun"
      end
      remfuncs.confRun(event)
    end
  end

	return {init=init, run=run, background=background}
------------------------------------------------
--End of file
------------------------------------------------