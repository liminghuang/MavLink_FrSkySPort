
-- mainrun lua part of of MavLink_FrSkySPort
--
-- Copyright (C) 2014 Luis Vale  Gonçalves
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
--   relocation of this code to mainrun.lua - allowing for unloading of
--   the telemetry screen from memory to facilitate running of the options screen.
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

-- Local Vars
  local hypdist = 0
  local whconsumed = 0
  local mult = 0
  local consumption = 0
  local dispTxt = ""
  local xposCons = 0
  local vspd = 0
  local altMax = 0
  local radarx = 0
  local radary = 0
  local radarxtmp = 0
  local radarytmp = 0
  local curlat = 0
  local curlon = 0
  local hdop = 0
  local telem_sats = 0
	local telem_lock = 0
	local telem_t1 = 0
  local speed_multi = 0
	local speed_units = ""
  local alt_multi = 0
	local alt_units = ""
  local units_set = false
  local arrowLine = {
		{-4, 5, 0, -4},
		{-3, 5, 0, -3},
		{3, 5, 0, -3},
		{4, 5, 0, -4}
	}
  local shvars, remfuncs = ... -- Setup access to shared tables passed by main.lua

-- Temporary text attribute
  --local FORCE = 0x02 -- draw ??? line or rectangle
  local X1 = 0
  local Y1 = 0
  local X2 = 0
  local Y2 = 0
  local sinCorr = 0
  local cosCorr = 0
  local radTmp = 0
  local CenterXcolArrow = 189
  local CenterYrowArrow = 41
  local offsetX = 0
  local offsetY = 0
  local scrW = 212 -- width of Taranis screen
  local scrH = 63 -- height of Taranis screen
  local divtmp = 1
  local upppp = 20480
  local divvv = 2048 --12 mal teilen

--------------------------
-- Local support functions
--------------------------
	local function round(num, idp)
		mult = 10^(idp or 0)
		return math.floor(num * mult + 0.5) / mult
	end


local function setUnits()
  if not units_set then
    if shvars.speedUnits == 1 then --lookup speed units and multi
      speed_multi = 3.6
      speed_units = "kph"
    elseif shvars.speedUnits == 2 then
      speed_multi = 2.23694
      speed_units = "mph"
    else --assume 0
      speed_multi = 1
      speed_units = "m/s"
    end
    if shvars.altUnits == 1 then  --lookup alt units and multi
      alt_multi = 3.28084
      alt_units = "f"
    else --assume 0
      alt_multi = 1
      alt_units = "m"
    end
    units_set=true
  end
end

