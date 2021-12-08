
# AS modules for VCV Rack

AS is a collection of modules for [VCV Rack](https://vcvrack.com/) by Alfredo Santamaria, Need a custom work?, you can find me at [Hakken.com.mx](http://www.hakken.com.mx/).

AS Logo/Monogram Copyright (c) 2017 Alfredo Santamaria , All rights reserved.
Panel graphics in res/ are © 2017
Derivative works may not use in any way the AS logo or panel graphics including custom component graphics (knobs, switches, screws, caps, etc.).

*** If you want to pay for a licence to use the graphics on any derivative work (excluding the AS Logo/Monogram), contact me directly at ascustomworks@gmail.com ***

### Releases

AS is compatible with VCV Rack 2.0.X. releases, (see previous releases for 1.0.X souce files, 0.6.X and 0.5.X. binaries and source files).  You can download old V0.6.X and below Mac, Win and Linux binary files and source on the [Release Page](https://github.com/AScustomWorks/as/releases)

If you enjoy those modules you can support the development by making a donation, it will be appreciated!. Here's the link: [DONATE](https://www.paypal.me/frederius/)

Or just buy my paid plugins AS Drums n Filters & AS Seqs n Tools and get even more modules and fun.
Buy them on the [VCV plugin library](https://vcvrack.com/plugins.html#Alfredo%20Santamaria)

![AS](https://github.com/AScustomWorks/AS/blob/V2/AS-V2.jpg)

# AS modules

### V 2.0.2

Updated component grahpics to V2 look, some code cleanup and tooltips added to all the ports, switches and buttons, also readme.md and licence


### V 2.0.1 TriggersMKIII and DelayPlus Stereo updates

New look for the black and gray knobs

A small tweak on the FX knobs

TriggersMKIII textfields updated to V2 code(was missing from the V2.0.0 update, and crashing rack when present on a patch)

Delay Plus Stereo FX: Added a Clear Buffer switch and CV input for each channel (long time request)

Also added input, output and switches tip labels for this two modules, the rest will come later

### V 2.0.0 update for compatibility with VCV RACK V2

Graphics and code updated to work with V2

### V 1.0.5 update, final update for V1 plugin

ZCV2T Zero crossing CV to Trigger module, anlexmatos made the update to support CV polyphony

PhaserFx long old bleeding bug between phaser modules finally fixed

### V 1.0.4 update

Just a fix on the plugin.json file

### V 1.0.3 update

BPM Calc and BPM Calc 2: small code optimization

Delay Plus Stereo FX: Now when the Feedback or Color channels are linked, the right knob reflects the setting from the left knob

Super Drive FX Stereo module added

WaveShaper module: CV inputs to control shape and scale added, wave mod input added.

WaveShaper FX Stereo module added, same features as WaveShaper, but stereo.


### V 1.0.2 update

More graphic updates.

Now every knob tooltip shows the proper parameter value according to it's function.

BPM Clock got the BPM range switch removed to work ok with the tooltips, 30-300 BPM is now the default range. (Your old patches using BPM Clock will need to adjust the BPM setting).

### V 1.0.1 update

Small graphic refresh/fixes, output ports on every module are now gold colored to make easier to find them

### ADSR
Fundamental ADSR module. Mods: graphics, sliders instead of knobs to provide faster visual input.

### VCA
Fundamental VCA module. Mods: graphics, sliders instead of knobs to provide faster visual input, one input + lin/exp switch instead of two separate inputs.

V 0.5.3: Code fix, now the VCA module works ok when there's no envelope input present.

### QuadVCA/Mixer
AS VCA module x 4, plus mixer functionality (user request).

V 0.5.5: First release of this module.

### BPM Clock
Strum's Mental VCV Master Clock. Mods: graphics, reset trigger input and output.

V 0.5.2: all the trigger signals are 10v now, it seems that some other modules don't work fine with the correct voltages.

V 0.5.4: Fixed a reset signal issue.

V 0.5.5: 16th clock output now sends unipolar signal, just as the other outputs.

V 0.6.1: Now BPM Clock outputs a short length trigger signal, as most of the available clocks.

V 0.6.2: Small fix on the reset signal length.

V 0.6.5: Added a Regular/Extended switch to change the clock from 40-250 to 30-300 range. Had to add the switch instead of simply changing the default settings to avoid changing the bpm tempo on the patches already using the clock.

V 0.6.6: Added a CV input for the "RUN" switch.

V 0.6.7: Added a CV output for the "RUN" switch, sends a trigger signal on each press of the switch.

V 1.0.2: BPM Clock got the BPM range switch removed to work ok with the tooltips, 30-300 BPM is now the default range.

### BPM to delay/hz calculator
A BPM to delay/hz calculator to setup easier those nice delay effects.

V 0.6.4: First release of this module.

V 0.6.5: Added an external input to detect a BPM from a LFO or some other sources, BPM detection code based on Koralfx Beatovnik from Tomek Sosnowski, nice work Tomek!.

V 1.0.3: Small code optimization

### BPM to delay/ms calculator
New slimmed down BPM Delay/MS Calculator module, just like the old BPM to delay/hz calculator module but without the calc values display to free some HP space.

V 0.6.11: First release of this module.

V 1.0.3: Small code optimization

### 2 Channel Mixer
Same as 8 channel mixer, but 2 channels only for those small setups(OmriCohen request).

V 0.6.13: First release of this module.

### 4 Channel Mixer
Same as 8 channel mixer, but 4 channels only for those small setups(OmriCohen request).

V 0.6.13: First release of this module.

### 8 Channel Mixer
Fundamental/Autodafe mixer module. Mods: graphics, sliders for channel volume, stereo or mono output(L channel outputs L+R signal if R channel is not active). Now with main mix mute button. Beware, the default setting for each channel volume is at 70% instead of 0%.

V 0.5.2: added MIX L & R input to chain mixers without giving up 2 mixer channels.

### Multi 2x5
2x5 Signal multiplier.

V 0.5.7 Module size reduced to 5HP

V 0.6.13 New feature: If the input 2 is not active the second row of outputs takes the value of input 1 so the module becomes a multi 1 x 10, nice and simple idea! requested by MarcBroule.

### Merge 2x5
2x5 CV Signal merger.

V 0.5.7: First release of this module.

V 1.0.2 New feature: If the output 2 is not active the output 1 sums up the values of the second row of inputs too, so the module becomes a merge 10 x 1.

### Mono VU Meter
V 0.5.4, New module added, Mono VU Meter made to match the 8CH Mixer.

### Stereo VU Meter
V 0.5.3, New module added, Stereo VU Meter made to match the 8CH Mixer.

### 16-step Sequencer
Fundamental/Autodafe SEQ module. Mods: graphics, digital display to show the number of steps selected.

V 0.5.2: added digital display to show current sequence step so you can run/stop the sequence and tune in the current step.

V 0.5.3: added edit mode: manual trigger with current step selector buttons and blinking led light to highlight the current step. Send a row output to a NYSTHI Hot Tuna and enjoy precise step tuning!

V 0.5.4: Exposed the trigger mode settings into the panel (contextual menu is still there but it won't change the mode)

### TinySawish
RODENTMODULES MuO. Mods: graphics, smaller panel size.

V 0.5.4 Added dc blocker code, modified a bit the internal parameters.

V 0.5.5 Extendend the freq range 1 octave below.

V 0.5.7 Module size reduced to 4HP

V 0.6.10 Added a base frequency switch to change from A (original and default) to C (to match the current VCV standard).

### TinySine
VCV tutorial module. Mods: graphics, proper sine wave.

V 0.5.7 Module size reduced to 4HP

V 0.6.10 Added a base frequency switch to change from A (original and default) to C (to match the current VCV standard).

### TriLFO
Fundamental LFO module. Mods: graphics, controls stripped to the basics but you get 3 LFOS on the same space.

### AtNuVrTr Dual attenuverter module
Just like Befaco Attenuverter module but with added cv inputs to modulate both Attenueverter and Offset parameters.

V 0.6.1: First release of this module.

### Triggers REMOVED
A couple of manual trigger buttons, one ON/OFF, one temporary, both with 4 trigger outputs, trigger volts knob going from 1 to 10 v output.

(NOTICE: Triggers MKI will supersede Triggers, so Triggers will be removed from the plugin by v0.6 but you have time now to replace it on your current patches and keep everything working fine).

### Triggers MKI
A manual CV signal trigger module with latch and temporary triggers, volts knob lets you adjust the range from -10v to 10v output.

V 0.5.7: First release of this module.
V 0.6.1: Changed the volts range to -10v +10v and now the display shows positive values in green, and negative values in red.

### Triggers MKII
A manual CV signal temporary trigger module with labeling integrated, so you remember where the signal is going.

The labels list includes:
    "------", "MUTE","  SOLO"," RESET"," DRUMS","  KICK"," SNARE"," HIHAT","  CLAP","  PERC","BASS 1","BASS 2"," GTR 1",
    " GTR 2","LEAD 1","LEAD 2"," PAD 1"," PAD 2","CHORDS","  FX 1","  FX 2"," SEQ 1"," SEQ 2"," MIX 1"," MIX 2",    
    " AUX 1"," AUX 2","    ON","   OFF"," START","  STOP"," PAUSE","    UP","  DOWN","  LEFT"," RIGHT", "RUN"

V 0.5.7: First release of this module.

V 0.6.5: Added "RUN" label at the end of the list.

### Triggers MKIII
A manual CV signal temporary trigger module with labeling integrated, now you can type directly the labels you want to use. Also added a handy second EXT input.

V 0.6.9: First release of this module.

### ReScale
Utility module to convert from one voltage range to another, available inputs are:
 -5v/5v, 0v/5v, 0v/10v, 1V/OCT*.
 Available output signal options are: 
 -5v/5v, 0v/5v, -10v/10v, 0v/10v*
 *1V/OCT option is to convert any voltage source to 1 octave 1V/OCT range.


V 0.6.9: First release of this module.

### Steps
Strum's Mental VCV Counters module. Mods: graphics, 3 counters, up to 64 steps each, added reset lights to the buttons.

V 0.5.4: First release of this module.

V 0.5.5: code tweaks.

### Launch Gate
Delay the start of a flow of signals by a set number of clock ticks (TAOS request).

V 0.5.5: First release of this module.

V 0.6.5: Now it features soft mute, so you can use it both for audio and cv signals without any switching noise.

### Kill Gate
Cut the flow of signals after a set number of clock ticks (TAOS request).

V 0.5.6: First release of this module.

V 0.6.5: Now it features soft mute, so you can use it both for audio and cv signals without any switching noise.

### Flow
Cut the flow of signals with a switch or a cv signal  (TAOS request).

V 0.6.0: First release of this module.

V 0.6.5: Now it features soft mute, so you can use it both for audio and cv signals without any switching noise.

V 0.6.9: Switch selector added to reverse the default ON/OFF swtiches light behavior (by user request).

### Signal Delay
Delay the incomming CV signal by set milliseconds, with signal thru and delayed output. You can chain several Signal Delay modules together for unlimited length of delays. (TAOS request).

V 0.5.5: First release of this module.

V 0.6,9: Small fix on the ms CV input to take the proper values fron BPM calc module.

V 1.0.1: Added a switch to each thru port to change the output from pre/post delay (Omri Cohen request)

### CV 2 T
CV to Trigger module. Feed a midi signal to the CV inputs and it will output one trigger signal when the incoming signal rises above 0v, and another trigger signal when the incoming signal returns to 0v. Useful to use your external hardware controller/keyboard as a trigger.

V 0.6.7: First release of this module.

### Zero Crossing CV 2 T
Zero crossing CV to Trigger module. Each time a CV input hits 0v it will generate a trigger signal on the respective output. User request.

V 0.6.10: First release of this module.

### Delay Plus
Fundamental Delay module. Mods: graphics, digital display to show delay time in MS , wet signal send & return, bypass switch.

V 0.5.4: Updated look.

V 0.5.5 CHanged the time knob reading from exponential to linear, now you can set any value precisely at the whole 1 to 10k ms range.

V 0.6.3: bypass CV input added.

V 0.6.5: Now it features soft bypass to avoid switching noise.

### Delay Plus Stereo
Stereo version of the Delay Plus module, with link switches for Feedback and Color parameters. If the respective switch is active, the left knob controls the changes for both left and right channels.

V 0.6.7: First release of this module.

V 1.0.3: Now when the Feedback or Color channels are linked, the right knob reflects the setting from the left knob

### Phaser
Autodafe's Phaser Fx module. Mods: graphics, bypass switch.

V 0.5.4: Added CV inputs for each parameter, updated look.

V 0.6.3: bypass CV input added.

V 0.6.5: Now it features soft bypass to avoid switching noise.

### SuperDrive
Overdrive/clipping Fx module with DRIVE, TONE and GAIN parameters, to get those acid bass lines we all love!.

V 0.5.4: First release of this module.

V 0.6.3: bypass CV input added.

V 0.6.5: Now it features soft bypass to avoid switching noise(Results may change according to signal levels).

### SuperDrive Stereo
Stereo Overdrive/clipping Fx module with DRIVE, TONE and GAIN parameters, to get those acid bass lines we all love!.

V 1.0.3: First release of this module.

### Reverb
Reverb Fx module based on ML_modules reverb and Freeverb code, with DECAY, DAMP and BLEND parameters, a little bit tamed Reverb with mixed output signal.

0.5.4: First release of this module.

V 0.6.3: bypass CV input added.

V 0.6.5: Now it features soft bypass to avoid switching noise.

### Reverb Stereo
Stereo version of the Reverb module, BLEND is replaced with a DRY /WET knob, to work better when used with a mixer send/return ports.

V 0.6.7: First release of this module.

### Tremolo
Tremolo Fx module with SHAPE, SPEED and BLEND parameters, and a phase switch (set your effect, duplicate the module and invert the phase for stereo tremolo setup) your Tremolo to go!.

0.5.6: First release of this module.

V 0.6.3: bypass CV input added.

V 0.6.5: Now it features soft bypass to avoid switching noise.

### Tremolo Stereo
Stereo version of the Tremolo module, use the phase switch to change from synced L and R channels to inverted phase, to get stereo panning effect.

V 0.6.7: First release of this module.

### WaveShaper
HetrickCV Wave Shaper module. Mods: graphics, bypass switch.

V 0.5.2: added back the voltage range switch.

V 0.5.4: Updated look.

V 0.6.3: bypass CV input added.

V 0.6.5: Now it features soft bypass to avoid switching noise.

V1.0.3: CV inputs to control shape and scale added, wave mod input added. Use the wave mod input to feed adifferent OSC or LFO signal to modulate the shape, use the CV inputs to get even more variations on the resulting sound.

### WaveShaper Stereo
Like WaveShaper module, but Stereo.

WaveShaper module: CV inputs to control shape and scale added, wave mod input added.

V 1.0.3: First release of this module.

### Blank Panel
Blank panels in 4, 6 & 8 HP.

New special "blank" panel 8 HP added for v0.6.0.

### Have fun!
