# elektronisk systemdesign grunnkurs prosjekt

Prosjektet gikk ut på å lage noe som kunne heve stemningen i venteområdet for NRKs publikum.
Vår gruppe tok inspirasjon av publikumsoppvarmeren som blant annet bruker klapping til å varme opp publikum.
Vi bygde dermed et automatisk system som spiller av noen lydfiler som oppforder folk til å klappe
og gir seg når klappingen når et vist nivå. 

Elektronikken i dette systemet består av tre komponenter
- ESP32: for å styre led lys, analysere lyder fra mikrofonen og kjøre sekvensstyringen
- Rasberrypi: for å styre høytalleren som spiller av lydfilene

Videre er det tre filer i dette prosjektet
- main: kode for ESP32
- raspberry.py: kode for raspberry pi-en
- MCP3XXX.h: modifisert versjon av MCP3XXX.h med støtte for MCP3001 som er ADC'en som brukes i dette prosjektet for å lese av mikrofonen
