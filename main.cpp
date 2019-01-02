//Code adapted from ArtNode by Suga Koubou 
//https://os.mbed.com/users/okini3939/code/ArtNode/file/e3d0bc58141e/main.cpp/





#include "mbed.h"
#include "EthernetInterface.h"
#include "ArtNet.h"
#include "NeoStrip.h"
#include "gt.h"

#define STATIC
#define DNS_SERVER_ADDRESS(ipaddr)        (ip4_addr_set_u32(ipaddr, ipaddr_addr("192.168.1.254")))
#define ECHO_SERVER_PORT   0x1936
#define DATA_PIN p18
#define LED_COUNT 300
 
 
const char *ipAddr = "192.168.1.130";
const char *snMask = "255.255.255.0";
const char *gwAddr = "192.168.1.254";
int okToWrite;
//******************************************Art-Net Stuff*******************************
extern "C" void mbed_mac_address(char *s);
struct ArtPacketHeader ArtHead;
struct ArtDMX_Packet ArtDMX;
struct ArtPoll_Packet ArtPoll;
struct ArtAddr localaddr;
struct ArtPollReply_Packet ArtPollReply;
int rxlen = 0;
int LastRecievedUniverse;

NeoStrip strip(DATA_PIN, LED_COUNT);

void InitArtPollReplyDefaults () {
   memcpy(ArtPollReply.ID, ArtHeaderID, 8);
   ArtPollReply.OpCode = OP_PollReply; // reply packet
   ArtPollReply.Version = ArtVersion;

   memcpy(&ArtPollReply.Addr, &localaddr, sizeof(localaddr));
   strncpy(ArtPollReply.ShortName, STR_ShortName, 18);
   strncpy(ArtPollReply.LongName, STR_LongName, 64);
   strncpy(ArtPollReply.NodeReport, "OK", 64);
   ArtPollReply.Style = StyleNode;

   mbed_mac_address((char*)&ArtPollReply.Mac);

   ArtPollReply.NumPortsH = 0;
   ArtPollReply.NumPorts = 2;
   ArtPollReply.Swout[0] = 1;
   ArtPollReply.Swout[1] = 2;
   ArtPollReply.Swout[2] = 0;
   ArtPollReply.Swout[3] = 0;
   ArtPollReply.Swin[0] = 0;
   ArtPollReply.Swin[1] = 0;
   ArtPollReply.Swin[2] = 0;
   ArtPollReply.Swin[3] = 0;

   ArtPollReply.GoodOutput[0] = 0;
   ArtPollReply.GoodOutput[1] = 0;
   ArtPollReply.GoodOutput[2] = 0;
   ArtPollReply.GoodOutput[3] = 0;
   ArtPollReply.PortType[0] = 0x40; // bit7:Can output from Art-Net network, bit0-5: 0 - type = DMX512 
   ArtPollReply.PortType[1] = 0x40;
   ArtPollReply.PortType[2] = 255;
   ArtPollReply.PortType[3] = 255;
  
 } 
  
   
void Init_ArtDMX ()
 {
   // header stuff
   okToWrite = 0;
   memcpy(ArtDMX.ID, ArtHeaderID, 8);
   ArtDMX.OpCode = OP_Output;
   ArtDMX.Version = ArtVersion;
   // dmx stuff
   ArtDMX.Universe = 0; // this is the destination for the dmx
   ArtDMX.Sequence = 0;
   ArtDMX.Length = htons(512);
}
// Converts HSV to RGB with the given hue, assuming
// maximum saturation and value
int hueToRGB(float h)
{
    // lots of floating point magic from the internet and scratching my head
    float r, g, b;
    if (h > 360)
        h -= 360;
    if (h < 0)
        h += 360;
    int i = (int)(h / 60.0);
    float f = (h / 60.0) - i;
    float q = 1 - f;
    
    switch (i % 6)
    {
        case 0: r = 1; g = f; b = 0; break;
        case 1: r = q; g = 1; b = 0; break;
        case 2: r = 0; g = 1; b = f; break;
        case 3: r = 0; g = q; b = 1; break;
        case 4: r = f; g = 0; b = 1; break;
        case 5: r = 1; g = 0; b = q; break;
        default: r = 0; g = 0; b = 0; break;
    }
    
    // scale to integers and return the packed value
    uint8_t R = (uint8_t)(r * 255);
    uint8_t G = (uint8_t)(g * 255);
    uint8_t B = (uint8_t)(b * 255);
 
    return (R << 16) | (G << 8) | B;
}

int makeword16 (int lsb, int msb)
{
    return (msb << 8) + lsb;
}
//*************************************************************************************


 
    


void thread1(void const *args)
{
    
    while(true) 
    {  
   /* static float dh = 360.0 / LED_COUNT;
    static float x = 0;
 
    for (int i = 0; i < LED_COUNT; i++)
        strip.setPixel(i, hueToRGB((dh * i) - x));
    
    x += 1;
    if (x > 360)
        x = 0;*/
   
    
        strip.write();
    
    Thread::wait(10);      
    }
}   


