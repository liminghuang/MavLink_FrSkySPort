--
-- Service lua
--
-- Copyright (C) 2015 Michael Wolkstein
--
-- https://github.com/Clooney82/MavLink_FrSkySPort
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

local inputs = { {"Main Ser.", VALUE,0,156,156}, {"Serv 1", VALUE, 0, 156, 0}, {"Serv 2", VALUE, 0, 156, 0}, {"Serv 3", VALUE, 0, 156, 0}, {"Repeat", VALUE, 0, 10, 5}  }


local checkforserviceinterval = 1500 -- 15 seconds
local checkfortimetest = 100 -- 1 seconds
local timertwotime = 0
local checkfortimetesttime=0
local lastcheckfortimetesttime=0
local lastcheckfortimetest=0


local testone = 0
local testtwo = 0
local testthree = 0
local mainreached = 0

local function run_func(mainservice, serviceone, servicetwo, servicethree, repeats)

    timertwotime = model.getTimer(1).value
    checkfortimetesttime=getTime()

    -- each second
    if checkfortimetesttime-lastcheckfortimetest>=checkfortimetest then

        if serviceone > 0 and math.fmod(timertwotime, serviceone*3600) == 0 then
            testone = repeats
        end

        if servicetwo > 0 and math.fmod(timertwotime, servicetwo*3600) == 0 then
            testtwo = repeats
        end

        if servicethree > 0 and math.fmod(timertwotime, servicethree*3600) == 0 then
            testthree = repeats
        end

        if mainservice > 0 and timertwotime >= mainservice * 3600 then
           mainreached = repeats
        end

        lastcheckfortimetest = checkfortimetesttime
    end

    -- all 15 secons
    if checkfortimetesttime-lastcheckfortimetesttime>=checkforserviceinterval then

        if testone>0 then
            playFile("/SOUNDS/en/TELEM/ServOne.wav")
            testone = testone - 1

            -- uncomment next three lines (if than) to test timer function in a loop. set "service 1" to something modulo 10.
            -- than set yor local timer to 10h and start the timer
            -- if testone == 0 then
            --    model.setTimer(1,{ mode=1, start=0,value=model.getTimer(1).value -200, countdownBeep=0, minuteBeep=false, persistent=2 })
            -- end
        end

        if testtwo>0 then
            playFile("/SOUNDS/en/TELEM/ServTwo.wav")
            testtwo = testtwo - 1
        end

        if testthree>0 then
            playFile("/SOUNDS/en/TELEM/ServThre.wav")
            testthree = testthree - 1
        end

        if mainreached > 0 then
            playFile("/SOUNDS/en/TELEM/ServMain.wav")
            mainreached = mainreached -1

            -- reset servicetime
            if mainreached == 0 then
                model.setTimer(1,{ mode=0, start=0,value=0, countdownBeep=0, minuteBeep=false, persistent=2 })
            end
        end

        lastcheckfortimetesttime = checkfortimetesttime
    end

    return model.getTimer(1).value / 360
end



return {run=run_func, input=inputs , output={ "T2"}}
