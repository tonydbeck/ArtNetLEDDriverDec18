#include "mbed.h"
#include "EthernetInterface.h"

#define ArtMaxUniv 4 // Universe
#define SizeRecvBuffer 700
#define ArtHeaderID "Art-Net" // packet header
#define ArtUDPPort 0x1936 // UDP port 6454 for Art-Net
#define ArtVersion 14    // Art-Net version
#define OP_Output 0x5000 //Art-Net DMX Packet 'Output'
#define OP_Poll 0x2000 // ArtPoll
#define OP_PollReply 0x2100 // ArtPoll Reply
#define StyleNode 0
#define StyleServer 1

#define STR_LongName "Art-Net to WS2812B - By Tony Beck"
#define STR_ShortName "MBED ArtNet LED"

// host to network short
#define htons( x ) ( (( (x) << 8 ) & 0xFF00) | (( (x) >> 8 ) & 0x00FF) )
#define ntohs( x ) htons(x)
// host to network long
#define htonl( x ) ( (( (x) << 24 ) & 0xFF000000)  \
                   | (( (x) <<  8 ) & 0x00FF0000)  \
                   | (( (x) >>  8 ) & 0x0000FF00)  \
                   | (( (x) >> 24 ) & 0x000000FF)  )
#define ntohl( x ) htonl(x)

struct ArtAddr {
     unsigned char IP[4]; // ip addess 0.1.2.3
     unsigned short Port;
} __attribute__((packed));

struct ArtPacketHeader {
     char ID[8];
     unsigned short OpCode; // 0x5000
} __attribute__((packed));


// dmx transport packet
struct ArtDMX_Packet {
     char ID[8];
     unsigned short OpCode; // 0x5000
     unsigned char VersionH; // 0
     unsigned char Version; // 14
     unsigned char Sequence; // 0
     unsigned char Physical; // 0
     unsigned short Universe;
     unsigned short Length; // size of data segment
     unsigned char Data[512]; // data segment
} __attribute__((packed));

struct ArtPoll_Packet {
     char ID[8];
     unsigned short OpCode; // 0x5000
     unsigned char VersionH; // 0
     unsigned char Version; // 14
     unsigned char TalkToMe; // 0
} __attribute__((packed));


// a responce to a artpoll packet
struct ArtPollReply_Packet {
     char ID[8];
     unsigned short OpCode; // 0x2000
     struct ArtAddr Addr; // our ip address and UDP port
     unsigned char VersionH;
     unsigned char Version;
     unsigned char SubSwitchH;
     unsigned char SubSwitch;
     unsigned short OEM;
     char UbeaVersion;
     char Status;
     unsigned short EstaMan;
     char ShortName[18];
     char LongName[64];
     char NodeReport[64];
     unsigned char NumPortsH;
     unsigned char NumPorts;
     unsigned char PortType[4];
     unsigned char GoodInput[4];
     unsigned char GoodOutput[4];
     unsigned char Swin[4];
     unsigned char Swout[4];
     unsigned char SwVideo;
     unsigned char SwMacro;
     unsigned char SwRemote;
     unsigned char Spare[3]; // three spare bytes
     unsigned char Style;
     unsigned char Mac[6];
     unsigned char Padding[32]; // padding
} __attribute__((packed));



