# ElsysProsjekt

**Main.ino**
Scriptet som kjøres

**Measurments.h/ino**
Hjelpefunksjoner for å ta målinger. takeMeasurment() returnerer struct Measurment med måledata for temperatur, turbiditet, konduktivitet og pH, samt en tellevariabel for å holde styr på hvilket målesett det er.
updatePayload() oppdaterer et dynamisk bytearray payload som inneholder måledataene.
readTemp(): leser spenning over thermistor og konverterer til byte. (per 20/2 ligger målt verdi et par grader over faktisk verdi)
readTurbidity(): leser turbiditetsverdi og konverterer til byte. (20/2: meget unøyaktig, men gir verdi rundt 0 for rent vann)
readConductivity(): ikke ferdig
readpH(): ferdig, men ikke kalibrert.


**Sleep.h/ino**
sleepInit(): set up watchdog timer, enable sleep mode, attach interrupt for wake up(ikke implementert) og setter digitale pins til lav.
sleep(nCycles): disable ADC og sover i nCycles sovesykluser på ~8sek
goodMorning(): enable ADC
