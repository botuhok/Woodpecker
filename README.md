# Woodpecker
ESP32-C3 drum machine

<img src="scheme.png">
Things that you need for this:
<ul>
<li>ESP32-C3-supermini</li>
<li>LED matrix for sets drums samples on grid</li>
<li>SSD1306 display</li>
<li>DFPlayer for playing samples from SD card (You can copy all folders from SAMPLES to root of SD card)</li>
<li>5 buttons</li>
</ul>
<h2>Keys combinations:</h2>
<ul>
<li><b>UP, DOWN, LEFT, RIGHT</b> Move cursor</li>
<li><b>ENTER</b> Set/unset drum</li>
<li><b>ENTER + UP</b> Start playback</li>
<li><b>ENTER + DOWN</b> Enter in menu</li>
<li><b>ENTER + LEFT</b> Decrease column volume</li>
<li><b>ENTER + RIGHT</b> Increase column volume</li>
</ul>
The volume of each column is displayed on the OLED display when you are not in menu.
<h2>The menu allows you:</h2>
 <ul>
  <li><b>BPM</b> - change the BPM</li>
  <li><b>Kit</b> - switch to a different set of samples (it is in different folders on SD card</li>
  <li><b>Shuffle</b> - add some swing</li>
  <li><b>Volume</b> - set global volume</li>
   <li><b>Pattern</b> - select pattern (now there is 4 patterns)</li>
   <li><b>Preset</b> - select preset for patterns (see presets.c)</li>
</ul> 

The LED matrix is ​​composed of two MAX 7219 modules. 
Eight tracks of different drum samples are arranged vertically. 
There are also four patterns that automatically follow each other during playback. 
You can switch between these patterns by simply moving the cursor or from the menu.

dfplayer can't play two files at the same time!
So all the drums sound plays one after the other, but it still sounds very good and helps in musical rehearsals.

You can compile all this stuff in vscode using PlatformIO and ESP-IDF library.
For simple and effective OLED library thanks https://github.com/quackonauty/ESP-IDF-ESP_SSD1306

<h2><b>TODO</b></h2>
<ul>
  <li>is it possible change UART speed to 115200?      // Maybe use DFPlayer Pro? https://wiki.dfrobot.com/DFPlayer_PRO_SKU_DFR0768</li>
  <li>add animation in startup function</li>
  <li>adjust volume on samples and may be choose better samples</li>
  <li>False set drums when use combination keys !</li>
</ul>



