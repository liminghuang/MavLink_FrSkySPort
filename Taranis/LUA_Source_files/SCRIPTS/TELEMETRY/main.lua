
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
  local FmodeNr
  local SumFlight = 0
	local lastArmed = 0
  local apmArmed = 0
  local	WavSfx
	local last_flight_mode = "init"
	local last_apm_message_played = 0
	local t2 = 0
	local localtime = 0
	local oldlocaltime= 0
	local localtimetwo = 0
	local oldlocaltimetwo= 0
	local status_severity = 0
	local status_textnr = 0
  local initRun = false
	local gpsLatLon = {}
  local flightMode = {}
  local shvars = {} -- Init shared variables table
  local apTypeOrig = 99 --Init to invalid value so FM run first time
	local apm_status_message = {severity=0, textnr=0, timestamp=0}  --Init status message table
  local req_mainscr = true
  local scr_loaded = ""

  --liming add
	local time_turbo = 0
	local voice_time = 0
	local old_voice_time = 0
	local take_turn = 1 -- 1:battery 2:height 3:speed 4:GPS
	local v_bat = 0
	local Va = 0
	local Vb = 0
	local gps_speed = 0
	local Gheight = 0
  local telem_t1 = 0

	local VoiceAction = function(Number)
		playFile("/SOUNDS/tw/" .. tostring(Number) .. ".wav")
	end


--Empty run_func will be populated later in run() function
  local run_func = function() end

