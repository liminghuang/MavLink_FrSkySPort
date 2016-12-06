	local function run(event)
		lcd.clear()
		lcd.drawText(1,1,"GSpd: ",MIDSIZE)
		lcd.drawNumber(lcd.getLastPos(),1,getValue("GSpd")*10,MIDSIZE+LEFT+PREC1)
		lcd.drawText(1,20,"ASpd: ",MIDSIZE)
		lcd.drawNumber(lcd.getLastPos(),20,getValue("ASpd")*10,MIDSIZE+LEFT+PREC1)
	end

	return {run=run}
