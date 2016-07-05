//-----------------------------------------------------------------------------
// assb.c
//
// Aubesserungsbeispiel
//
// Group: 13527 study assistant Christoph Maurer
//
// Authors: Gerald Birngruber (1530438)
//          Stefan Antic      (1230765)
//          Marko Nikic       (1530878)
//
// Latest Changes: 16.02.2016 (by Gerald Birngruber/Stefan Antic)
//-----------------------------------------------------------------------------
//

//include C libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//define struct for various stats, position, counters,...
typedef struct 
{
  char *band_;
  int position_;
  int currentstate_;
  int errorcode_;
  int ruleToUse_;
  int breakPointState_[1024];
  char breakPointRead_[1024];
  char breakPointWrite_[1024];
  int breakPointPos_[1024];
  int countState_;
  int countPos_;
  int countRead_;
  int countWrite_;
} turing;

//define struct for the rules
typedef struct
{
  int state_;
  char read_;
  char write_;
  int nextstate_;
  char movement_;
} rules;

//-----------------------------------------------------------------------------
/// Initializise the struct with zeros.
///
/// @param turing Maschine: A Struct which contains the variables.
///
/// @return Maschine: Returns the struct with the initialized variables. 
//
turing init(turing Maschine)
{ 
  Maschine.position_ = 0;
  Maschine.currentstate_ = 0;
  Maschine.errorcode_ = 0;
  Maschine.ruleToUse_ = 0;
  Maschine.countState_ = 0;
  Maschine.countPos_ = 0;
  Maschine.countRead_ = 0;
  Maschine.countWrite_ = 0;
  
  return Maschine;
}

//-----------------------------------------------------------------------------
/// Realloc the band if the posion is out of the band and the fill the new space with empty signs (_)
///
/// @param turing Maschine: A Struct which contains the band to be allocated in case it is out of Range.
///
/// @return Maschine: Returns the struct with the realloceted and filled band. 
///                   If a out of memory error occured the errorcode is in the struct.
//
turing bandfiller(turing Maschine)
{
  int i; 
  char buffer[2048];
  while(Maschine.position_ >=  strlen(Maschine.band_) || Maschine.position_ < 0)
  {
    if(Maschine.position_ < 0)
    {
      strcpy(buffer , Maschine.band_);
      Maschine.band_ = realloc(Maschine.band_ , strlen(Maschine.band_) + 2);
      if (Maschine.band_ == NULL)
      {
        printf("[ERR] out of memory\n");
        Maschine.errorcode_ = 2;
        return Maschine;
      }
      Maschine.band_[strlen(Maschine.band_) + 1] = '\0';
      Maschine.position_ ++;
      for(i = 0 ; i < strlen(Maschine.band_) ; i++)
      {
        Maschine.band_[0] = '_';
        Maschine.band_[i+1] = buffer[i];
      }

    }
    else if(Maschine.position_ >= strlen(Maschine.band_))
    {
      Maschine.band_ = realloc(Maschine.band_ , strlen(Maschine.band_) + 2);
      if (Maschine.band_ == NULL)
      {
        printf("[ERR] out of memory\n");
        Maschine.errorcode_ = 2;
        return  Maschine;
      }
      Maschine.band_[strlen(Maschine.band_) + 1] = '\0';
      Maschine.band_[strlen(Maschine.band_)] = '_';
    }
  }
  return Maschine;
}

//-----------------------------------------------------------------------------
/// shows the rules with a promt an the next rule.
///
/// @param turing Maschine: A Struct which contains current state, position and the band.
/// @param rules data: A Struct which contains the rules which are read from the input file.
/// @param int numberof rules: Number of Rules for the Touringmaschien.
///
/// @return No return Value.
//
void list(turing Maschine, rules *Data,int numberofrules)
{
  int i;
  for (i=0;i<numberofrules;i++)
  {
    if (Data[i].state_ == Maschine.currentstate_ && Data[i].read_ == Maschine.band_[Maschine.position_]) 
    {
      printf(">>> ");
    }
    printf("%d %c -> %c %d %c\n",Data[i].state_ ,Data[i].read_,Data[i].write_,Data[i].nextstate_,Data[i].movement_);
  }
}

