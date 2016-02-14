# MQTTPumpControllerV2
Pump Controller project using Arduino and MQTT. 
This Arduino project is to sense water level of an over head water tank and controll a pump that fills it.

The tank is fitted with float swiches at every quarter mark. These switches will go high once water level reaches it.
These switches are connecte to the arduino which monitors the switch status and publish a corresponding message via MQTT to a 
MQTT broker (Mosquitto) in the network.

The Switches and pump are also registered as items in OpenHab instance in the network. Once the water level falls belwo 3/4th mark, 
a 'rule' in OpenHab send a MQTT message for the Pump to be turned on. As the water level raises to Full mark, OpenHab sends a turn off 
the pump message to Arduino which will turn it off. There by maintaining the water level between 3/4th and Full levels.

As a extra level of safety, turn the pump off locally from Arduino when water level reaches full.
There had been instances where network connectivity vi ethernet shield drops. In such cases, using arduino interrupt 0 to sense rising 
voltage for Full Float Switch and turn the pump off. 

The pump is controlled by a Solid State Relay rated at 40 Amps, the motor is rated at 1HP.