--------------------------------------------------------------------------
--Main run section functions made available to main.lua via remfuncs table
--------------------------------------------------------------------------
  local function toppanel(fMode, apmArmed)
		lcd.drawFilledRectangle(0, 0, scrW, 9, 0)
		if apmArmed==1 then
			lcd.drawText(1, 0, fMode, INVERS)
		else
			lcd.drawText(1, 0, fMode, INVERS+BLINK)
		end
		lcd.drawText(92, 0, "TxBat:", INVERS)
		lcd.drawNumber(lcd.getLastPos()+2, 0, getValue(189)*10,0+PREC1+INVERS+LEFT)
		lcd.drawText(lcd.getLastPos(), 0, "v", INVERS+SMLSIZE)
    if getValue("A3")>0 or getValue("RSSI") >0 then -- check if any RSSI
      if getValue("A4")==0 then -- using Mavlink RSSI
			  dispTxt="rx-rssi:" .. tostring(math.ceil(getValue("A3")))
        lcd.drawText(scrW-1-string.len(dispTxt)*5.1, 0, dispTxt , INVERS)
		  else -- using regular FrSky RSSI
        dispTxt="rssi:" .. tostring(getValue("RSSI"))
        lcd.drawText(scrW-1-string.len(dispTxt)*5.1, 0, dispTxt , INVERS)
		    lcd.drawNumber(lcd.getLastPos()+2, 0, getValue("RSSI"),0+INVERS+LEFT)
		  end
    else
      dispTxt="rssi:n/a"
      lcd.drawText(scrW-2-string.len(dispTxt)*5.1, 0, dispTxt , INVERS+BLINK)
    end
	end


	local function powerpanel()
		consumption=getValue("mAh")---

		lcd.drawNumber(4,10,getValue("VFAS")*10,MIDSIZE+PREC1+LEFT)
		lcd.drawText(lcd.getLastPos(),14,"V",0)

    if shvars.is22 then
      lcd.drawNumber(61,10,getValue("Cmin")*100,MIDSIZE+PREC2+RIGHT)
    else
      lcd.drawNumber(61,10,getValue("Cmin")*100,MIDSIZE+PREC2)
    end
		xposCons=lcd.getLastPos()
		lcd.drawText(xposCons,9,"c-",SMLSIZE)
		lcd.drawText(xposCons,15,"min",SMLSIZE)
		lcd.drawNumber(4,24,getValue("Curr")*10,MIDSIZE+PREC1+LEFT)
		lcd.drawText(lcd.getLastPos(),28,"A",0)

    if shvars.is22 then
      lcd.drawNumber(66,24,consumption + (consumption*shvars.offsetmah/100),MIDSIZE+RIGHT)
    else
      lcd.drawNumber(66,24,consumption + (consumption*shvars.offsetmah/100),MIDSIZE)
    end
		xposCons=lcd.getLastPos()
		lcd.drawText(xposCons,24,"m",SMLSIZE)
		lcd.drawText(xposCons,29,"Ah",SMLSIZE)
		lcd.drawNumber(1,38,getValue("Watt"),MIDSIZE+LEFT)
		lcd.drawText(lcd.getLastPos(),42,"W",0)

    if shvars.is22 then
      lcd.drawNumber(65,43,(shvars.watthours + (shvars.watthours*shvars.offsetwh/100))*10,SMLSIZE+PREC1+RIGHT)
    else
      lcd.drawNumber(65,43,(shvars.watthours + (shvars.watthours*shvars.offsetwh/100))*10,SMLSIZE+PREC1)
    end
		lcd.drawText(lcd.getLastPos(),43,"Wh",SMLSIZE)
		lcd.drawLine(0,53,75,53,SOLID,0)
		lcd.drawText(1,56,"ArmT",SMLSIZE)
		lcd.drawTimer(lcd.getLastPos()+2,56,model.getTimer(0).value,SMLSIZE)

    if shvars.is22 then
      lcd.drawNumber(71,56,model.getTimer(1).value/360,SMLSIZE+PREC1+RIGHT)
    else
      lcd.drawNumber(71,56,model.getTimer(1).value/360,SMLSIZE+PREC1)
    end
		lcd.drawText(lcd.getLastPos(),56,"h",SMLSIZE)
	end


  local function htsapanel()
    local altNow = 0
		lcd.drawLine (scrW-47,9,scrW-47,scrH,SOLID,0)
		--Heading & Alt headers
		lcd.drawText(85,10,"Alt",SMLSIZE)
		lcd.drawText(125,10,"Hdg ",SMLSIZE)
		lcd.drawLine(123,30,164,30,DOTTED,0)
		lcd.drawLine(84,40,164,40,DOTTED,0)
		lcd.drawLine(123,9,123,29,DOTTED,0)
    --Alt
    altNow = getValue("Alt")
    lcd.drawNumber(87,18,altNow*alt_multi,MIDSIZE+LEFT)
		lcd.drawText(lcd.getLastPos(),23,alt_units,SMLSIZE)
    if altNow > altMax then
      altMax = altNow
    end
		--Heading
		lcd.drawNumber(127,18,getValue("Hdg"),MIDSIZE+LEFT)
		lcd.drawText(lcd.getLastPos(),18,"\64",MIDSIZE)
		--vspeed
		vspd= getValue("VSpd")
		if vspd == 0 then
			lcd.drawText(88,32,"==",0+SMLSIZE)
		elseif vspd >0 then
			lcd.drawText(87,32,"++",0+SMLSIZE)
		elseif vspd <0 then
			lcd.drawText(88,32,"--",0+SMLSIZE)
      vspd = -vspd
		end
		lcd.drawNumber(99,32,vspd*alt_multi,0+SMLSIZE+LEFT)
    lcd.drawText(lcd.getLastPos(),32,alt_units .. "/s",SMLSIZE)
		lcd.drawNumber(128,32,altMax*alt_multi,SMLSIZE+LEFT)
		lcd.drawText(lcd.getLastPos(),32,alt_units .. " max",SMLSIZE)

		lcd.drawText(85,43,"GSpd",SMLSIZE)
		lcd.drawText(125,43,"ASpd",SMLSIZE)
		lcd.drawNumber(87,51,getValue("GSpd")*speed_multi,MIDSIZE+LEFT)
		lcd.drawText(lcd.getLastPos(),56,speed_units,SMLSIZE)
		lcd.drawNumber(127,51,getValue("ASpd")*speed_multi,MIDSIZE+LEFT)
		lcd.drawText(lcd.getLastPos(),56,speed_units,SMLSIZE)

	end


	local function gpspanel()
		telem_t1 = getValue("Tmp1") -- Temp1
		telem_lock = 0
		telem_sats = 0
		telem_lock = telem_t1%10
		telem_sats = (telem_t1 - (telem_t1%10))/10
		if telem_lock >= 3 then
			lcd.drawText (168, 10, "3D",0)
			lcd.drawNumber (195, 10, telem_sats, 0+LEFT)
			lcd.drawText (lcd.getLastPos(), 10, "S", 0)
		elseif telem_lock>1 then
			lcd.drawText (168, 10, "2D", 0)
			lcd.drawNumber (195, 10, telem_sats, 0+LEFT )
			lcd.drawText (lcd.getLastPos(), 10, "S", 0)
		else
			lcd.drawText (168, 10, "NO", 0+BLINK+INVERS)
			lcd.drawText (195, 10, "--S",0)
		end
		hdop=round(getValue("A2"))/10
		if hdop <2.5 then
			lcd.drawNumber (180, 10, hdop*10, PREC1+LEFT+SMLSIZE )
		else
			lcd.drawNumber (180, 10, hdop*10, PREC1+LEFT+BLINK+INVERS+SMLSIZE)
		end
		curlat = math.rad(shvars.LocationLat)
		curlon = math.rad(shvars.LocationLon)
		if shvars.pilotlat~=0 and curlat~=0 and shvars.pilotlon~=0 and curlon~=0 then
			z1 = math.sin(curlon - shvars.pilotlon) * math.cos(curlat)
			z2 = math.cos(shvars.pilotlat) * math.sin(curlat) - math.sin(shvars.pilotlat) * math.cos(curlat) * math.cos(curlon - shvars.pilotlon)
			-- use prearmheading later to rotate cordinates relative to copter.
			radarx=z1*6358364.9098634 -- meters for x absolut to center(homeposition)
			radary=z2*6358364.9098634 -- meters for y absolut to center(homeposition)
			hypdist =  math.sqrt(math.pow(math.abs(radarx),2) + math.pow(math.abs(radary),2))
			radTmp = math.rad(shvars.prearmheading)
			radarxtmp = radarx * math.cos(radTmp) - radary * math.sin(radTmp)
			radarytmp = radarx * math.sin(radTmp) + radary * math.cos(radTmp)
			if math.abs(radarxtmp) >= math.abs(radarytmp) then
				for i = 13 ,1,-1 do
					if math.abs(radarxtmp) >= upppp then
						divtmp=divvv
						break
					end
					divvv = divvv/2
					upppp = upppp/2
				end
			else
				for i = 13 ,1,-1 do
					if math.abs(radarytmp) >= upppp then
						divtmp=divvv
						break
					end
					divvv = divvv/2
					upppp = upppp/2
				end
			end
			upppp = 20480
			divvv = 2048 --12 mal teilen
			offsetX = radarxtmp / divtmp
			offsetY = (radarytmp / divtmp)*-1
		end
		lcd.drawText(187,37,"o",0)
		lcd.drawRectangle(167, 19, 45, 45)
		for j=169, 209, 4 do
			lcd.drawPoint(j, 19+22)
		end
		for j=21, 61, 4 do
			lcd.drawPoint(167+22, j)
		end
    if shvars.is22 then
      lcd.drawNumber(189, 57,hypdist*alt_multi, SMLSIZE+RIGHT)
    else
      lcd.drawNumber(189, 57,hypdist*alt_multi, SMLSIZE)
    end
		lcd.drawText(lcd.getLastPos(), 57, alt_units, SMLSIZE)
	end


	local function drawWhGauge()
		whconsumed = shvars.watthours + (shvars.watthours*shvars.offsetwh/100)
		if whconsumed > shvars.whCap then
			whconsumed = shvars.whCap
		end
		lcd.drawFilledRectangle(76,9,8,55,INVERS)
    lcd.drawFilledRectangle(77,9,6,whconsumed*55/shvars.whCap, 0)
	end


	local function drawArrow()
		sinCorr = math.sin(math.rad(getValue("Hdg")-shvars.prearmheading))
		cosCorr = math.cos(math.rad(getValue("Hdg")-shvars.prearmheading))
		for index, point in pairs(arrowLine) do
			X1 = CenterXcolArrow + offsetX + math.floor(point[1] * cosCorr - point[2] * sinCorr + 0.5)
			Y1 = CenterYrowArrow + offsetY + math.floor(point[1] * sinCorr + point[2] * cosCorr + 0.5)
			X2 = CenterXcolArrow + offsetX + math.floor(point[3] * cosCorr - point[4] * sinCorr + 0.5)
			Y2 = CenterYrowArrow + offsetY + math.floor(point[3] * sinCorr + point[4] * cosCorr + 0.5)
			if X1 == X2 and Y1 == Y2 then
				lcd.drawPoint(X1, Y1, SOLID, FORCE)
			else
				lcd.drawLine (X1, Y1, X2, Y2, SOLID, FORCE)
			end
		end
	end

remfuncs.runMain = function(fMode, apmArmed)
  lcd.clear()
  setUnits()
  toppanel(fMode, apmArmed)
  powerpanel()
  htsapanel()
  gpspanel()
  drawWhGauge()
  drawArrow()
end
--------------
-- End of file
--------------
