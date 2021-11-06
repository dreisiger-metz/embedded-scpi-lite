// =============================================================================
// Filename         : Parser.h
// Version          : 
//
// Original Author  : Peter Metz
// Date Created     : 18-Jun-2020
//
// Revision History : 18-Jun-2020: v0.1, Initial version
//                    29-Jun-2020: v0.2, Modified to support matching on sub-
//                                 strings and channel pattern matching
//
// Purpose          : 
//                     
// Licensing        : Copyright (C) 2020, Peter Dreisiger
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
// =============================================================================
#ifndef Parser_h
#define Parser_h
#include <ctype.h>
#include <string.h>



// =============================================================================
// Class            : Parser
//
// Purpose          : This base class defines a minimal(ly) SCPI-compatible
//                    parser that is, perhaps, more suitable for use on memory-
//                    constrained micro-controller than, say, the SCPI parser
//                    library of http://j123b567.github.io/scpi-parser.
//
//                    In order to be useful, a derived class must be defined
//                    which, not only implements the language-specific handlers,
//                    but sets [CommandHandlers] to point to an array of com-
//                    mand-to-handling-method mappings.
//
// ----------------------------------------------------------------------------
// Notes:
//   - process(...) should probably be modified to return either the number of
//     characters processed or, more usefully, a pointer to the start of the
//     command to be processed (to support semicolon-delimited command lists)
// =============================================================================
class Parser {
  public:
    typedef void (Parser::*ParserHandler)(const char *cmd, bool query, unsigned channel, const char *args);
    struct Handler { const char *cmd; ParserHandler handler; bool supportsChannels; const char *help; };

    Parser() { CommandHandlers = NULL; }
    char *process(const char *in);

  protected:

    // 'temporary' variables used by Parser::process declared here rather than once per call
    Handler *CommandHandlers;
    static const Handler END_OF_HANDLERS;

    static const unsigned MAX_TOKEN_DEPTH = 8;
};


const Parser::Handler Parser::END_OF_HANDLERS = { NULL, (Parser::ParserHandler) NULL, false, NULL };




// =============================================================================
// Method           : Parser::process(...)
//
// Purpose          : To check [cmd] against the list of registered command-
//                    handlers, and if one is found, call it, passing through
//                    a flag indicating if it should be treated as a query, the
//                    requesting channel, and the argument string, if any
//
// Inputs           : [cmd] a null-terminated command string; note that no syn-
//                        tax-checking is performed here
//
// Returns          : the pointer to the next command(s), if any --- i.e. the
//                    sub-string following the first semicolon
// =============================================================================
char *Parser::process(const char *in) {
  bool query = false, matched = false;
  char i, j, inputTokenCount = 0, tplTokenCount, channel = 0;
  char *input, *inputFreeRef, *inputTokens[MAX_TOKEN_DEPTH], *args, *tpl, *tplFreeRef, *tplTokens[MAX_TOKEN_DEPTH];
  //unsigned firstInputCmdLength;

  // Process the input string, [in]
  // ------------------------------
  input = inputFreeRef = strdup(in);

  // separate out the arguments, if any
  args = input + (unsigned) strcspn(input, " ");
  if (*args == ' ')
    *args++ = NULL;

  // see if we've received a query
  if (input[strlen(input) - 1] == '?') {
    query = true;
    input[strlen(input) - 1] = NULL;
  }

  // tokenise the incoming command string
  while ((inputTokens[inputTokenCount++] = strsep(&input, ":")) != NULL);
  inputTokens[inputTokenCount] = NULL;


  // Work through [CommandHandlers] to see if we can find a matching template
  // ------------------------------------------------------------------------
  i = 0;
  while (CommandHandlers[i].cmd && !matched) {
    tplTokenCount = 0;
    tpl = tplFreeRef = strdup(CommandHandlers[i].cmd);

    // tokenise the (current) command template
    while ((tplTokens[tplTokenCount++] = strsep(&tpl, ":")) != NULL);
    tplTokens[tplTokenCount] = NULL;

    // if the template supports channels, remove the channel placeholder
    // prior to comparing tokens
    if (CommandHandlers[i].supportsChannels)
      tplTokens[0][strlen(tplTokens[0]) - 1] = NULL;

    // assuming the input and the template have the same number of tokens
    // (excluding any arguments), compare them
    j = 0;
    matched = (inputTokenCount == tplTokenCount);
    while (matched && tplTokens[j]) {
      matched = (inputTokens[j] && strcasestr(inputTokens[j], tplTokens[j]) == inputTokens[j]);
      j++;
    }

    if (matched) {
      if (CommandHandlers[i].supportsChannels)
        channel = inputTokens[0][strlen(inputTokens[0]) - 1] - '0';
      (this->*(CommandHandlers[i].handler))(in, query, channel, args);
    }

    free(tplFreeRef);
    i++;
  }

  free(inputFreeRef);

  // return (char *) (in + firstInputCmdLength);
  return NULL;
}

#endif

