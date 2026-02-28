# Woodpecker
ESP32-C3 drum machine

<figure class="half">
  <table style="border-collapse: collapse; border: none; border-spacing: 30px;" width="100%">
    <tr>
      <td>
        <img src="photo/11.jpg">
      </td>
      <td>
        <img src="photo/12.jpg">
      </td>
    </tr>
  </table>
</figure>

## Things that you need for this:

<ul>
<li>ESP32-C3-supermini</li>
<li>2 x MAX7219 LED-matrix</li>
<li>SSD1306 OLED display 128x32 with i2c</li>
<li>DFPlayerMini for playing samples from SD card (You can copy all folders from SAMPLES to root of SD card)</li>
<li>5 buttons</li>
 <li>Speaker and (or) audio 3.5mm</li>
</ul>


<img src="scheme.png">
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
<h2>Menu allows you:</h2>
 <ul>
  <li><b>BPM</b> - change the BPM</li>
  <li><b>Kit</b> - switch to a different set of samples (it is in different folders on SD card)</li>
  <li><b>Swing</b> - add some swing when playing</li>
  <li><b>Volume</b> - set global volume</li>
   <li><b>Pattern</b> - select pattern (now there is 4 patterns)</li>
   <li><b>Preset</b> - select preset for patterns (see presets.c)</li>
</ul> 

The LED matrix is ​​composed of two MAX 7219 modules. 
Eight tracks of different drum samples are arranged vertically. 
There are also four patterns that automatically follow each other during playback. 
You can switch between these patterns by simply moving the cursor or from the menu.

> [!NOTE]
> dfplayer can't play two files at the same time! So all the drums sound plays one after the other, but it still sounds very good and helps in musical rehearsals.

For simple and effective OLED library thanks [quackonaut](https://github.com/quackonauty/ESP-IDF-ESP_SSD1306)

<h2><b>Installation:</b></h2>
<p><b>You can compile all this stuff in vscode using PlatformIO and ESP-IDF framework.</b></p>




## TODO
- [ ] is it possible change UART speed to 115200? Test [DFPlayer Pro](https://wiki.dfrobot.com/DFPlayer_PRO_SKU_DFR076)
- [ ] Sometimes false set drums when use combination keys!
- [ ] add startup animation on MAX7219
- [ ] add random generation drums
- [ ] add woodpecker animation when start playing




