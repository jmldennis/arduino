/* This sketch was modified from the Web_Buzzer.pde - example sketch for Webduino library */
/* Find the WebServer.h file on Github.  The last time I ran this code was in 2011 */
/* 
 
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include "Ethernet.h"
#include "SPI.h"
#include "WebServer.h"
#include "Wire.h"

// no-cost stream operator as described at 
// http://sundial.org/arduino/?page_id=119
template<class T>
inline Print &operator <<(Print &obj, T arg)
{ obj.print(arg); return obj; }


/* CHANGE THIS TO YOUR OWN UNIQUE VALUE.  The MAC number should be
 * different from any other devices on your network or you'll have
 * problems receiving packets. */
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/* CHANGE THIS TO MATCH YOUR HOST NETWORK.  Most home networks are in
 * the 192.168.0.XXX or 192.168.1.XXX subrange.  Pick an address
 * that's not in use and isn't going to be automatically allocated by
 * DHCP from your router. */
static uint8_t ip[] = { 192, 168, 1, 64 };

/* all URLs on this server will start with /buzz because of how we
 * define the PREFIX value.  We also will listen on port 80, the
 * standard HTTP service port */
#define PREFIX ""
WebServer webserver(PREFIX, 80);

/* the piezo speaker on the Danger Shield is on PWM output pin #3 */
#define LED_PIN 2

/* this is the number of microseconds to wait after turning the
 * speaker on before turning it off. */
int Submit = 0, user = 0, pass = 0;
int LED_Toggle = 0, LED = 0;
int i = 0;

/* toggle is used to only turn on the speaker every other loop
iteration. */
char toggle = 0;

int buzzDelay = 0;
int Outlet1 = 0;
int Outlet2 = 0;
int Outlet3 = 0;
int Outlet4 = 0;
int Outlet5 = 0;
int Outlet6 = 0;
int Outlet7 = 0;
int Outlet8 = 0;

int Outlet1State = 0;
int Outlet2State = 0;
int Outlet3State = 0;
int Outlet4State = 0;
int Outlet5State = 0;
int Outlet6State = 0;
int Outlet7State = 0;
int Outlet8State = 0;

#define Outlet1Pin 2
#define Outlet2Pin 3
#define Outlet3Pin 4
#define Outlet4Pin 5
#define Outlet5Pin 6
#define Outlet6Pin 7
#define Outlet7Pin 8
#define Outlet8Pin 9


/* This command is set as the default command for the server.  It
 * handles both GET and POST requests.  For a GET, it returns a simple
 * page with some buttons.  For a POST, it saves the value posted to
 * the buzzDelay variable, affecting the output of the speaker */
void loginCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  Submit = 0;
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      /* readPOSTparam returns false when there are no more parameters
       * to read from the input.  We pass in buffers for it to store
       * the name and value strings along with the length of those
       * buffers. */
      repeat = server.readPOSTparam(name, 16, value, 16);
      

      /* this is a standard string comparison function.  It returns 0
       * when there's an exact match.  We're looking for a parameter
       * named "submit" here. */
      if (strcmp(name, "submit") == 0)
      {
	/* use the STRing TO Unsigned Long function to turn the string
	 * version of the delay number into our integer buzzDelay
	 * variable */
        Submit = strtoul(value, NULL, 10);
      }
      
       
     /* This compares the username and password submitted vs the
      * predefinded username and password string definded below
      * sets user and pass = 1 when correct and debugs to serial. */
      if (strcmp(name, "user") == 0)
      {
        if (strcmp(value, "username") == 0) //Edit this line for the username
        {
          user = 1;
          Serial.print("\n");
          Serial.print("Valid Username");
        }
        if (strcmp(value, "username") != 0) //Edit this line for the username
        {
          user = 0;
          Serial.print("\n");
          Serial.print("Invalid Username");
        }
      }  
      
      if (strcmp(name, "pass") == 0)
      {
        if (strcmp(value, "password") == 0)  // Edit this line for the password
        {      
          pass = 1;
          Serial.print("\n");
          Serial.print("Valid Password");
        }
        if (strcmp(value, "password") != 0)  //Edit this line for the password
        {
          pass = 0;
          Serial.print("\n");
          Serial.print("Invalid Password");
        }
      }
      
      
    } while (repeat);
    
    Serial.print("\n");
    Serial.print("Form Submitted");
    
    // after procesing the POST data, tell the web browser to reload
    // the page using a GET method if username and password are invalid.
    //GET next page if username and password were correct. 
    if (user == 1 && pass == 1)
    {
      server.httpSeeOther(PREFIX "/ControlPanel");
    }
    if (user != 1 || pass != 1)
    {
      server.httpSeeOther(PREFIX);
    }
    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" */
  server.httpSuccess();

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    /* store the HTML in program memory using the P macro */
    P(message) = 
      "<html><head><title>JMD's Arduino Server</title>"
      "<body>"
      "<h1>Authentication</h1>"
      "<b1>Unauthorized access is not permitted</b1>"
      "<form action='/' method='POST'>"
      "<p>Username  <input name='user' value=''></input></p>"
      "<p>Password   <input type='password' name='pass' value=''></input></p>"
      "<p><button name='submit' value='1'>Submit</input></p>"
      "</form></body></html>";

    server.printP(message);
  }
}


