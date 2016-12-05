--
-- txtmsg.lua part of of MavLink_FrSkySPort
--		https://github.com/Clooney82/MavLink_FrSkySPort
--
-- Copyright (C) (2015) Jochen Kielkopf
--    https://github.com/Clooney82/MavLink_FrSkySPort
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
----------------------------------------------------------------------------------
-- Date of last Change: 2015/10/30
-- Version: 0.21
----------------------------------------------------------------------------------
local lastTime = 0
local messageBuffer = ""
local messageBufferSize = 0
local previousMessageWord = 0
local messagePriority = -1
local MESSAGEBUFFERSIZE = 5
local messageArray = {}
local messageFirst = 0
local messageNext = 0
local messageLatestTimestamp = 0
local AsciiMap = {}
----------------------------------------------------------------------------------
-- Char conversion function
----------------------------------------------------------------------------------
local function char(c)
    if c >= 48 and c <= 57 then
      return "0" + (c - 48)
    elseif c >= 65 and c <= 90 then
      return AsciiMap[c - 64]
    elseif c >= 97 and c <= 122 then
      return AsciiMap[c - 96]
    elseif c == 32 then
      return " "
    elseif c == 46 then
      return "."
    else
      return ""
    end
end


----------------------------------------------------------------------------------
-- get new TextMessage function
----------------------------------------------------------------------------------
local function getTextMessage()
    local returnValue = ""
    local messageWord = getValue("MSG") / 2   -- correct value for opentx 2.1.x

    if messageWord ~= previousMessageWord then
        local highByte = bit32.rshift(messageWord, 7)
        highByte = bit32.band(highByte, 127)
        local lowByte = bit32.band(messageWord, 127)

        if highByte ~= 0 then
            if highByte >= 48 and highByte <= 57 and messageBuffer == "" then
                messagePriority = highByte - 48
            else
              messageBuffer = messageBuffer .. char(highByte)
              messageBufferSize = messageBufferSize + 1
            end
            if lowByte ~= 0 then
                messageBuffer = messageBuffer .. char(lowByte)
                messageBufferSize = messageBufferSize + 1
            end
        end
        if highByte == 0 or lowByte == 0 then
          returnValue = messageBuffer
          messageBuffer = ""
          messageBufferSize = 0
        end
        previousMessageWord = messageWord
    end
    return returnValue
end

----------------------------------------------------------------------------------
-- get latest TextMessage function
----------------------------------------------------------------------------------
local function getLatestMessage()
    if messageFirst == messageNext then
        return ""
    end
    return messageArray[((messageNext - 1) % MESSAGEBUFFERSIZE) + 1]
end

----------------------------------------------------------------------------------
-- check for new TextMessage function
----------------------------------------------------------------------------------
local function checkForNewMessage()
    local msg = getTextMessage()
    if msg ~= "" then
        if msg ~= getLatestMessage() then
            messageArray[(messageNext % MESSAGEBUFFERSIZE) + 1] = msg
            messageNext = messageNext + 1
            if (messageNext - messageFirst) >= MESSAGEBUFFERSIZE then
                messageFirst = messageNext - MESSAGEBUFFERSIZE
            end
            messageLatestTimestamp = getTime()
        end
    end
end

local function init()
    AsciiMap = {
    		[1] ="A",
    		[2] ="B",
    		[3] ="C",
    		[4] ="D",
    		[5] ="E",
    		[6] ="F",
    		[7] ="G",
    		[8] ="H",
    		[9] ="I",
    		[10] ="J",
    		[11] ="K",
    		[12] ="L",
    		[13] ="M",
    		[14] ="N",
    		[15] ="O",
    		[16] ="P",
    		[17] ="Q",
    		[18] ="R",
    		[19] ="S",
    		[20] ="T",
    		[21] ="U",
    		[22] ="V",
    		[23] ="W",
    		[24] ="X",
    		[25] ="Y",
    		[26] ="Z"
    }
end

local function background()
  local loopStartTime = getTime()
--  if loopStartTime > (lastTime + 500) then
  if loopStartTime > (lastTime + 100) then
      checkForNewMessage()
      lastTime = loopStartTime
  end
end

local function run()
  background()
  lcd.clear()
  --toppanel()
  checkForNewMessage()
  local i
  local row = 1
  lcd.drawFilledRectangle(0,0,212,10)
  lcd.drawText(1, 1, "Status Messages (newest at bottom):", 0+INVERS) 
  for i = messageFirst, messageNext - 1, 1 do
    --lcd.drawText(1, row * 10 + 2, "abc " .. i .. " " .. messageFirst .. " " .. messageNext, 0)
    lcd.drawText(1, row * 10 + 2, messageArray[(i % MESSAGEBUFFERSIZE) + 1], 0)
    row = row + 1
  end
end

return {init=init, run=run, background=background}