//-----------------------------------------------------------------------------
/// shows the current band from the first to the last non blank state and the read/write head as > <.
///
/// @param turing Maschine: A Struct which contains current state, position and the band.
///
/// @return No return Value.
//
void show(turing Maschine)
{
  int first;
  int last;
  int i;
  for (i = 0 ; i<strlen(Maschine.band_) ; i++)
  {
    if(Maschine.band_[i] != '_')
    {
      first = i;
      break;
    }
  }
  
  for (i = first ; i < strlen(Maschine.band_) ; i++)
  {
    if(Maschine.band_[i] != '_')
    {
      last = i;
    }
  }
  if(Maschine.position_ <= first)
  {
    for (i = Maschine.position_ ; i <= last ; i++)
    {
      if(Maschine.position_ == i)
      {
        printf(">%c<",Maschine.band_[i]);
      }
      else
      {
        printf("%c",Maschine.band_[i]);
      }
      if (i != last)
      {
        printf("|");
      }
    }
  }
  
  if (first != last)
  {
    if (Maschine.position_ >= last)
    {
      for (i = first ; i <= Maschine.position_ ; i++)
      {
        if(Maschine.position_ == i)
        {
          printf(">%c<",Maschine.band_[i]);
        }
        else
        {
          printf("%c",Maschine.band_[i]);
        }
        if (i != Maschine.position_)
        {
          printf("|");
        }
      }
    }
  }
  
  if (Maschine.position_  < last && Maschine.position_ > first)
  {
    for (i = first ; i <= last ; i++)
    {
      if(Maschine.position_ == i)
      {
        printf(">%c<",Maschine.band_[i]);
      }
      else
      {
        printf("%c",Maschine.band_[i]);
      }
      if (i != last)
      {
        printf("|");
      }
    }
  }
  printf("\n");
}

//-----------------------------------------------------------------------------
/// Finds the next rule which is to perform.
///
/// @param turing Maschine: A Struct which contains the band and the current position.
/// @param rules data: A Struct which contains the rules which are read from the input file.
/// @param in numberofrules: Number of Rules for the Touringmaschien.
///
/// @return Maschine: Returns the Struct which contains the number of the nexte to execute rule. 
///         If there is no next rule a errocode is in the Struct returned to stop the programm.
//
turing rulefinder(turing Maschine, rules *Data, int numberofrules)
{
  int i;
  for(i = 0 ; i < numberofrules ; i++)
    {
      if(Maschine.currentstate_ == Data[i].state_  && Maschine.band_[Maschine.position_] == Data[i].read_)
      {
        Maschine.ruleToUse_ = i;
        break;
      }
      else if(i == numberofrules - 1)
      {
        printf("machine stopped in state %d\n",Maschine.currentstate_);
        show(Maschine);
        Maschine.errorcode_ = 6;
      }
    }
  return Maschine; 
}

//-----------------------------------------------------------------------------
/// Performs 1 Rule of the Turingmaschine
///
/// @param turing Maschine: A Struct which contains the band and the current position and state.
/// @param rules data: A Struct which contains the rules which are read from the input file.
/// @param in numberofrules: Number of Rules for the Touringmaschien.
///
/// @return Maschine: Returns the Struct which contains the new current state and position
//
turing turingmaschine(turing Maschine, rules *Data, int numberofrules)
{
  Maschine.band_[Maschine.position_] = Data[Maschine.ruleToUse_].write_;
  Maschine.currentstate_ = Data[Maschine.ruleToUse_].nextstate_;
  switch (Data[Maschine.ruleToUse_].movement_)
    {
      case 'R':
      Maschine.position_ ++;
      break;
      case 'L':
      Maschine.position_ --;
      break;
    }
  Maschine = bandfiller(Maschine);
  Maschine = rulefinder(Maschine , Data , numberofrules);
  return Maschine;
}

//-----------------------------------------------------------------------------
/// Checks if a set breakpoint hits the current stats
///
/// @param turing Maschine: A Struct which contains the band and the current position and state and the breakpoints.
///
/// @return Maschine: Returns the Struct which contains the new number of state breakpoints and the other set state breakpoints
///                   If a set breakpoint hits the current state in the struct a errorcode 7 is returned to show that no further perform of rules should take place
//
turing checkBreakpointState(turing Maschine)
{
  int i;
  for(i = 0 ; i < Maschine.countState_; i++)
  {
    if(Maschine.breakPointState_[i] == Maschine.currentstate_)
    {
      Maschine.breakPointState_[i] = Maschine.breakPointState_[Maschine.countState_ - 1 ];
      Maschine.countState_ --;
      if(Maschine.countState_ > 0)
      {
        i = -1;
        Maschine.errorcode_ = 7;
        continue;
      }
      else
      {
        Maschine.errorcode_ = 7;
        return Maschine;
      }
    }
    
    if(Maschine.errorcode_ == 7)
    {
      return Maschine;
    }
  }
  return Maschine;
}