//Used to Set Pins High or Low depending on Settings
void controlCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
 if (user == 1 && pass == 1)
 {
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      /* readPOSTparam returns false when there are no more parameters
       * to read from the input.  We pass in buffers for it to store
       * the name and value strings along with the length of those
       * buffers. */
      repeat = server.readPOSTparam(name, 16, value, 16);

      /* this is a standard string comparison function.  It returns 0
       * when there's an exact match.  We're looking for a parameter
       * named "buzz" here. */
      if (strcmp(name, "led") == 0)
      {
	/* use the STRing TO Unsigned Long function to turn the string
	 * version of the delay number into our integer LED_Toggle
	 * variable */
        LED_Toggle = strtoul(value, NULL, 10);
      }
    } while (repeat);
    
    // after procesing the POST data, tell the web browser to reload
    // the page using a GET method. 
    server.httpSeeOther(PREFIX "/ControlPanel");
    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" */
  server.httpSuccess();

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    /* store the HTML in program memory using the P macro */
    P(message) = 
      "<html><head><title>JMD's Arduino Server</title>"
      "<body>"
      "<h1>*** Control Panel ***</h1>"
      "<b1>Power Outlets</b1>"
      "<form action='/ControlPanel' method='POST'>";
   
      server.printP(message);
      
      if (LED_Toggle == 10)
      {
        Outlet1State = 1; 
      }  
      if (LED_Toggle == 20)
      {
        Outlet1State = 0; 
      }
      if (LED_Toggle == 30)
      {
        Outlet2State = 1; 
      }
      if (LED_Toggle == 40)
      {
        Outlet2State = 0; 
      }
      if (LED_Toggle == 50)
      {
        Outlet3State = 1; 
      }  
      if (LED_Toggle == 60)
      {
        Outlet3State = 0; 
      }
      if (LED_Toggle == 70)
      {
        Outlet4State = 1; 
      }
      if (LED_Toggle == 80)
      {
        Outlet4State = 0; 
      }
      if (LED_Toggle == 90)
      {
        Outlet5State = 1; 
      }  
      if (LED_Toggle == 100)
      {
        Outlet5State = 0; 
      }
      if (LED_Toggle == 110)
      {
        Outlet6State = 1; 
      }
      if (LED_Toggle == 120)
      {
        Outlet6State = 0; 
      }
      if (LED_Toggle == 130)
      {
        Outlet7State = 1; 
      }  
      if (LED_Toggle == 140)
      {
        Outlet7State = 0; 
      }
      if (LED_Toggle == 150)
      {
        Outlet8State = 1; 
      }
      if (LED_Toggle == 160)
      {
        Outlet8State = 0; 
      }
      
      if (Outlet1State == 1)
      {
        server << "<p><button name='led' value='20'>Turn Outlet 1 Off</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 1 is ON");
      }
      else
      {
        server << "<p><button name='led' value='10'>Turn Outlet 1 On</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 1 is OFF");
      }
      
      if (Outlet2State == 1)
      {
        server << "<p><button name='led' value='40'>Turn Outlet 2 Off</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 2 is ON");
      }
      else
      {
        server << "<p><button name='led' value='30'>Turn Outlet 2 On</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 2 is OFF");
      }
      
      if (Outlet3State == 1)
      {
        server << "<p><button name='led' value='60'>Turn Outlet 3 Off</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 3 is ON");
      }
      else
      {
        server << "<p><button name='led' value='50'>Turn Outlet 3 On</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 3 is OFF");
      }
      
      if (Outlet4State == 1)
      {
        server << "<p><button name='led' value='80'>Turn Outlet 4 Off</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 4 is ON");
      }
      else
      {
        server << "<p><button name='led' value='70'>Turn Outlet 4 On</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 4 is OFF");
      }
      
      if (Outlet5State == 1)
      {
        server << "<p><button name='led' value='100'>Turn Outlet 5 Off</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 5 is ON");
      }
      else
      {
        server << "<p><button name='led' value='90'>Turn Outlet 5 On</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 5 is OFF");
      }
      
      if (Outlet6State == 1)
      {
        server << "<p><button name='led' value='120'>Turn Outlet 6 Off</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 6 is ON");
      }
      else
      {
        server << "<p><button name='led' value='110'>Turn Outlet 6 On</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 6 is OFF");
      }
      
      if (Outlet7State == 1)
      {
        server << "<p><button name='led' value='140'>Turn Outlet 7 Off</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 7 is ON");
      }
      else
      {
        server << "<p><button name='led' value='130'>Turn Outlet 7 On</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 7 is OFF");
      }
      
      if (Outlet8State == 1)
      {
        server << "<p><button name='led' value='160'>Turn Outlet 8 Off</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 8 is ON");
      }
      else
      {
        server << "<p><button name='led' value='150'>Turn Outlet 8 On</button></p>";
        Serial.print("\n");
        Serial.print("Outlet 8 is OFF");
      }
      
   
      /*Displays the Analog Values for the 6 Analog Pins
      server << "<br/>";
      server << "<p><b1>Analog Inputs</b1></p>";
      for (i = 0; i <= 5; ++i)
      {
        int val = analogRead(i);
        server << "Analog " << i << ": " << val << "<br/>";
      } */
      
      server << "</form></body></html>";

    server.printP(message);
  }
 }
}


