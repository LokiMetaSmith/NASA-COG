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
int myBackground = 64;

int recordCountUDP = 0; // count the received records through the UDP port.

void setup() {
  textAlign(CENTER);
  size(450, 255);
  background(myBackground);
  stroke(0);
  frameRate(30); // Slow it down a little
  //udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
  //udp = new UDP( this, 1900 );  // create a new datagram connection on port FIALS
  //  udp = new UDP( this, 161 );  // create a new datagram connection on port 6000  This worked with the Arduino Sketch on Due OEDCS SN11.
  udp = new UDP( this, 57575 );  // create a new datagram connection on same port as Geoff's
  udp.log( true );     // <-- printout the connection activity
  udp.listen( true );           // and wait for incoming message
}

void draw()
{
  background(myBackground);
  fill(191, 233, 255);
  text("Received from client record number: " + int(recordCountUDP), width/2, height/2);
}

void keyPressed() {
  //String ip       = "192.168.1.177";  // the remote IP address
  String ip       = "192.168.1.137";  // the OEDCS SN11 on Lee' LAN
  //String ip       = "192.168.1.118";  // the Desk Top Computer on Lee' LAN

  int port        = 8888;    // the destination port
  //          123456789012345678901234567890123456789
  //  udp.send("Hello World, from UDP_Server_LAN_B\n", ip, port );   // the message to send
  udp.send("UDP_Server_LAN_B", ip, port );   // the message to send
  myBackground = (120);
}

void receive( byte[] data ) {       // <-- default handler
  recordCountUDP++; 
  //void receive( byte[] data, String ip, int port ) {  // <-- extended handler
  print("Received from client record number: ");
  println(recordCountUDP);

  for (int i=0; i < data.length; i++)
    print(char(data[i]));
  println();
  myBackground = (0);
}
