/* UDP_Server_LAN_B
  A UDP Server Backup for the OEDCS connection on the local network
  
  Processing sketch to run with this example
 =====================================================

 // Processing UDP example to send and receive string data from Arduino
 // press any key to send the "Hello Arduino" message
 From: https://docs.arduino.cc/tutorials/ethernet-shield-rev2/udp-send-receive-string/
*/


 import hypermedia.net.*;

 UDP udp;  // define the UDP object


 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true );     // <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message
 }

 void draw()
 {
 }

 void keyPressed() {
 //String ip       = "192.168.1.177";  // the remote IP address
 //String ip       = "192.168.1.137";  // the OEDCS SN11 on Lee' LAN
 String ip       = "192.168.1.118";  // the Desk Top Computer on Lee' LAN
 
 int port        = 8888;    // the destination port

 udp.send("Hello World, from UDP_Server_LAN_B", ip, port );   // the message to send

 }

 void receive( byte[] data ) {       // <-- default handler
 //void receive( byte[] data, String ip, int port ) {  // <-- extended handler

 for(int i=0; i < data.length; i++)
 print(char(data[i]));
 println();
 }
 