int main() {
    strip.setBrightness(1.0);    // set default brightness
    strip.setPixels(0, LED_COUNT, gt_img);
    strip.write();

    EthernetInterface eth;
#ifdef STATIC
    eth.init(ipAddr,snMask,gwAddr); //Use DHCP
#else
    eth.init();
#endif
    eth.connect();
    printf("\r\nIP Address is %s\r\n", eth.getIPAddress());
    
    
    
    
    
      
    
 
 /*   
    TCPSocketConnection sock;
    sock.connect("mbed.org", 80);
    
    char http_cmd[] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\n\n";
    sock.send_all(http_cmd, sizeof(http_cmd)-1);
    
    char buffer[300];
    int ret;
    while (true) {
        ret = sock.receive(buffer, sizeof(buffer)-1);
        if (ret <= 0)
            break;
        buffer[ret] = '\0';
        printf("Received %d chars from server:\n%s\n", ret, buffer);
    }
      
    sock.close();
    
    eth.disconnect();
    */
    
    
   
    InitArtPollReplyDefaults();
    
    ArtPollReply.PortType[0] = 128;
    ArtPollReply.PortType[1] = 128;
    ArtPollReply.PortType[2] = 64;
    ArtPollReply.GoodInput[2] = 4;
    ArtPollReply.PortType[3] = 64;
    ArtPollReply.GoodInput[3] = 4;
    
    UDPSocket server;
    server.bind(ECHO_SERVER_PORT);
    server.set_broadcasting();
    
    Endpoint client;
    
    char *RecvByte;
    char buffer[1500];
   // char outputString[512];
    
    
    int pos;
    int x,y;
    
    Thread t1(thread1); //start thread1
    //t1.set_priority(osPriorityRealtime);
    
    while (true) {
       // printf("\r\nWaiting for UDP packet...\r\n");
        int n = server.receiveFrom(client, buffer, sizeof(buffer));
        buffer[n] = '\0';
        
        
        memcpy(&ArtHead,buffer,sizeof(ArtPacketHeader));
        if(strncmp(ArtHead.ID, ArtHeaderID, 8) == 0 )
        {
            RecvByte = &buffer[10];
           // printf("Valid ArtNet packet\r\n");
        }
        
       // printf("\nID = %s \n",ArtHead.ID);
        
        switch(ArtHead.OpCode)
        {
            case OP_Output:
                //printf("OPCode = Output,%#x Size = %d\r\n",ArtHead.OpCode,n);
                ArtDMX.VersionH = RecvByte[0];
                ArtDMX.Version = RecvByte[1];
                ArtDMX.Sequence = RecvByte[2];
                ArtDMX.Physical = RecvByte[3];
                x = RecvByte[4];
                y = RecvByte[5];
                ArtDMX.Universe = makeword16(x,y);
                LastRecievedUniverse = ArtDMX.Universe;
                x = RecvByte[6];
                y = RecvByte[7];
                ArtDMX.Length = makeword16(y,x);
                //printf("VerH: %d, VerL: %d, Seq: %d, Phys: %d, Univ: %d, Length: %d\r\n",ArtDMX.VersionH,ArtDMX.Version,ArtDMX.Sequence,ArtDMX.Physical,ArtDMX.Universe,ArtDMX.Length); //used for debugging
                int byteLoc = 0;
                if(ArtDMX.Universe == 1)
                {
                     
                     for (pos = 0;pos<170;pos++)
                     {
                        byteLoc = (pos*3);
                        strip.setPixel(pos, RecvByte[8+byteLoc],RecvByte[9+byteLoc],RecvByte[10+byteLoc]);
                        //  printf("%d, ",RecvByte[8+pos])
                     }
                     
                }
                
                if(ArtDMX.Universe == 2)
                {
                   
                    for (pos = 170;pos<300;pos++)
                    {
                        byteLoc = ((pos-170)*3);
                        strip.setPixel(pos, RecvByte[8+byteLoc],RecvByte[9+byteLoc],RecvByte[10+byteLoc]);
                        
                    }
                }
             //   strip.write();
                break;
                
            case OP_Poll:
                printf("OpCode = Poll, %#x, Size: %d\r\n",ArtHead.OpCode,n);
                ArtPoll.VersionH = RecvByte[0];
                ArtPoll.Version = RecvByte[1];
                ArtPoll.TalkToMe = RecvByte[2];
                printf("Sending Reply\r\n");
                server.sendTo(client, (char*)&ArtPollReply, sizeof(ArtPollReply));
                printf("Sent Reply Complete\r\n");
                break;
            
            default:
                printf("No OPcode read, OPCode=%#x, Size: %d\r\n",ArtHead.ID,n);
                break;
        } 
            
        
        
       // printf("Received packet from: %s\r\n", client.get_address());
     //   printf("Packet contents : '%s'\r\n",buffer);
        //printf("Sending Packet back to Client\n");
        //server.sendTo(client, buffer, n);
       
    }


    while(1) {}
}