--Load flight modes
  local function loadFModTab()
    if shvars.apType ~= apTypeOrig then
      flightMode = loadfile("/SCRIPTS/TELEMETRY/LIBRARY/fmodes.lib")(shvars.apType)
      apTypeOrig = shvars.apType
    end
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
		apmArmed = t2%0x02
		gpsLatLon = getValue("GPS")
		if (type(gpsLatLon) == "table") then
			if gpsLatLon["lat"] ~= NIL then
				shvars.LocationLat = gpsLatLon["lat"]
			end
			if gpsLatLon["lon"] ~= NIL then
				shvars.LocationLon = gpsLatLon["lon"]
			end
		end
		if apmArmed ~=1 then -- record heading and location before arming for radar home
			shvars.prearmheading=getValue("Hdg")
			shvars.pilotlat = math.rad(shvars.LocationLat)
			shvars.pilotlon = math.rad(shvars.LocationLon)
		end
    if lastArmed~=apmArmed then
			lastArmed=apmArmed
			if apmArmed==1 then
				model.setTimer(0,{mode=1})
				model.setTimer(1,{mode=1})
				playFile("/SOUNDS/en/TELEM/SARM.wav")
				playFile("/SOUNDS/en/TELEM/AVFM"..(FmodeNr-1)..WavSfx..".wav")
			else
				model.setTimer(0,{mode=0})
				model.setTimer(1,{mode=0})
				playFile("/SOUNDS/en/TELEM/SDISAR.wav")
			end
		end
		t2 = (t2-apmArmed)/0x02
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
    loadFModTab() --only reloads if apType has changed in config screen
    if shvars.apType == 0 then
	    WavSfx = "A"
	  else
	    WavSfx = "P"
	  end
		FmodeNr = getValue("Fuel")+1
		if FmodeNr<1 or FmodeNr>#flightMode then
			if shvars.apType == 0 then
			  FmodeNr=13 -- This is an invalid flight number for Copter when no data available
			else
			  FmodeNr=10 -- This is an invalid flight number for Plane when no data available
			end
		end
		if last_flight_mode~=flightMode[FmodeNr] then
      if last_flight_mode == "init" then
        last_flight_mode=flightMode[FmodeNr]
      else
			  playFile("/SOUNDS/en/TELEM/AVFM"..(FmodeNr-1)..WavSfx..".wav")
        last_flight_mode=flightMode[FmodeNr]
      end
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

  -- voice task
  	local function play_number(x)
  		if tonumber(x)>1000 then
  			play_number(tonumber(string.format("%u",x/1000)))
  			VoiceAction("thousand")
  			play_number(tonumber(string.format("%u",x%1000)))
  		elseif (x%1000)==0 then
  			VoiceAction(x/1000)
  			VoiceAction("hundred")
  		elseif x>100 then
  			play_number(tonumber(string.format("%u",x/100)))
  			VoiceAction("hundred")
  			play_number(tonumber(string.format("%u",x%100)))
  		elseif (x%100)==0 then
  			VoiceAction(x/100)
  			VoiceAction("hundred")
  		elseif x <10 then
  			VoiceAction(x)
  		elseif (x%10)==0 then
  			VoiceAction(x)
  		elseif x >90 then
  			VoiceAction(90)
  			VoiceAction(x%10)
  		elseif x >80 then
  			VoiceAction(80)
  			VoiceAction(x%10)
  		elseif x >70 then
  			VoiceAction(70)
  			VoiceAction(x%10)
  		elseif x >60 then
  			VoiceAction(60)
  			VoiceAction(x%10)
  		elseif x >50 then
  			VoiceAction(50)
  			VoiceAction(x%10)
  		elseif x >40 then
  			VoiceAction(40)
  			VoiceAction(x%10)
  		elseif x >30 then
  			VoiceAction(30)
  			VoiceAction(x%10)
  		elseif x >20 then
  			VoiceAction(20)
  			VoiceAction(x%10)
  		elseif x >10 then
  			VoiceAction(10)
  			VoiceAction(x%10)
  		end
  	end

  	local function speed_voice()
  		gps_speed = tonumber(string.format("%u", getValue("GSpd")*3.6))
  		if gps_speed>1 then
  			VoiceAction("speed")
  			if gps_speed<10 then
  				VoiceAction(gps_speed)
  			elseif gps_speed >=10 then
  				play_number(gps_speed)
  			end
  		else
  			VoiceAction("speed")
  			VoiceAction(0)
  		end
  	end

  	local function gps_voice()
      telem_t1 = getValue("Tmp1")
  		telem_sats = 0
  		telem_sats = (telem_t1 - (telem_t1%10))/10
  		if telem_sats>0 then
  			VoiceAction("gps")
  			if telem_sats<10 then
  				VoiceAction(telem_sats)
  			elseif telem_sats >=10 then
  				play_number(telem_sats)
  			end
  		else
  			VoiceAction("gps")
  			VoiceAction(0)
  		end
  	end

  	local function height_voice() --Alt
  		if(getValue("Alt")>0) then
  			VoiceAction("height")
  			Gheight = tonumber(string.format("%u", getValue("Alt")))
  			play_number(Gheight)
  			VoiceAction("meter")
  		end
  	end

  	local function battery_voice()
    		if(getValue("VFAS")>0) then
    			v_bat = string.format("%0.1f", getValue("VFAS"))
  	  		VoiceAction("battery")
  	  		Va,Vb = math.modf(v_bat)
  	  		play_number(Va)
  	  		if (Vb > 0) then
  	  			VoiceAction("dot")
  	  			VoiceAction((v_bat*10)%10)
  	  		end
  	 	end
  	end

  	local function voice_task()
  		if getValue("sd") < 1024 then --switch D control voice on/off
  			return 0
  		end
  		voice_time = voice_time + (getTime() - old_voice_time) + time_turbo
  	  	if voice_time >=850 then
  			if take_turn==1 then
  				time_turbo = 1
  				battery_voice()
  				take_turn = take_turn+1
  			elseif take_turn==2 then
  				time_turbo = 3
  				height_voice()
  				take_turn = take_turn+1
  			elseif take_turn==3 then
  				time_turbo = 3
  				gps_voice()
  				take_turn = take_turn+1
  			elseif take_turn==4 then
  				time_turbo = 3
  				speed_voice()
  				take_turn = 1
  			end
  			voice_time = 0
  		end
  		old_voice_time = getTime()
  	end

------------------------------------------------
--Init
------------------------------------------------
local function init()
  local init_func = loadfile("/SCRIPTS/TELEMETRY/LIBRARY/maininit.lib")(shvars)
  init_func()
  init_func = nil
  initRun = true
end

------------------------------------------------
--Background
------------------------------------------------
	local function background()
    if not initRun then init() end
		armed_status()
		Flight_modes()
		calcWattHs()
		playMaxWhReached()
    voice_task()
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
        run_func = loadfile("/SCRIPTS/TELEMETRY/LIBRARY/mainrun.lib")(shvars)
        scr_loaded = "mainrun"
      end
      run_func(flightMode[FmodeNr], apmArmed)
    else
      if scr_loaded ~= "confrun" then
        run_func = loadfile("/SCRIPTS/TELEMETRY/LIBRARY/confrun.lib")(shvars)
        scr_loaded = "confrun"
      end
      run_func(event)
    end
  end

	return {run=run, background=background}
------------------------------------------------
--End of file
------------------------------------------------
