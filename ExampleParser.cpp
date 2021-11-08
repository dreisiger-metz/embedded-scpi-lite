// =============================================================================
// Filename         : ExampleParser.cpp
// Version          : 
//
// Original Author  : Peter Metz
// Date Created     : 08-Nov-2021
//
// Revision History : 08-Nov-2021: Initial version
//
// Purpose          : To implement a simple shell-based example parser which
//                    demonstrates the basic requirements and functionality of
//                    the parser (without requiring an actual embedded target)
//                     
// Licensing        : Copyright (C) 2021, Peter Metz
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
// details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Notes:
//   - should probably change platform-dependent type declaration to fixed-
//     length ones
// =============================================================================
#include <stdio.h>
#include "Parser.h"

#define NUM_CHANS 8



// =============================================================================
// Class            : ExampleParser
//
// Purpose          : This class implements the overarching controller's command
//                    parser
// =============================================================================
class ExampleParser : public Parser {
  public:
    enum ChannelMode { INPUT,  INPUT_PULLUP, OUTPUT  };

    ExampleParser(char unitID) {
      Parser::CommandHandlers = (Parser::Handler *)&(Handlers);

      p_unitID = unitID;
      if (unitID == 7)
        p_active = true;
      else
        p_active = false;

      for (unsigned i = 0; i < NUM_CHANS; i++) {
        p_channel[i] = false;
        p_channelMode[i] = INPUT;
      }
    }
    inline bool active() { return p_active; }

  protected:
    // All of these commands should really be implemented if you're wanting an SCPI-like command
    // line interface
    void           ADDRHandler(const char *cmd, bool query, unsigned channel, const char *args);
    void            IDNHandler(const char *cmd, bool query, unsigned channel, const char *args);
    void            SAVHandler(const char *cmd, bool query, unsigned channel, const char *args);
    void            RCLHandler(const char *cmd, bool query, unsigned channel, const char *args);
    void            RSTHandler(const char *cmd, bool query, unsigned channel, const char *args);
    void            TRGHandler(const char *cmd, bool query, unsigned channel, const char *args);

    void           HELPHandler(const char *cmd, bool query, unsigned channel, const char *args);

    void     SystemAddrHandler(const char *cmd, bool query, unsigned channel, const char *args);
    void  SystemTriggerHandler(const char *cmd, bool query, unsigned channel, const char *args);

    // You wouldn't normally have a command like this in an embedded system
    void           QUITHandler(const char *cmd, bool query, unsigned channel, const char *args);

    // And a couple of example multi-channel handlers
    void        ChannelHandler(const char *cmd, bool query, unsigned channel, const char *args);
    void    ChannelModeHandler(const char *cmd, bool query, unsigned channel, const char *args);


    static const char NOTHING[];
    static const char QUERY_ONLY[];

    static const Handler Handlers[];

    char p_unitID;
    bool p_active;

    bool        p_channel[NUM_CHANS];
    ChannelMode p_channelMode[NUM_CHANS];
};


const char ExampleParser::NOTHING[]    = "";
const char ExampleParser::QUERY_ONLY[] = "(QUERY ONLY)";

const Parser::Handler ExampleParser::Handlers[] = {
//    Command           Command handler                                   Multi-channel?  Allowed values
    { "++ADDR",         (ParserHandler) &ExampleParser::ADDRHandler,              false, "[ 0 -- 7 ]" },
    { "*IDN",           (ParserHandler) &ExampleParser::IDNHandler,               false, QUERY_ONLY },
    { "*SAV",           (ParserHandler) &ExampleParser::SAVHandler,               false, NOTHING },
    { "*RCL",           (ParserHandler) &ExampleParser::RCLHandler,               false, NOTHING },
    { "*RST",           (ParserHandler) &ExampleParser::RSTHandler,               false, NOTHING },
    { "*TRG",           (ParserHandler) &ExampleParser::TRGHandler,               false, NOTHING },
    { "HELP",           (ParserHandler) &ExampleParser::HELPHandler,              false, QUERY_ONLY },
    { "ID",             (ParserHandler) &ExampleParser::IDNHandler,               false, QUERY_ONLY },
    { "SYST:ADDR",      (ParserHandler) &ExampleParser::SystemAddrHandler,        false, QUERY_ONLY },
    { "SYST:TRIG",      (ParserHandler) &ExampleParser::SystemTriggerHandler,     false, QUERY_ONLY },
    { "QUIT",           (ParserHandler) &ExampleParser::QUITHandler,              false, NOTHING },
    { "CHAN0",          (ParserHandler) &ExampleParser::ChannelHandler,           true,  "{ 0 | 1 | OFF | ON | LO | HI }" },
    { "CHAN0:MODE",     (ParserHandler) &ExampleParser::ChannelModeHandler,       true,  "{ INPUT | INPUT_PULLUP | OUTPUT }" },
    END_OF_HANDLERS
};