//-----------------------------------------------------------------------------
/// Checks if a set breakpoint hits the current position
///
/// @param turing Maschine: A Struct which contains the band and the current position and state and the breakpoints.
///
/// @return Maschine: Returns the Struct which contains the new number of state breakpoints and the other set state breakpoints
///                   If a set breakpoint hits the current state in the struct a errorcode 7 is returned to show that no further perform of rules should take place
//
turing checkBreakpointPos(turing Maschine)
{
  int i;
  for(i = 0 ; i < Maschine.countPos_; i++)
  {
    if(Maschine.breakPointPos_[i] == Maschine.position_)
    {
      Maschine.breakPointPos_[i] = Maschine.breakPointPos_[Maschine.countPos_ - 1 ];
      Maschine.countPos_ --;
      if(Maschine.countPos_ > 0)
      {
        i = -1;
        Maschine.errorcode_ = 7;
        continue;
      }
      else
      {
        Maschine.errorcode_ = 7;
        return Maschine;
      }
    }
    
    if(Maschine.errorcode_ == 7)
    {
      return Maschine;
    }
  }
  return Maschine;
}

//-----------------------------------------------------------------------------
/// Checks if a set breakpoint hits the written Symbole
///
/// @param turing Maschine: A Struct which contains the band and the current position and state and the breakpoints.
///
/// @return Maschine: Returns the Struct which contains the new number of state breakpoints and the other set state breakpoints
///                   If a set breakpoint hits the current state in the struct a errorcode 7 is returned to show that no further perform of rules should take place
//
turing checkBreakpointWrite(turing Maschine, rules *Data)
{
  int i;
  for(i = 0 ; i < Maschine.countWrite_; i++)
  {
    if(Maschine.breakPointWrite_[i] == Data[Maschine.ruleToUse_].write_)
    {
      Maschine.breakPointWrite_[i] = Maschine.breakPointWrite_[Maschine.countWrite_ - 1 ];
      Maschine.countWrite_ --;
      if(Maschine.countWrite_ > 0)
      {
        i = -1;
        Maschine.errorcode_ = 8;
        continue;
      }
      else
      {
        Maschine.errorcode_ = 8;
        return Maschine;
      }
    }
    
    if(Maschine.errorcode_ == 8)
    {
      return Maschine;
    }
  }
  return Maschine;
}

//-----------------------------------------------------------------------------
/// Checks if a set breakpoint hits the read Symbole
///
/// @param turing Maschine: A Struct which contains the band and the current position and state and the breakpoints.
///
/// @return Maschine: Returns the Struct which contains the new number of state breakpoints and the other set state breakpoints
///                   If a set breakpoint hits the current state in the struct a errorcode 7 is returned to show that no further perform of rules should take place
//
turing checkBreakpointRead(turing Maschine, rules *Data)
{
  int i;
  for(i = 0 ; i < Maschine.countRead_; i++)
  {
    if(Maschine.breakPointRead_[i] == Data[Maschine.ruleToUse_].read_)
    {
      Maschine.breakPointRead_[i] = Maschine.breakPointRead_[Maschine.countRead_ - 1 ];
      Maschine.countRead_ --;
      if(Maschine.countRead_ > 0)
      {
        i = -1;
        Maschine.errorcode_ = 8;
        continue;
      }
      else
      {
        Maschine.errorcode_ = 8;
        return Maschine;
      }
    }
    
    if(Maschine.errorcode_ == 8)
    {
      return Maschine;
    }
  }
  return Maschine;
}

//-----------------------------------------------------------------------------
/// Performs 1 Rule and shows this rule.
///
/// @param turing Maschine: A Struct which contains the band and the current position and state.
/// @param rules data: A Struct which contains the rules which are read from the input file.
/// @param in numberofrules: Number of Rules for the Touringmaschien.
///
/// @return Maschine: Returns the Struct which contains the new current state and position
//
turing step(turing Maschine, rules *Data, int numberofrules)
{
  printf("%d %c -> %c %d %c\n",Data[Maschine.ruleToUse_].state_  , Data[Maschine.ruleToUse_].read_ , Data[Maschine.ruleToUse_].write_ , Data[Maschine.ruleToUse_].nextstate_ , Data[Maschine.ruleToUse_].movement_);
  Maschine = turingmaschine(Maschine, Data, numberofrules);
  return Maschine;
}