void setup()
{
  //Serial Debugging
  Serial.begin(9600);
  Serial.print("*** Power On ***\n");
  Serial.print("Defining Buzzer Pin...");
  
  // set the PWM output for the buzzer to out
  pinMode(Outlet1Pin, OUTPUT);
  pinMode(Outlet2Pin, OUTPUT);
  pinMode(Outlet3Pin, OUTPUT);
  pinMode(Outlet4Pin, OUTPUT);
  pinMode(Outlet5Pin, OUTPUT);
  pinMode(Outlet6Pin, OUTPUT);
  pinMode(Outlet7Pin, OUTPUT);
  pinMode(Outlet8Pin, OUTPUT);
  
  digitalWrite(Outlet1Pin, LOW);
  digitalWrite(Outlet2Pin, LOW);
  digitalWrite(Outlet3Pin, LOW);
  digitalWrite(Outlet4Pin, LOW);
  digitalWrite(Outlet5Pin, LOW);
  digitalWrite(Outlet6Pin, LOW);
  digitalWrite(Outlet7Pin, LOW);
  digitalWrite(Outlet8Pin, LOW);

  Serial.print("DONE\n");
  Serial.print("Initalizing MAC and IP...");

  // setup the Ehternet library to talk to the Wiznet board
  Ethernet.begin(mac, ip);

  Serial.print("DONE\n");
  Serial.print("Register Default Command...");

  /* register our default command (activated with the request of
   * http://x.x.x.x/buzz */
  webserver.setDefaultCommand(&loginCmd);
  webserver.addCommand("ControlPanel", &controlCmd);

  Serial.print("DONE\n");
  Serial.print("Starting Server...");

  /* start the server to wait for connections */
  webserver.begin();
    
  Serial.print("DONE\n");
}

void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();

  // Toggle LED 
  if (Outlet1State == 1)
  {
    digitalWrite(Outlet1Pin, HIGH);
  }
  if (Outlet1State == 0)
  {
    digitalWrite(Outlet1Pin, LOW);
  }
  
  if (Outlet2State == 1)
  {
    digitalWrite(Outlet2Pin, HIGH);
  }
  if (Outlet2State == 0)
  {
    digitalWrite(Outlet2Pin, LOW);
  }
  
  if (Outlet3State == 1)
  {
    digitalWrite(Outlet3Pin, HIGH);
  }
  if (Outlet3State == 0)
  {
    digitalWrite(Outlet3Pin, LOW);
  }
  
  if (Outlet4State == 1)
  {
    digitalWrite(Outlet4Pin, HIGH);
  }
  if (Outlet4State == 0)
  {
    digitalWrite(Outlet4Pin, LOW);
  }
  
  if (Outlet5State == 1)
  {
    digitalWrite(Outlet5Pin, HIGH);
  }
  if (Outlet5State == 0)
  {
    digitalWrite(Outlet5Pin, LOW);
  }
  
  if (Outlet6State == 1)
  {
    digitalWrite(Outlet6Pin, HIGH);
  }
  if (Outlet6State == 0)
  {
    digitalWrite(Outlet6Pin, LOW);
  }
  
  if (Outlet7State == 1)
  {
    digitalWrite(Outlet7Pin, HIGH);
  }
  if (Outlet7State == 0)
  {
    digitalWrite(Outlet7Pin, LOW);
  }
  
/*  if (Outlet8State == 1)
  {
    digitalWrite(Outlet8Pin, HIGH);
  }
  if (Outlet8State == 0)
  {
    digitalWrite(Outlet8Pin, LOW);
  }*/
  
 // }
}
