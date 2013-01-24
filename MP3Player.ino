

// simple rMP3 playback, plays for 5 seconds, and then stop for 5 seconds, looping this pattern continuously
// open serial terminal at 9600 to view playback status
 
    #include "RogueMP3.h"
    #include <SoftwareSerial.h>
    #include "RogueSD.h"
    #include <LiquidCrystal.h>
     
    SoftwareSerial rmp3_serial(6, 7);
     
    RogueMP3 rmp3(rmp3_serial);
    RogueSD filecommands(rmp3_serial);
    
    LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
     
    int numberOfSongs;
    int lastSong;
    char path[96];
     
    const char *directory = "";
     
     
    void setup()
    {
      Serial.begin(9600);
     
      Serial.println("Setting up lcd and mp3 player.");
      
      lcd.clear();
      // set up the LCD's number of columns and rows:
      lcd.begin(16, 2);
     
      rmp3_serial.begin(9600);
     
      rmp3.sync();
      rmp3.stop();
     
      filecommands.sync();
     
      // mix up our random number generator
      randomSeed(analogRead(0));
     
      // get the number of songs available
      strcpy(path, directory);
      strcat(path, "/");
      strcat(path, "*.mp3");  // we have to do this because the IDE thinks that "/*" needs to be terminated everywhere
     
      numberOfSongs = filecommands.filecount(path);
     
      lastSong = -1;
    }
     
    void playNextSong()
    {
      uint8_t s, i;
      char filename[80];
      char path[96];
      int nextSong = 0;
     
      if (numberOfSongs > 0)
      {
        // Select our next song randomly
        if (numberOfSongs > 2)
        {
          do
            nextSong = random(numberOfSongs);
          while (nextSong == lastSong);
        }
        else if (numberOfSongs == 2)
        {
          // we only have two songs
          if (lastSong == 0)
            nextSong = 1;
          else
            nextSong = 0;
        }
     
        // now, get our file name from file list
     
        filecommands.opendir(directory);
     
        for (i = 0; i <= nextSong; i++)
        {
          filecommands.readdir(filename, "*.mp3");
        }
     
        strcpy(path, directory);
        strcat(path, "/");
        strcat(path, filename);
     
        rmp3.playfile(path);
     
        Serial.print("Playing: ");
        Serial.println(path);
     
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(filename);
     
        lastSong = nextSong;
      }
      else
      {
        Serial.println("No files to play.");
      }
    }
     
     
    void loop()
    {
      char c;
      uint8_t i;
      uint8_t lastvolume = 20;
      int16_t newtime;
     
      playbackinfo playinfo;
     
      char status = 'S';
      uint8_t playing = 1;
      uint8_t volume = 20;
      uint8_t boostOn = false;
     
      volume = rmp3.getvolume();  // this only gets the left volume
     
      playNextSong();
     
      while(1)
      {
        while(!Serial.available())
        {
          // we should do fancy stuff like flash lights on our Xmas tree here!
          // got lots of time!
          delay(200);
     
          status = rmp3.getplaybackstatus();
          playinfo = rmp3.getplaybackinfo();
     
          if (status == 'S' && playing)
            playNextSong();
        }
     
        // OOH!! got a character!
        c = Serial.read();
     
        switch(c)
        {
          case 'p':
            // pause
            if(status == 'D')
            {
              // fade in
              rmp3.playpause();
              rmp3.fade(volume, 400);
            }
            else if(status == 'P')
            {
              // fade out
              rmp3.fade(100, 400);
              rmp3.playpause();
            }
            else
            {
              // start playing
              playNextSong();
              playing = 1;
            }
            break;
          case 's':
            rmp3.stop();
            playing = 0;
            break;
          case 'n':
            playNextSong();
            playing = 1;
            break;
     
          case 'k':
            if(volume < 254) volume++;
            if(status != 'D') rmp3.setvolume(volume);
            break;
     
          case 'i':
            if(volume > 0) volume--;
            if(status != 'D') rmp3.setvolume(volume);
            break;
     
        }
      }
    }


