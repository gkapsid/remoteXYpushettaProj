# remoteXYpushettaProj
An IoT project where RemoteXY is used for controlling from mobile, and pushetta is used to push alarm notification
Project Bill of Materials
1 ESP8266-12
1 sensor
1 output (like indicative LED, sound etc (ενδεικτική λυχνία, σειρίνα για να δείξει το alarm στο χώρο που συμβαίνει)
1 5V power supply

Operation description
The ESP8266 tries to connect as a client to the local router. If this is impossible then it uses the wifimanager (https://github.com/tzapu/WiFiManager) to allow for an easy configuration of the SSID and password.
If everything is OK it connects to RemoteXY as a client. 
If an alarm occures then:
1. diconnects from RemoteXY
2. Sets ON a local alarm (like a sound from a sirene) and sends a trigger to pushetta (http://www.pushetta.com).
3. After a few seconds it connects again to RemoteXY and the user can set the alarm OFF from an internet connected device(phone, laptop, tablet).

For any new alarm occurence the same procedure is followed.