void ExampleParser::ADDRHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  unsigned id;

  if (!query) {
    sscanf(args, "%d", &id);
    p_active = (p_unitID == id);

    if (p_active)
      process("*IDN?");
  }
}


void ExampleParser::IDNHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active && query)
    printf("ENGINUITY.DE,EXAMPLEPARSER,000000,0.1-20211108\n");
}


void ExampleParser::SAVHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active && !query)
    printf("SAV handler called\n");
}


void ExampleParser::RCLHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active && !query)
    printf("RCL handler called\n");
}


void ExampleParser::RSTHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active && !query)
    printf("RST handler called\n");
}


void ExampleParser::TRGHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (!query)
    printf("TRG handler called (note TRG's are /not/ address-specific)\n");
}


void ExampleParser::HELPHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  char i = 0;
  
  if (p_active && query)
    while (Handlers[i].cmd) {
      printf("%s %s\n", Handlers[i].cmd,  Handlers[i].help);
      i++;
    }
}


void ExampleParser::SystemAddrHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active)
    printf("%d\n", p_unitID);
}


void ExampleParser::SystemTriggerHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active)
    printf("SYSTem:TRIGger handler called\n");
}


void ExampleParser::QUITHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active)
  printf("QUIT handler called\n");
  exit(1);
}


void ExampleParser::ChannelHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active && ((channel > 0) && (channel <= NUM_CHANS)))
    if (query) {
      printf("CHANnel query handler called for channel '%d' : state == '%s'\n", channel, (p_channel[channel - 1])?"HI":"LOW");

    } else {
      if (!strcasecmp(args, "0") || !strcasecmp(args, "OFF") || !strncasecmp(args, "LO", 2))
        printf("CHANnel handler called for channel '%d' : setting state to LOW\n", channel);
        p_channel[channel - 1] = false;
        
      if (!strcasecmp(args, "1") || !strcasecmp(args, "ON") || !strncasecmp(args, "HI", 2))
        printf("CHANnel handler called for channel '%d' : setting state to HI\n", channel);
        p_channel[channel - 1] = true;
    }
}


void ExampleParser::ChannelModeHandler(const char *cmd, bool query, unsigned channel, const char *args) {
  if (p_active && ((channel > 0) && (channel <= 8)))
    if (query) {
      switch(p_channelMode[channel - 1]) {
        case OUTPUT:
        printf("CHANnel MODE query handler called for channel '%d' : mode == 'OUTPUT'\n", channel);
        break;

        case INPUT_PULLUP:
        printf("CHANnel MODE query handler called for channel '%d' : mode == 'INPUT_PULLUP'\n", channel);
        break;

        case INPUT:
        printf("CHANnel MODE query handler called for channel '%d' : mode == 'INPUT'\n", channel);
        break;

        default:
        break;
      }

    } else {
      if (!strncasecmp(args, "OUT", 3)) {
        printf("CHANnel MODE handler called for channel '%d' : new mode == 'OUTPUT'\n", channel);
        p_channelMode[channel - 1] = OUTPUT;
        
      } else if (!strncasecmp(args, "INPUT_", 6) || !strncasecmp(args, "PUL", 3)) {
        printf("CHANnel MODE handler called for channel '%d' : setting mode to 'INPUT_PULLUP'\n", channel);
        p_channelMode[channel - 1] = INPUT_PULLUP;
        
      } else if (!strncasecmp(args, "IN", 2)) {
        printf("CHANnel MODE handler called for channel '%d' : setting mode to 'INPUT'\n", channel);
        p_channelMode[channel - 1] = INPUT;

      } else 
        printf("CHANnel MODE handler called for channel '%d' : unsupported mode '%s'\n", channel, args);

    }
}






// =============================================================================
int main(int argc, char **argv) {
  char command[255];
  ExampleParser *parser;
  
  parser = new ExampleParser(7);

  printf("*** created a new parser with ADDR == 7; enter commands or type HELP? for\n");
  printf("*** more information; to execute a query, append a '?' after the command\n");

  while (1) {
    scanf("%s", command);
    parser->process(command);
  }
  
  delete parser;

  return 0;
}