//-----------------------------------------------------------------------------
/// Performs the rules as long as there is no breaktpoint or the last rule was performed
///
/// @param turing Maschine: A Struct which contains the band and the current position, state breakpoints and the couter for the brakpoints.
/// @param rules data: A Struct which contains the rules which are read from the input file.
/// @param in numberofrules: Number of Rules for the Touringmaschien.
///
/// @return Maschine: Returns the Struct which contains the new number of brakpoints, and a array whith this brakpoints. 
///                   Further there are the new state and position or the errorcode 6 which means the last state was reached.
//
turing forward(turing Maschine, rules *Data, int numberofrules)
{
  for(;;)
  {
    //Checks if a breakpoint was set, if one is finde the maschine stops and delets other set breakpoints wich also matches
    Maschine = checkBreakpointState(Maschine);
    Maschine = checkBreakpointPos(Maschine);
    if(Maschine.errorcode_ == 7)
    {
      Maschine = checkBreakpointWrite(Maschine, Data);
      Maschine = checkBreakpointRead(Maschine, Data);
      Maschine.errorcode_ = 0;
      return Maschine;
    }
    //Perform a rule and check if the last state was reached
    Maschine = turingmaschine(Maschine,Data, numberofrules);
    if(Maschine.errorcode_ == 6)
    {
      return Maschine;
    }
    //Checks if a breakpoint after the perfomed rule matches
    Maschine = checkBreakpointWrite(Maschine,Data);
    Maschine = checkBreakpointRead(Maschine,Data);
    if(Maschine.errorcode_ == 8)
    {
      Maschine = checkBreakpointState(Maschine);
      Maschine = checkBreakpointPos(Maschine);
      Maschine.errorcode_ = 0;
      return Maschine;
    }
  }
  return Maschine;
}

