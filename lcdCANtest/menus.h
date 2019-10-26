menu 0 {
  waterTemp = getWaterTemp();
  RPM = getRPM();
  MPH = getMPH();
  fuelLevel = getFuelLevel();
  
	RPM				waterTemp
	MPH				fuelLevel
}

menu 1 {
  ambientAirTemp = getAmbientAir();
  iat = getIAT();
  barometric = getBaro();
  manifAbsPres = getMAP();
  
	ambientAirTemp [C]		iat [C]
	barometric [kPa]		manifAbsPres [kPa]
}

menu 2 {
  throttle = getThrottle();
  MAF = getMAF();
  engineFuelRate = getFuelRate();
  O2Volts = getO2Volts();

	throttle [%]			MAF [g/s]
	engineFuelRate [gal/hr]	O2Volts [V]
}

menu 3 {
	  MPH = getMPH();
	MPG
}