//-----------------------------------------------------------------------------
/// Mainfunction
/// - reads the file
/// - save the read lines in the structs turing Maschine and the read rules in rules Data
/// - parses for
///   - non deterministic Rules (When a state can be there for 2 Rules)
///   - checks if the 5 char of a rule is a R,L or 0
/// - prepare the band , if the current posiion ist out of the band with function bandfiller
/// - find the role to be done with funktion rulefinder. If no rule the program ends.
/// - waits for user input
///   Possible inputs:
///   - list                  : shows the rules for the turingmaschine with a promt at the currente state.
///   - step                  : perform one rule and shows the rule.
///   - show                  : shows the current band with a "> <" for the write/read head.
///   - continue              : performs rules till a breakpoint or the end of the maschine is reached.
///   - break state <state>   : creats a breakpoint which stops the maschine if the entered state is reached.
///   - break write <symbol>  : creats a breakpoint which stops the maschine bevore the entered symbole is written.
///   - break read <symbol>   : creats a breakpoint which stops the maschine after the entered symbole was read.
///   - break pos <position>  : creats a breakpoint which stops the maschine if the write/read Head is in the entered position.
///   - quit/EOF              : quits the programm in the current state.
///
/// @param int argc: Counts the number of entered parameters when the programm is started.
/// @param char *argv[]: Saves the parameters which are entered when the programm is started.
///
/// @return 0 : programm endet correct by quit, EOF or if there is no further state to do.
/// @return 1 : programm was startet with the wrong parameters. Right start is ./assb <file>\n.
/// @return 2 : Out of memory during runtime.
/// @return 3 : Parsing of input file failed. There is a formatting error
/// @return 4 : Reading the File Failed. Something went wrong whith the file or the path.
/// @return 5 : non-deterministic turing maschine. The entered turingmaschine is not deterministric. There are more than 1 rule for 1 state.
///
//
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("[ERR] usage: ./assb <file>\n");
    return 1;
  }

  //declaration
  turing Maschine;
  int i = 0 ;
  int j = 0;
  char buffer[1024];
  int finder;
  int noOfRules = 0;
  char commandline [100];
  char *command1;
  char *command2;
  char *command3;
  Maschine = init(Maschine);
  
  //read File
  FILE *textfile;
  textfile = fopen (argv[1] , "r");
  if (textfile == NULL)
  {
    printf("[ERR] reading the file failed\n");
    return 4;
  }
  
  //counts how much rules in the file
  while( (finder = fgetc(textfile)) != EOF)
  {
    if( finder == '\n')
    {
      noOfRules++;
    }
  }  
  noOfRules = noOfRules - 2;
  rewind(textfile);
  
  //scans for the first 3 lines which contains band, startposition and startstate 
  fscanf(textfile , "%s\n%d\n%d\n", buffer , &Maschine.position_ , &Maschine.currentstate_);
  Maschine.band_ = malloc(strlen(buffer) + 1);
  if(Maschine.band_ == NULL)
  {
    printf("[ERR] out of memory\n");
    free(Maschine.band_);
    return 2;
  }
  strcpy(Maschine.band_,buffer);
  Maschine.band_[strlen(buffer)] = '\0';

  //scans for the rules and writes them into the struct rules data
  rules Data[noOfRules];

  for(i = 0 ; i < noOfRules ; i++)
  {
    fscanf(textfile , "%d %c %c %d %c\n", &Data[i].state_ , &Data[i].read_ , &Data[i].write_ , &Data[i].nextstate_ , &Data[i].movement_);
  }
  fclose (textfile);
  //list(Maschine,Data,noOfRules);//TEST
  //parsing
  for(i = 0 ; i < noOfRules ; i++)
  {    
    switch (Data[i].movement_)
    {
      case 'R':
      break;
      case 'L':
      break;
      case '0':
      break; 
      default:
      printf("[ERR] parsing of input failed\n");
      free(Maschine.band_);
      return 3;
    }
  }
  for(i = 0 ; i < noOfRules - 1 ; i++)
  {
    for(j = i + 1 ; j < noOfRules ; j++)
    {
      if(Data[i].state_ == Data[j].state_ && Data[i].read_ == Data[j].read_)
      {
        printf("[ERR] non-deterministic turing machine\n"); 
        free(Maschine.band_);
        return 5; 
      }
    }
  }
  
  //band preperation if the position is out of the band
  Maschine = bandfiller(Maschine);
  if(Maschine.errorcode_ == 2)
  {
    free(Maschine.band_);  
    return 2;
  }
  //find the rule which is the next to execute
  Maschine = rulefinder(Maschine,Data,noOfRules);
  if(Maschine.errorcode_ == 6)
  {
    free(Maschine.band_);
    return 0;
  }

  //wait for user input , if EOF the programm is closed, else the input is seperated
  for(;;)
  {
    printf("esp> "); 
    if(fgets (commandline , 99 , stdin) == NULL || strncmp (commandline,"quit",4) == 0)
    {
      printf("Bye.\n");
      free(Maschine.band_);
      return 0;
    }
    command1 = strtok (commandline," ");
    command2 = strtok (NULL," ");
    command3 = strtok (NULL," ");
    
    //compares the userinput with the known commands
    if (strncmp (command1 , "list" , 4) == 0)
    {
      list(Maschine , Data , noOfRules);
    }
    
    if (strncmp (command1 , "show" , 4) == 0)
    {
      show(Maschine);
    }
    
    if (strncmp (command1 , "step" , 4) == 0)
    {
      Maschine = step(Maschine , Data , noOfRules);
      if (Maschine.errorcode_ == 6)
      {
        free(Maschine.band_);
        return 0;
      } 
      if (Maschine.errorcode_ == 2)
      {
        free(Maschine.band_);
        return 2;
      } 
    }
    
    if (strncmp (command1 , "continue" , 8) == 0)
    {
      Maschine = forward(Maschine,Data,noOfRules);
      if (Maschine.errorcode_ == 6)
      {
        free(Maschine.band_);
        return 0;
      } 
      if (Maschine.errorcode_ == 2)
      {
        free(Maschine.band_);
        return 2;
      } 
    }
    
    if (strncmp (command1 , "break" , 5) == 0 && strncmp (command2 , "state" , 4) == 0 && command3 != NULL)
    {
      Maschine.breakPointState_[Maschine.countState_] = atoi(command3);
      Maschine.countState_ ++;
    }
    
    if (strncmp (command1 , "break" , 5) == 0 && strncmp (command2 , "read" , 4) == 0 && command3 != NULL)
    {
      Maschine.breakPointRead_[Maschine.countRead_] = *command3;
      Maschine.countRead_++;
    }
    
    if (strncmp (command1 , "break" , 5) == 0 && strncmp (command2 , "write" , 5) == 0 && command3 != NULL)
    {
      Maschine.breakPointWrite_[Maschine.countWrite_] = *command3;
      Maschine.countWrite_ ++;
    }
    
    if (strncmp (command1, "break" , 5) == 0 && strncmp (command2 , "pos" , 3) == 0 && command3 != NULL)
    {
      Maschine.breakPointPos_[Maschine.countPos_] = atoi(command3);
      Maschine.countPos_ ++;
    }
  }
}
